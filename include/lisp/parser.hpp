#ifndef BVM_PARSER_H
#define BVM_PARSER_H

#include "lisp/ast.hpp"
#include "lisp/lexer.hpp"
namespace Lisp {

    using Program = std::vector<std::unique_ptr<SExpr>>;

    class Parser {

        private:
            const std::vector<Token> tokens_;
            size_t cur_ = 0;

        public:
            Parser(std::vector<Token> tokens);
            ~Parser();
            const Token& peek();
            const Token& peek(size_t n);
            inline void consume() { cur_++; }
            Program parse();
            std::unique_ptr<SExpr> parse_symbol();
            std::unique_ptr<SExpr> parse_boolean();
            std::unique_ptr<SExpr> parse_integer();
            std::unique_ptr<SExpr> parse_double();
            std::unique_ptr<SExpr> parse_atom();
            std::unique_ptr<SExpr> parse_list();
            std::unique_ptr<SExpr> parse_expr();



    };

}

#endif
