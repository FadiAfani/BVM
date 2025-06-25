#include "lisp/lexer.hpp"
#include <cctype>
#include <stdexcept>

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
                case '#':
                    t.type = TokenType::Pound;
                    t.value = "#";
                    tokens_.push_back(std::move(t));
                    i++;
                    break;
                case '\'':
                    t.type = TokenType::Apost;
                    t.value = "'";
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
                        if (c == '.') {
                            t.value += c;
                            c = text_[++i]; // skip '.'
                            t.type = TokenType::Float;
                            while (i < text_.length() && std::isdigit(c)) {
                                t.value += c;
                                c = text_[++i];
                            }
                        } else {
                            t.type = TokenType::Integer;
                        }
                    } else if (std::isalpha(c) || special_initial.contains(c)) {
                        while (i < text_.length() && (
                        std::isalnum(c) || special_initial.contains(c))) {
                            t.value += c;
                            c = text_[++i];
                        }
                        if (reserved_keywords.contains(t.value))
                            t.type = TokenType::Keyword;
                        else
                            t.type = TokenType::Identifier;
                    }
                    else {
                        throw std::logic_error("not a recognizable symbol: " + std::string(1, c));
                    }
                    tokens_.push_back(std::move(t));
                    break;

            }
        }
        tokens_.push_back({.type = TokenType::Eof});
    }
}
