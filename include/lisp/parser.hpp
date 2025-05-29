#ifndef BVM_PARSER_H
#define BVM_PARSER_H

#include "lisp/ast.hpp"
#include "lisp/lexer.hpp"
namespace Lisp {

    using Program = std::vector<Expr>;

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
            Atom parse_symbol();
            Atom parse_boolean();
            Atom parse_integer();
            Atom parse_double();
            Atom parse_atom();
            List parse_list();
            Expr parse_expr();



    };

}

#endif
