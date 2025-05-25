#include "lisp/lexer.hpp"
#include <gtest/gtest.h>

TEST(LexerTests, SimpleExpr) {

    Lisp::Lexer lexer("(define x 3)");
    lexer.tokenize();
    auto tokens = lexer.get_tokens();
    for (auto& t : tokens)
        std::cout << t.value << std::endl;
    EXPECT_EQ(tokens.size(), 5);
    

}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
