#ifndef LISP_LEXER_H
#define LISP_LEXER_H

#include <string>
#include <unordered_set>
#include <vector>

namespace Lisp {

    const std::unordered_set<const char*> reserved_keywords = {
        "if", "lambda", "define", "let", "begin", "cond", "and", "or",
        "set!", "quote"
    };

    const std::unordered_set<char> reserved_symbols = {
        '#', '\'', '`', ',', ';',
        '\'', '(', ')', '\\'
    };

    enum class TokenType {
        Plus,
        Minus,
        Mul,
        Div,
        QuestionMark,
        ExclamationMark,
        Eq,
        Bt,
        Ls,
        Underscore,
        Keyword,
        Number,
        Identifier,
        Boolean,
        Pound,
        Lparen,
        Rparen
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



