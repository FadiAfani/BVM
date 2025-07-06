#ifndef LISP_CODEGEN_H
#define LISP_CODEGEN_H

#include "bolt_virtual_machine/vm.hpp"
#include "lisp/semantics.hpp"
#include <cassert>
#include <fstream>
#include <stack>


namespace Lisp {

    /* Bolt File Layout 
     * n_funcs
     * [func_objs]
     * */

    /* Function Objects 
     * n_locals
     * n_consts
     * [constants]
     * n_insts
     * [instructions]
     * */

    class Compiler {



        private:
            std::ofstream out_;
            std::stack<const Scope*> active_scopes_;
            std::vector<std::unique_ptr<BVM::Prototype>> func_objs_;
            std::stack<BVM::Prototype*> active_objs_;

        public:
            Compiler(std::string filename);
            void compile(const Lambda* node);
            unsigned int compile_expr(const ASTNode* node);
            void compile_atom(const AtomicNode* node);
            void compile_list(const ASTNode* node);
            void compile_define(const Define* node);
            void compile_lambda(const Lambda* node);
            void compile_if(const IfExpr* node);
            void compile_list_expr(const ListExpr* node);
            void compile_proc_call(const ProcCall* node);

            inline void dealloc_expr(const ASTNode* expr) {
                auto fo = active_objs_.top();
                if (expr->get_type() != NodeType::Atomic 
                        || static_cast<const AtomicNode*>(expr)->get_value()->get_type() != SExprType::SymbolLiteral)
                    fo->next_reg--;
            }

            const std::vector<std::unique_ptr<BVM::Prototype>>& get_objs();

    };
}

#endif
