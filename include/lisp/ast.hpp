#ifndef LIST_AST_H
#define LIST_AST_H

#include "lisp/lexer.hpp"
#include <functional>
#include <unordered_map>
#include <variant>
namespace Lisp {

    enum class NodeType {
        NumberLiteral,
        BooleanLiteral,
        SymbolLiteral,
        List,
        QuotedExpr,
    };

    class ASTNode {
        protected:
            NodeType type_;
        virtual ~ASTNode() = default;
        NodeType get_type();
    };

    class Integer : ASTNode {
        private:
            int64_t value_;
        public:
            Integer(int64_t value);
            int64_t get_value();
    };

    class Double : ASTNode {
        private:
            double value_;
        public:
            Double(double value);
            double get_value();
    };

    class List;

    using Atom = std::variant<Integer, Double>;

    using Expr = std::variant<Atom, List>;

    class List : ASTNode {
        private:
            std::vector<Expr> elems_;

        public:
            List();
            List(std::vector<Expr> elems);
            void add_elem(Expr expr);
            const std::vector<Expr>& get_elems();
    };

    class Lambda : ASTNode {
        private:
            std::vector<std::reference_wrapper<Atom>> params;
            std::unordered_map<std::string, const Token*> symbol_table;

        public:
            const std::unordered_map<std::string, const Token*>& get_symbol_table();
            Lambda();
            ~Lambda();
    };




}

#endif
