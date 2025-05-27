#include <lisp/parser.hpp>

namespace Lisp {

    Parser::Parser(const std::vector<Token> tokens) : tokens_(std::move(tokens)) {}
    Parser::~Parser() {}

    const Token& Parser::peek(size_t n) { return tokens_.at(cur_ + n); }
    const Token& Parser::peek() { return peek(0); }

    Integer Parser::parse_integer() {
        auto t = peek();
        if (t.type != TokenType::Integer)
            throw "unexpected token: " + t.value;
        consume();
        int64_t res = 0;
        for (char c : t.value) {
            res = res * 10 + static_cast<int64_t>(c - '0');
        }
        return Integer(res);
    }

    Double Parser::parse_double() {
        auto t = peek();
        double res = 0.0, n = 0.1;
        size_t i = 0;
        char c;

        if (t.type != TokenType::Float)
            throw "unexpected token: " + t.value;
        
        consume();

        while ((c = t.value[i++]) != '.') {
            res = res * 10 + static_cast<double>(c - '0');
        }
        for (size_t j = i; j < t.value.length(); j++) {
            c = t.value[j];
            res += static_cast<double>(c - '0') * n;
            n /= 10;
        }
        return Double(res);
    }
}
