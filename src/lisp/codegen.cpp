#include "bolt_virtual_machine/vm.hpp"
#include <cstdint>
#include <format>
#include <lisp/codegen.hpp>
#include <bolt_virtual_machine/emitter.h>

namespace Lisp {


    const std::vector<std::unique_ptr<BVM::FuncObj>>& Compiler::get_objs() { return func_objs_; }

    Compiler::Compiler() {}


    void Compiler::compile(const Lambda* program) {
        compile_lambda(program);
    }

    unsigned int Compiler::compile_expr(const ASTNode* node) {
        auto fo = active_objs_.top();
        auto scope = active_scopes_.top();
        unsigned int reg;
        if (node->get_type() == NodeType::Atomic) {
            const AtomicNode* atom = static_cast<const AtomicNode*>(node);
            if (atom->get_value()->get_type() != SExprType::SymbolLiteral)
                reg = fo->next_reg++;
            else {
                const std::string& name = static_cast<const StringAtom*>(atom->get_value())->get_value();
                reg = scope->lookup(name)->reg;
            }
            compile_atom(atom);
        }
        else {
            reg = fo->next_reg++;
            compile_list(node);
        }
        return reg;
    }
    /* (define var expr) */
    void Compiler::compile_define(const Define* node) {
        auto fo = active_objs_.top();
        unsigned int r1;
        auto scope = active_scopes_.top();
        const ASTNode* expr = node->get_expr();
        r1 = compile_expr(expr);
        uint8_t dst = scope->lookup(node->get_id())->reg;
        fo->instructions.push_back(BVM::Emitter::mov(dst, r1));
        dealloc_expr(expr);
    }

    void Compiler::compile_lambda(const Lambda* node) {
        auto nfo = std::make_unique<BVM::FuncObj>();
        auto& params = node->get_parameters();
        int arity = params.size();
        int n_locals = node->get_const_scope().symbol_table.size();
        BVM::FuncObj* ptr = nfo.get();
        ptr->n_locals = n_locals;

        // pre-allocate virtual registers for variables
        for (auto&[_, v]: node->get_const_scope().symbol_table) {
            if (v.type == SymbolType::Variable)
                ptr->next_reg++;
        }

        active_objs_.push(ptr);
        active_scopes_.push(&node->get_const_scope());
        func_objs_.push_back(std::move(nfo));

        for (auto& e : node->get_exprs()) {
            compile_expr(e.get());
        }
        
        ptr->next_reg = arity + n_locals;
        active_objs_.pop();
    }

    void Compiler::compile_list(const ASTNode* node) {
        switch(node->get_type()) {
            case NodeType::Define:
                compile_define(static_cast<const Define*>(node));
                break;
            case NodeType::IfExpr:
                compile_if(static_cast<const IfExpr*>(node));
                break;
            case NodeType::Lambda:
                compile_lambda(static_cast<const Lambda*>(node));
                break;
            case NodeType::ProcCall:
                compile_proc_call(static_cast<const ProcCall*>(node));
                break;
            default:
                throw std::runtime_error("compile_list: Not Implemented");
        }

    }

    void Compiler::compile_atom(const AtomicNode* node) {
        auto fo = active_objs_.top();
        uint32_t inst;
        BVM::BoltValue value;
        switch(node->get_value()->get_type()) {
            case SExprType::BoolLiteral:
                value.as_bool = static_cast<const BoolAtom*>(node->get_value())->get_value();
                value.type = BVM::BoltType::Boolean;
                break;
            case SExprType::FloatLiteral:
                value.as_double= static_cast<const FloatAtom*>(node->get_value())->get_value();
                value.type = BVM::BoltType::Float;
                break;
            case SExprType::IntLiteral:
                value.as_int = static_cast<const IntAtom*>(node->get_value())->get_value();
                value.type = BVM::BoltType::Integer;
                break;
            case SExprType::SymbolLiteral:
                return;
            default:
                throw std::logic_error("unsupported atomic value");
        }
        size_t n_consts = fo->consts.size();
        size_t i;

        // linear search is terrible but boltvalue is not hashable (yet)
        for (i = 0; i < n_consts; i++) {
            if (fo->consts[i] == value)
                break;
        }

        if (i == n_consts) {
            fo->consts.push_back(value);
        }

        inst = BVM::Emitter::load_const(fo->next_reg - 1, i);
        fo->instructions.push_back(inst);
    }

    void Compiler::compile_if(const IfExpr* node) {
        auto fo = active_objs_.top();
        unsigned int r1, r2, r3, if_reg = fo->next_reg;
        size_t if_pos, else_pos;

        r1 = compile_expr(node->get_cond()); 
        // reserve space for the instruction
        fo->instructions.push_back(0);
        if_pos = fo->instructions.size();
        r2 = compile_expr(node->get_texpr());
        fo->instructions.push_back(0);
        else_pos = fo->instructions.size();
        r3 = compile_expr(node->get_fexpr());
        fo->instructions[else_pos - 1] = BVM::Emitter::mov(if_reg, r2);
        fo->instructions.push_back(BVM::Emitter::mov(if_reg, r3));
        fo->instructions[if_pos - 1] = BVM::Emitter::jmp_if_false(r1, else_pos - if_pos);
        fo->next_reg = if_reg;
        dealloc_expr(node->get_cond());
        dealloc_expr(node->get_texpr());
        dealloc_expr(node->get_fexpr());
    }

    void Compiler::compile_proc_call(const ProcCall* node) {
        auto fo = active_objs_.top();
        const Scope* scope = active_scopes_.top();
        auto atom = node->get_proc()->get_value();
        const std::string& name = static_cast<const SymbolAtom*>(atom)->get_value();

        for (auto& arg : node->get_args()) {
            compile_expr(arg.get());
        }

        if (scope->lookup(name)->type == SymbolType::NativeProc) {
            fo->instructions.push_back(BVM::Emitter::call_native(fo->next_reg, node->get_args().size(), 
                        static_cast<uint8_t>(native_funcs.at(name))));
        } else {
            fo->instructions.push_back(BVM::Emitter::call(fo->next_reg));
        }

        for (auto& arg : node->get_args()) {
            dealloc_expr(arg.get());
        }
    }
}
