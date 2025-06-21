#ifndef LISP_SEMANTICS_H
#define LISP_SEMANTICS_H

#include "lisp/ast.hpp"
#include <lisp/parser.hpp>
#include <stack>
#include <unordered_map>

namespace Lisp {


    enum class ExprType {
        Lambda,
        Define,
        Cons,
        Qoute,
        Set,
        If,
        Plus,
        Minus,
        Mul,
        Div,
        Bt,
        Lt,
        Bte,
        Lte,
        Eq,
        Ne,
    };


    const std::unordered_map<std::string, ExprType> reserved_funcs = {
        {"lambda", ExprType::Lambda},
        {"define", ExprType::Define},
        {"cons", ExprType::Cons},
        {"qoute", ExprType::Qoute},
        {"set!", ExprType::Set},
        {"if", ExprType::If},
        {"+", ExprType::Plus},
        {"-", ExprType::Minus},
        {"*", ExprType::Mul},
        {">", ExprType::Bt},
        {">=", ExprType::Bte},
        {"<", ExprType::Lt},
        {"<=", ExprType::Lte},
        {"/=", ExprType::Ne},
        {"=", ExprType::Eq},
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
            std::unique_ptr<BinaryExpr> verify_binary(std::unique_ptr<List> sexpr);
            std::unique_ptr<ListExpr> verify_list_expr(std::unique_ptr<List> sexpr);
            bool has_value(std::unique_ptr<SExpr> sexpr);
    };

}


#endif
