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

    Compiler::Compiler(std::unique_ptr<AnnotatedProgram> program) : program_(std::move(program)) {

    }

    const std::vector<std::unique_ptr<FuncObj>>& Compiler::get_objs() { return func_objs_; }


    void Compiler::compile() {
        auto ep = std::make_unique<FuncObj>();
        auto globals = program_->globals.get();
        ep->n_locals = globals->symbol_table.size();
        ep->next_reg = ep->n_locals;
        active_objs_.push(ep.get());
        active_scopes_.push(globals);
        func_objs_.push_back(std::move(ep));
        for (const Expr& expr : program_->program) {
            compile_expr(expr);
        }
    }

    unsigned int Compiler::compile_expr(const Expr& node) {
        auto fo = active_objs_.top();
        auto scope = active_scopes_.top();
        unsigned int reg;
        if (std::holds_alternative<Atom>(node)) {
            auto atom = std::get<Atom>(node);
            if (atom.get_type() != NodeType::SymbolLiteral)
                reg = fo->next_reg++;
            else
                reg = scope->lookup(atom.get_value<std::string>())->reg;
            compile_atom(atom);
        }
        else {
            reg = fo->next_reg++;
            compile_list(std::get<List>(node));
        }
        return reg;
    }
    /* (define var expr) */
    void Compiler::compile_define(const List& node) {
        auto fo = active_objs_.top();
        unsigned int r1, prev_reg_state = fo->next_reg;
        auto scope = active_scopes_.top();
        auto elems = node.get_elems();
        r1 = compile_expr(elems[2]);
        uint8_t dst = scope->lookup(std::get<Atom>(elems[1]).get_value<std::string>())->reg;
        fo->instructions.push_back(BVM::Emitter::mov(dst, r1));
        fo->next_reg = prev_reg_state;
    }

    void Compiler::compile_lambda(const List& node) {
        auto nfo = std::make_unique<FuncObj>();
        auto elems = node.get_elems();
        int arity = std::get<List>(elems[1]).get_elems().size();
        int n_locals = program_->scopes[&node]->symbol_table.size();
        FuncObj* ptr = nfo.get();
        ptr->n_locals = n_locals;
        ptr->next_reg = arity + n_locals;
        ptr->name = std::get<Atom>(elems[0]).get_value<std::string>();

        active_objs_.push(ptr);
        func_objs_.push_back(std::move(nfo));
        
        for (size_t i = 2; i < elems.size(); i++) {
            compile_expr(elems[i]);
        }
        ptr->next_reg = arity + n_locals;
        active_objs_.pop();
    }

    void Compiler::compile_list(const List& node) {
        auto fo = active_objs_.top();
        auto elems = node.get_elems();
        auto first = elems.at(0);
        if (std::holds_alternative<Atom>(first)) {
            auto atom = std::get<Atom>(first);

            switch(expr_types.at(atom.get_value<std::string>())) {
                case ExprType::Define:
                    compile_define(node);
                    break;
                case ExprType::If:
                    compile_if(node);
                    break;
                case ExprType::Plus:
                case ExprType::Minus:
                case ExprType::Mul:
                case ExprType::Div:
                    compile_binary(node);
                    break;
                default:
                    throw std::runtime_error("compile_list: Not Implemented");
            }

        }
        fo->next_reg--;
    }

    void Compiler::compile_atom(const Atom& node) {
        auto fo = active_objs_.top();
        uint32_t inst = BVM::Emitter::load_const(fo->next_reg - 1, fo->consts.size());
        BVM::BoltValue value;
        switch(node.get_type()) {
            case NodeType::BooleanLiteral:
                value.as_bool = node.get_value<bool>();
                value.type = BVM::BoltType::Boolean;
                break;
            case NodeType::FloatLiteral:
                value.as_double = node.get_value<double>();
                value.type = BVM::BoltType::Float;
                break;
            case NodeType::IntegerLiteral:
                value.as_double = node.get_value<int64_t>();
                value.type = BVM::BoltType::Integer;
                break;
            case NodeType::SymbolLiteral:
                return;
            default:
                throw std::logic_error("unsupported atomic value");
        }
        fo->instructions.push_back(inst);
        size_t n_consts = fo->consts.size();
        if (!fo->consts.contains(value))
            fo->consts[value] = n_consts;
    }

    /* (if cond if-expr else-expr) */
    void Compiler::compile_if(const List& node) {
        auto fo = active_objs_.top();
        unsigned int prev_reg_state = fo->next_reg;
        auto elems = node.get_elems();
        // compile condition
        compile_expr(elems[1]); 
        size_t if_pos = fo->instructions.size();
        // reserve space for the instruction
        fo->instructions.reserve(1);
        compile_expr(elems[2]);
        size_t else_pos = fo->instructions.size();
        compile_expr(elems[3]);
        fo->instructions[if_pos] = BVM::Emitter::jmp_if_false(fo->next_reg - 2, else_pos - if_pos);
        fo->next_reg = prev_reg_state;


    }

    void Compiler::compile_binary(const List& node) {
        auto fo = active_objs_.top();
        unsigned int r1, r2, prev_reg_state = fo->next_reg;
        auto elems = node.get_elems();
        r1 = compile_expr(elems[1]);
        r2 = compile_expr(elems[2]);
        const std::string& proc_name = std::get<Atom>(elems[0]).get_value<std::string>();
        uint32_t inst;
        switch(expr_types.at(proc_name)) {
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
            default:
                throw std::logic_error("not a valid binary operation");
        }
        fo->instructions.push_back(inst);
        fo->next_reg = prev_reg_state;

    }

}
