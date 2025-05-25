#include "lisp/lexer.hpp"
#include <cctype>

namespace Lisp {

    Lexer::Lexer(std::string text) : text_(std::move(text)) {}
    Lexer::~Lexer() {}
    const std::vector<Token>& Lexer::get_tokens() { return tokens_; }

    void Lexer::tokenize() {
        char c;
        for (size_t i = 0; i < text_.length();) {
            Token t;
            t.row = cur_row;
            t.col = cur_col;
            switch((c = text_[i])) {
                case '+':
                    t.type = TokenType::Plus;
                    t.value = "+";
                    tokens_.push_back(std::move(t));
                    i++;
                    break;
                case '-':
                    t.type = TokenType::Minus;
                    t.value = "-";
                    tokens_.push_back(std::move(t));
                    i++;
                    break;
                case '/':
                    t.type = TokenType::Div;
                    t.value = "/";
                    tokens_.push_back(std::move(t));
                    i++;
                    break;
                case '*':
                    t.type = TokenType::Mul;
                    t.value = "*";
                    tokens_.push_back(std::move(t));
                    i++;
                    break;
                case '?':
                    t.type = TokenType::QuestionMark;
                    t.value = "?";
                    tokens_.push_back(std::move(t));
                    i++;
                    break;
                case '!':
                    t.type = TokenType::ExclamationMark;
                    t.value = "!";
                    tokens_.push_back(std::move(t));
                    i++;
                    break;
                case ')':
                    t.type = TokenType::Rparen;
                    t.value = ")";
                    tokens_.push_back(std::move(t));
                    i++;
                    break;
                case '(':
                    t.type = TokenType::Lparen;
                    t.value = "(";
                    tokens_.push_back(std::move(t));
                    i++;
                    break;
                case '\r':
                case '\n':
                    cur_row++;
                    i++;
                    break;
                case '\t':
                    cur_col += 4;
                    i++;
                    break;
                case ' ':
                    cur_col++;
                    i++;
                    break;
                default:
                    if (std::isdigit(c)) {
                        while (i < text_.length() && std::isdigit(c)) {
                            t.value += c;
                            c = text_[++i];
                        }
                    } else if (std::isalpha(c)) {
                        while (i < text_.length() && (
                        std::isalnum(c) && !reserved_symbols.contains(c))) {
                            t.value += c;
                            c = text_[++i];
                        }
                        if (reserved_keywords.contains(t.value.data()))
                            t.type = TokenType::Keyword;
                        else
                            t.type = TokenType::Identifier;
                    }
                    else {
                        throw "not a recognizable symbol: " + t.value;
                    }
                    tokens_.push_back(std::move(t));
                    break;

            }
        }
    }
}
