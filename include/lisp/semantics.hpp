#ifndef LISP_SEMANTICS_H
#define LISP_SEMANTICS_H

#include "lisp/ast.hpp"
#include <lisp/parser.hpp>
#include <stack>

#define MAX_REGS 255

namespace Lisp {

    enum class NativeFunc {
        Add,
        Sub,
        Div,
        Mul,
        Bt,
        Lt,
        Bte,
        Lte,
        Eq,
        Ne,
    };

    const std::unordered_map<std::string, NativeFunc> native_funcs = {
        {"+", NativeFunc::Add},
        {"-", NativeFunc::Sub},
        {"/", NativeFunc::Div},
        {"*", NativeFunc::Mul},
        {">", NativeFunc::Bt},
        {">=", NativeFunc::Bte},
        {"<", NativeFunc::Lt},
        {"<=", NativeFunc::Lte},
        {"=", NativeFunc::Eq},
        {"/=", NativeFunc::Ne},
    };



    class SemanticAnalyzer {

        private:
            std::stack<Scope*> scopes_;
            Program& program_;

        public:
            SemanticAnalyzer(Program& program);
            std::unique_ptr<Lambda> verify();
            std::unique_ptr<ASTNode> verify_sexpr(std::unique_ptr<SExpr> sexpr);
            std::unique_ptr<AtomicNode> verify_symbol(std::unique_ptr<SymbolAtom> sexpr);
            std::unique_ptr<ASTNode> verify_list(std::unique_ptr<List> sexpr);
            std::unique_ptr<Define> verify_define(std::unique_ptr<List> sexpr);
            std::unique_ptr<Lambda> verify_lambda(std::unique_ptr<List> sexpr);
            std::unique_ptr<IfExpr> verify_if(std::unique_ptr<List> sexpr);
            std::unique_ptr<ProcCall> verify_proc_call(std::unique_ptr<List> sexpr);
            bool has_value(std::unique_ptr<SExpr> sexpr);
    };

}


#endif
