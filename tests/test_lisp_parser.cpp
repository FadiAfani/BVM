#include "lisp/ast.hpp"
#include <gtest/gtest.h>


#include <lisp/parser.hpp>

TEST(ParserTests, TestIntegerParsing) {
    Lisp::Lexer lexer("715");
    lexer.tokenize();
    auto toks = lexer.get_tokens();

    Lisp::Parser parser(toks);
    auto integer = parser.parse_integer();
    EXPECT_EQ(integer->get_value(), 715);
}


TEST(ParserTests, TestFloatParsing) {
    Lisp::Lexer lexer("15.3214");
    lexer.tokenize();
    auto toks = lexer.get_tokens();

    Lisp::Parser parser(toks);
    auto f = parser.parse_double();
    EXPECT_EQ(f->get_value(), 15.3214);
}

TEST(ParserTests, TestBooleanParsing) {
    Lisp::Lexer lexer("#t #f");
    lexer.tokenize();
    auto toks = lexer.get_tokens();

    Lisp::Parser parser(toks);
    auto b = parser.parse_boolean();
    auto b1 = parser.parse_boolean();
    EXPECT_EQ(b->get_value(), true);
    EXPECT_EQ(b1->get_value(), false);
}


TEST(ParserTests, TestListParsing) {
    Lisp::Lexer lexer("(1 2 3)");
    lexer.tokenize();
    auto toks = lexer.get_tokens();

    Lisp::Parser parser(toks);
    auto list = parser.parse_list();
}

TEST(ParserTests, ParseSimpleAddition) {
    Lisp::Lexer lexer("(+ 2 3)");
    lexer.tokenize();
    auto toks = lexer.get_tokens();

    Lisp::Parser parser(toks);
    auto list = parser.parse_list();

}

TEST(ParserTests, ParseLambda) {
    Lisp::Lexer lexer("(lambda (x y) (+ x y))");
    lexer.tokenize();
    auto toks = lexer.get_tokens();

    Lisp::Parser parser(toks);
    auto list = parser.parse_list();
}

TEST(ParserTests, TestQuotedExpr) {
    Lisp::Lexer lexer("'(1 2 3)");
    lexer.tokenize();
    auto toks = lexer.get_tokens();
    Lisp::Parser parser(toks);

    auto expr = parser.parse_qouted_expr();

}

