#include <lisp/codegen.hpp>
#include <bolt_virtual_machine/emitter.h>

namespace Lisp {

    static void encode_const(FuncObj* obj, const void* data, size_t size) {
        if (obj == nullptr)
            throw std::runtime_error("function object is null, constant encoding failed");

        obj->consts.reserve(size);
        const uint8_t* ptr = static_cast<const uint8_t*>(data);
        for (size_t i = 0; i < size; i++) {
            obj->consts.push_back((ptr[i] >> i * 8) & 0xFF);
        }
    }

    Compiler::Compiler(std::unique_ptr<AnnotatedProgram> program) : program_(std::move(program)) {

    }

    void Compiler::compile() {
        auto ep = std::make_unique<FuncObj>();
        active_objs_.push(ep.get());
        func_objs_.push_back(std::move(ep));
        for (const Expr& expr : program_->program) {
            compile_expr(expr);
        }
    }

    void Compiler::compile_expr(const Expr& node) {
        if (std::holds_alternative<Atom>(node))
            compile_atom(std::get<Atom>(node));
        else
            compile_list(std::get<List>(node));
    }

    void Compiler::compile_define(const List& node) {
        auto fo = active_objs_.top();
        auto scope = active_scopes_.top();
        auto elems = node.get_elems();
        compile_expr(elems[2]);
        uint8_t dst = scope->lookup(std::get<Atom>(elems[1]).get_value<std::string>())->reg;
        fo->instructions.push_back(BVM::Emitter::mov(dst, fo->next_reg - 1));
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
        active_objs_.pop();
    }

    void Compiler::compile_list(const List& node) {
        switch(program_->sexpr_type[&node]) {
            case ExprType::Define:
                compile_define(node);
                break;
            default:
                throw std::runtime_error("compile_list: Not Implemented");
        }
    }

    void Compiler::compile_atom(const Atom& node) {
        auto fo = active_objs_.top();
        switch(node.get_type()) {
            case NodeType::BooleanLiteral:
                fo->consts.push_back(static_cast<uint8_t>(node.get_value<bool>()));
                break;
            case NodeType::FloatLiteral:
                encode_const(fo, &node.get_value<double>(), sizeof(double));
                break;
            case NodeType::IntegerLiteral:
                encode_const(fo, &node.get_value<int>(), sizeof(int));
                break;
            default:
                break;



        }
    }


}
