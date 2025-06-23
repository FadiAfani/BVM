#include "bolt_virtual_machine/vm.hpp"
#include <cstdint>
#include <lisp/codegen.hpp>
#include <bolt_virtual_machine/emitter.h>

namespace Lisp {

    static void encode_const(FuncObj* obj, const void* data, size_t size) {
        assert(obj != nullptr);

        obj->consts.reserve(size);
        const uint8_t* ptr = static_cast<const uint8_t*>(data);
        for (size_t i = 0; i < size; i++) {
            //obj->consts.push_back((ptr[i] >> i * 8) & 0xFF);
        }
    }

    const std::vector<std::unique_ptr<FuncObj>>& Compiler::get_objs() { return func_objs_; }

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
        unsigned int r1, prev_reg_state = fo->next_reg;
        auto scope = active_scopes_.top();
        r1 = compile_expr(node->get_expr());
        uint8_t dst = scope->lookup(node->get_id())->reg;
        fo->instructions.push_back(BVM::Emitter::mov(dst, r1));
        fo->next_reg = prev_reg_state;
    }

    void Compiler::compile_lambda(const Lambda* node) {
        auto nfo = std::make_unique<FuncObj>();
        auto& params = node->get_parameters();
        int arity = params.size();
        int n_locals = node->get_const_scope().symbol_table.size();
        FuncObj* ptr = nfo.get();
        ptr->n_locals = n_locals;
        ptr->next_reg = arity + n_locals;

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
        auto fo = active_objs_.top();
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
            case NodeType::BinaryExpr:
                compile_binary(static_cast<const BinaryExpr*>(node));
                break;
            default:
                throw std::runtime_error("compile_list: Not Implemented");
        }

        fo->next_reg--;
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
        if (!fo->consts.contains(value))
            fo->consts[value] = n_consts;
        inst = BVM::Emitter::load_const(fo->next_reg - 1, fo->consts[value]);
        fo->instructions.push_back(inst);
    }

    /* (if cond if-expr else-expr) */
    void Compiler::compile_if(const IfExpr* node) {
        auto fo = active_objs_.top();
        unsigned int r1, r2, r3, prev_reg_state = fo->next_reg;
        // compile condition
        r1 = compile_expr(node->get_cond()); 
        // reserve space for the instruction
        fo->instructions.push_back(0);
        size_t if_pos = fo->instructions.size();
        r2 = compile_expr(node->get_texpr());
        fo->instructions.push_back(BVM::Emitter::mov(prev_reg_state - 1, r2));
        size_t else_pos = fo->instructions.size();
        r3 = compile_expr(node->get_fexpr());
        fo->instructions.push_back(BVM::Emitter::mov(prev_reg_state - 1, r3));
        fo->instructions[if_pos - 1] = BVM::Emitter::jmp_if_false(r1, else_pos - if_pos);
        fo->next_reg = prev_reg_state;
    }

    void Compiler::compile_binary(const BinaryExpr* node) {
        auto fo = active_objs_.top();
        unsigned int r1, r2, prev_reg_state = fo->next_reg;
        r1 = compile_expr(node->get_left());
        r2 = compile_expr(node->get_right());
        uint32_t inst;
        switch(node->get_op()) {
            case ExprType::Plus:
                inst = BVM::Emitter::add(prev_reg_state - 1, r1, r2);
                break;
            case ExprType::Minus:
                inst = BVM::Emitter::sub(prev_reg_state - 1, r1, r2);
                break;
            case ExprType::Div:
                inst = BVM::Emitter::div(prev_reg_state - 1, r1, r2);
                break;
            case ExprType::Mul:
                inst = BVM::Emitter::mul(prev_reg_state - 1, r1, r2);
                break;
            case ExprType::Bte:
                inst = BVM::Emitter::bte(prev_reg_state - 1, r1, r2);
                break;
            case ExprType::Bt:
                inst = BVM::Emitter::bt(prev_reg_state - 1, r1, r2);
                break;
            case ExprType::Lte:
                inst = BVM::Emitter::lte(prev_reg_state - 1, r1, r2);
                break;
            case ExprType::Lt:
                inst = BVM::Emitter::lt(prev_reg_state - 1, r1, r2);
                break;
            case ExprType::Eq:
                inst = BVM::Emitter::eq(prev_reg_state - 1, r1, r2);
                break;
            case ExprType::Ne:
                inst = BVM::Emitter::ne(prev_reg_state - 1, r1, r2);
                break;
            default:
                throw std::logic_error("not a valid binary operation");
        }
        fo->instructions.push_back(inst);
        fo->next_reg = prev_reg_state;

    }

}
