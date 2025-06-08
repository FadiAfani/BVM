#ifndef LISP_CODEGEN_H
#define LISP_CODEGEN_H

#include "bolt_virtual_machine/vm.hpp"
#include "lisp/semantics.hpp"
#include <cassert>
#include <fstream>
#include <stack>

#define MAX_REGS 255

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
        std::unordered_map<BVM::BoltValue, size_t> consts;
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
            void compile_qoute(const List& node);
            void compile_define(const List& node);
            void compile_lambda(const List& node);
            void compile_if(const List& node);
            void compile_binary(const List& node);

            inline void free_reg(unsigned int n_regs) {
                auto fo = active_objs_.top();
                if (n_regs > MAX_REGS - fo->next_reg)
                    throw std::logic_error("not enough registers to free");
                fo->next_reg--;
            }
            inline void alloc_reg() {
                auto fo = active_objs_.top();
                fo->next_reg++;
            }
            const std::vector<std::unique_ptr<FuncObj>>& get_objs();

    };
}

#endif
