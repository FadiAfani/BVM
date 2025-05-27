#ifndef BVM_PARSER_H
#define BVM_PARSER_H

#include "lisp/ast.hpp"
#include "lisp/lexer.hpp"
namespace Lisp {

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
            void parse();
            Integer parse_integer();
            Double parse_double();
            Atom parse_atom();
            Lambda parse_lambda();



    };

}

#endif
