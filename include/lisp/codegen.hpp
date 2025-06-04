#ifndef LISP_CODEGEN_H
#define LISP_CODEGEN_H

#include "lisp/semantics.hpp"
#include <fstream>
#include <stack>

namespace Lisp {

    /* Bolt File Layout 
     * n_funcs
     * [func_objs]
     * */

    /* Function Objects 
     * len_name
     * func_name
     * n_locals
     * n_consts
     * [constants]
     * n_insts
     * [instructions]
     * */

    struct FuncObj {
        std::string name;
        unsigned int n_locals;
        std::vector<uint8_t> consts;
        std::vector<uint32_t> instructions;
        unsigned int next_reg;
    };

    class Compiler {

        private:
            std::unique_ptr<AnnotatedProgram> program_;
            std::ofstream out_;
            std::stack<Scope*> active_scopes_;
            std::vector<std::unique_ptr<FuncObj>> func_objs_;
            std::stack<FuncObj*> active_objs_;

        public:
            Compiler(std::unique_ptr<AnnotatedProgram> program);
            void compile();
            void compile_expr(const Expr& node);
            void compile_atom(const Atom& node);
            void compile_list(const List& node);
            void compile_qoute();
            void compile_define(const List& node);
            void compile_lambda(const List& node);
            void bind_reg(const Expr& node);
            void free_reg();
            unsigned int get_free_reg();

    };
}

#endif
