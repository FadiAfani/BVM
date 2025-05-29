#ifndef LIST_AST_H
#define LIST_AST_H

#include "lisp/lexer.hpp"
#include <variant>
namespace Lisp {

    enum class NodeType {
        IntegerLiteral,
        FloatLiteral,
        BooleanLiteral,
        SymbolLiteral,
        List,
        QuotedExpr,
    };

    class Visitor;

    class ASTNode {
        protected:
            NodeType type_;
        public:
            NodeType get_type() const;
            virtual ~ASTNode() = default;
    };

    class Atom : public ASTNode {
        private:
            std::variant<std::string, int64_t, double, bool> value_;
        public:
            Atom(std::string value, bool symbol = true);
            Atom(int64_t value);
            Atom(double value);
            Atom(bool value);
            template<typename T>
            T& get_value() {
                return std::get<T>(value_);
            }
    };


    class List;

    // Atom should be a generic class - not sure if it can hold anything other than a single value

    using Expr = std::variant<Atom, List>;

    class List : public ASTNode {
        private:
            std::vector<Expr> elems_;

        public:
            List();
            List(std::vector<Expr> elems);
            void add_elem(Expr expr);
            const std::vector<Expr>& get_elems() const;
    };

}

#endif
