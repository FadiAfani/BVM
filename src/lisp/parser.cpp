#include "lisp/lexer.hpp"
#include <lisp/parser.hpp>

namespace Lisp {

    Parser::Parser(const std::vector<Token> tokens) : tokens_(std::move(tokens)) {}
    Parser::~Parser() {}

    const Token& Parser::peek(size_t n) { return tokens_.at(cur_ + n); }
    const Token& Parser::peek() { return peek(0); }

    std::unique_ptr<IntAtom> Parser::parse_integer() {
        auto t = peek();
        if (t.type != TokenType::Integer)
            throw "unexpected token: " + t.value;
        consume();
        int res = 0;
        for (char c : t.value) {
            res = res * 10 + static_cast<int64_t>(c - '0');
        }
        return std::make_unique<IntAtom>(res);
    }

    std::unique_ptr<FloatAtom> Parser::parse_double() {
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
        return std::make_unique<FloatAtom>(res);
    }

    std::unique_ptr<BoolAtom> Parser::parse_boolean() {
        char c;
        auto t1 = peek(), t2 = peek(1);
        if (t1.type == TokenType::Pound && t2.type == TokenType::Identifier) {
            consume();
            consume();
            c = *t2.value.data();
            if (c == 't')
                return std::make_unique<BoolAtom>(true);
            if (c == 'f')
                return std::make_unique<BoolAtom>(false);
            throw std::runtime_error("not a valid boolean value: " + t2.value);

        }
        throw std::runtime_error("not a valid boolean value: " + t1.value);
    }

    std::unique_ptr<SymbolAtom> Parser::parse_symbol() {
        auto t = peek();
        std::unique_ptr<SExpr> s = std::make_unique<SymbolAtom>(t.value);
        if (t.type == TokenType::Identifier || t.type == TokenType::Keyword) {
            consume();
            return std::make_unique<SymbolAtom>(t.value);
        }
        throw std::runtime_error("not a symbol: " + t.value);
    }

    std::unique_ptr<SExpr> Parser::parse_atom() {
        auto t = peek();
        if (t.type == TokenType::Integer)
            return parse_integer();
        else if (t.type == TokenType::Float)
            return parse_double();
        return parse_symbol();
    }

    std::unique_ptr<SExpr> Parser::parse_list() {
        std::vector<std::unique_ptr<SExpr>> exprs;
        if (peek().type != TokenType::Lparen)
            throw "expected '(' before a list expression";
        consume();
        while (peek().type != TokenType::Rparen)
            exprs.push_back(std::move(parse_expr()));
        consume();

        return std::make_unique<List>(std::move(exprs));
    }

    std::unique_ptr<SExpr> Parser::parse_expr() {
        auto t = peek();
        if (t.type == TokenType::Lparen)
            return parse_list();

        return parse_atom();
    }

    Program Parser::parse() {
        std::vector<std::unique_ptr<SExpr>> exprs;
        while (peek().type != TokenType::Eof) {
            exprs.push_back(std::move(parse_expr()));
        }
        return std::move(exprs);
    }

}
