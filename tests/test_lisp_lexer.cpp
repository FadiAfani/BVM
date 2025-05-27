#include "lisp/lexer.hpp"
#include <gtest/gtest.h>

TEST(LexerTests, SimpleExpr) {

    Lisp::Lexer lexer("(define x 3)");
    lexer.tokenize();
    auto tokens = lexer.get_tokens();
    EXPECT_EQ(tokens.size(), 5);
    

}

TEST(LexerTests, LexFloat) {

    Lisp::Lexer lexer("(define x 15.3214)");
    lexer.tokenize();
    auto tokens = lexer.get_tokens();
    EXPECT_EQ(tokens.size(), 5);
    EXPECT_EQ(tokens[tokens.size() - 2].type, Lisp::TokenType::Float);
    EXPECT_EQ(tokens[tokens.size() - 2].value, "15.3214");
}

