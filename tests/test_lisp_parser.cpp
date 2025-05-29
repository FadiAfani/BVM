#include <gtest/gtest.h>


#include <lisp/parser.hpp>

TEST(ParserTests, TestIntegerParsing) {
    Lisp::Lexer lexer("715");
    lexer.tokenize();
    auto toks = lexer.get_tokens();

    Lisp::Parser parser(toks);
    Lisp::Atom integer = parser.parse_integer();
    EXPECT_EQ(integer.get_value<int64_t>(), 715);
}


TEST(ParserTests, TestFloatParsing) {
    Lisp::Lexer lexer("15.3214");
    lexer.tokenize();
    auto toks = lexer.get_tokens();

    Lisp::Parser parser(toks);
    Lisp::Atom f = parser.parse_double();
    EXPECT_EQ(f.get_value<double>(), 15.3214);
}

TEST(ParserTests, TestBooleanParsing) {
    Lisp::Lexer lexer("#t #f");
    lexer.tokenize();
    auto toks = lexer.get_tokens();

    Lisp::Parser parser(toks);
    Lisp::Atom b = parser.parse_boolean();
    Lisp::Atom b1 = parser.parse_boolean();
    EXPECT_EQ(b.get_value<bool>(), true);
    EXPECT_EQ(b1.get_value<bool>(), false);
}


TEST(ParserTests, TestListParsing) {
    Lisp::Lexer lexer("(1 2 3)");
    lexer.tokenize();
    auto toks = lexer.get_tokens();

    Lisp::Parser parser(toks);
    Lisp::List list = parser.parse_list();
    EXPECT_EQ(list.get_elems().size(), 3);
}

TEST(ParserTests, ParseSimpleAddition) {
    Lisp::Lexer lexer("(+ 2 3)");
    lexer.tokenize();
    auto toks = lexer.get_tokens();

    Lisp::Parser parser(toks);
    Lisp::List list = parser.parse_list();
    EXPECT_EQ(list.get_elems().size(), 3);

}

TEST(ParserTests, ParseLambda) {
    Lisp::Lexer lexer("(lambda (x y) (+ x y))");
    lexer.tokenize();
    auto toks = lexer.get_tokens();

    Lisp::Parser parser(toks);
    Lisp::List list = parser.parse_list();
    EXPECT_EQ(list.get_elems().size(), 3);
}

