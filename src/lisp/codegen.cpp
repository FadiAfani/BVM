#include <lisp/codegen.hpp>
#include <bolt_virtual_machine/emitter.h>

namespace Lisp {

    static void write_bytes_le(const void* data, size_t size) {
        const char* ptr = static_cast<const char*>(data);
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


}
