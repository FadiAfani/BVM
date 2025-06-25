#ifndef LISP_LEXER_H
#define LISP_LEXER_H

#include <string>
#include <unordered_set>
#include <vector>

namespace Lisp {

    const std::unordered_set<std::string> reserved_keywords = {
        "if", "lambda", "define", "let", "begin", "cond", "and", "or",
        "set!", "quote", "+", "-", "/", "*", ">", ">=", "<", "<=", "=", "/="
    };

    const std::unordered_set<char> reserved_symbols = {
        '#', '\'', '`', ',', ';',
        '\'', '(', ')', '\\'
    };

    const std::unordered_set<char> special_initial = {
        '!' , '$' , '%' , '&' , '*' , '/' , ':' , '<' , '=' , '>' , '?' , '^' , '_' , '~' , '+' , '-' , '.'
    };



    enum class TokenType {
        Keyword,
        Identifier,
        Pound,
        Apost,
        Lparen,
        Rparen,
        Float,
        Integer,
        Eof,
    };

    struct Token {
        int col;
        int row;
        std::string value;
        TokenType type;
    };

    class Lexer {

        private:
            std::vector<Token> tokens_;
            std::string text_;
            int cur_row = 0;
            int cur_col = 0;
        public:
            Lexer(std::string text);
            ~Lexer();

            void tokenize();
            const std::vector<Token>& get_tokens();

    };
}

#endif



