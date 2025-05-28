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
            Symbol parse_symbol();
            Boolean parse_boolean();
            Integer parse_integer();
            Double parse_double();
            Atom parse_atom();
            Lambda parse_lambda();
            List parse_list();
            Expr parse_expr();



    };

}

#endif
