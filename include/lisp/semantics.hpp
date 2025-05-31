#ifndef LISP_SEMANTICS_H
#define LISP_SEMANTICS_H

#include <lisp/parser.hpp>
#include <list>
#include <memory>
#include <unordered_map>

namespace Lisp {

    enum class ExprType {
        Lambda,
        Define,
        Cons,
        Qoute,
        Set,
    };

    const std::unordered_map<const char*, ExprType> expr_types = {
        {"lambda", ExprType::Lambda},
        {"define", ExprType::Define},
        {"cons", ExprType::Cons},
        {"qoute", ExprType::Qoute},
        {"set!", ExprType::Set},
    };

    struct Scope {
        const List* owner;
        std::unordered_set<std::string> symbol_table;
        std::unordered_set<const Atom*> locals;
    };

    class SemanticAnalyzer {

        private:
            std::list<std::unique_ptr<Scope>> scopes;
            std::unordered_map<const List*, Scope> scope_map;

        public:
            SemanticAnalyzer();
            void visit(const List& node);
            void visit(const Atom& node);
            void verify_define(const List& node);
            void verify_lambda(const List& node);
            void verify_if(const List& node);
            void verify_or(const List& node);
            void verify_and(const List& node);
            void verify_qoute(const List& node);
            void verify_set(const List& node);
            bool has_value(const List& node);
    };

}


#endif
