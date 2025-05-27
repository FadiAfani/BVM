#include <gtest/gtest.h>


#include <lisp/parser.hpp>

TEST(ParserTests, TestIntegerParsing) {
    Lisp::Lexer lexer("715");
    lexer.tokenize();
    auto toks = lexer.get_tokens();

    EXPECT_EQ(toks.size(), 1);

    Lisp::Parser parser(toks);
    Lisp::Integer integer = parser.parse_integer();
    EXPECT_EQ(integer.get_value(), 715);
}


TEST(ParserTests, TestFloatParsing) {
    Lisp::Lexer lexer("15.3214");
    lexer.tokenize();
    auto toks = lexer.get_tokens();

    EXPECT_EQ(toks.size(), 1);

    Lisp::Parser parser(toks);
    Lisp::Double f = parser.parse_double();
    EXPECT_EQ(f.get_value(), 15.3214);
}
