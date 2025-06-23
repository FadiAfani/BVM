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
            std::ofstream out_;
            std::stack<const Scope*> active_scopes_;
            std::vector<std::unique_ptr<FuncObj>> func_objs_;
            std::stack<FuncObj*> active_objs_;

        public:
            Compiler();
            void compile(const Lambda* node);
            unsigned int compile_expr(const ASTNode* node);
            void compile_atom(const AtomicNode* node);
            void compile_list(const ASTNode* node);
            void compile_define(const Define* node);
            void compile_lambda(const Lambda* node);
            void compile_if(const IfExpr* node);
            void compile_binary(const BinaryExpr* node);

            inline void free_reg(unsigned int n_regs) {
                auto fo = active_objs_.top();
                if (n_regs > MAX_REGS - fo->next_reg)
                    throw std::logic_error("not enough registers to free");
                fo->next_reg -= n_regs;
            }

            const std::vector<std::unique_ptr<FuncObj>>& get_objs();

    };
}

#endif
