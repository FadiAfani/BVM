#ifndef LISP_SEMANTICS_H
#define LISP_SEMANTICS_H

#include <lisp/parser.hpp>
#include <list>
#include <memory>
#include <stack>
#include <unordered_map>

namespace Lisp {

    struct Symbol {
        uint8_t reg;
        const Atom* node;
    };

    struct Scope {
        Scope* parent;
        std::unordered_map<std::string, Symbol> symbol_table;

        Symbol* lookup(const std::string& name);
    };

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


    const std::unordered_map<std::string, ExprType> expr_types = {
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
            Program& program;

        public:
            SemanticAnalyzer();
            SemanticAnalyzer(Program& program);
            void analyze_program();
            void visit(const List& node);
            void visit(const Atom& node);
            void verify_define(const List& node);
            void verify_lambda(const List& node);
            void verify_if(const List& node);
            void verify_or(const List& node);
            void verify_and(const List& node);
            void verify_qoute(const List& node);
            void verify_set(const List& node);
            void verify_binary(const List& node);
            bool has_value(const List& node);
    };

}


#endif
