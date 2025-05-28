#include "lisp/lexer.hpp"
#include <gtest/gtest.h>

TEST(LexerTests, SimpleExpr) {

    Lisp::Lexer lexer("(define x 3)");
    lexer.tokenize();
    auto tokens = lexer.get_tokens();
    EXPECT_EQ(tokens.size(), 6);
    

}

TEST(LexerTests, LexFloat) {

    Lisp::Lexer lexer("(define x 15.3214)");
    lexer.tokenize();
    auto tokens = lexer.get_tokens();
    EXPECT_EQ(tokens.size(), 6);
    EXPECT_EQ(tokens[tokens.size() - 3].type, Lisp::TokenType::Float);
    EXPECT_EQ(tokens[tokens.size() - 3].value, "15.3214");
}

TEST(LexerTests, LexSpecialInitial) {

    Lisp::Lexer lexer("(+ x y)");
    lexer.tokenize();
    auto tokens = lexer.get_tokens();

    EXPECT_EQ(tokens.size(), 6);
    EXPECT_EQ(tokens[1].type, Lisp::TokenType::Identifier);
    

}

