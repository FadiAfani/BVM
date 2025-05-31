#include <gtest/gtest.h>
#include <lisp/semantics.hpp>
#include <stdexcept>

#define TEST_SETUP(in) 

class SemanticsTester : public ::testing::Test {
protected:
    std::string param;
    Lisp::SemanticAnalyzer analyzer;
    Lisp::List node;

    void SetUp() override {
        param = param_map_[::testing::UnitTest::GetInstance()
                                ->current_test_info()
                                ->name()];

        Lisp::Lexer lexer(param); 
        lexer.tokenize(); 
        auto toks = lexer.get_tokens(); 
        Lisp::Parser parser(std::move(toks)); 
        node = parser.parse_list(); 
    }

    static std::unordered_map<std::string, std::string> param_map_;
};

// Define parameters for each test name
std::unordered_map<std::string, std::string> SemanticsTester::param_map_ = {
    {"TestIf", "(if test (+ x y) (- x y))"},
    {"TestMalformedIf", "(if test (+ x y))"},
    {"TestDefine", "(define x (+ x y))"},
    {"NestedDefine", "( define y (define x (+ x y)))"},
};

// Tests share the same setup but each gets its own parameter
TEST_F(SemanticsTester, TestIf) {
    EXPECT_NO_THROW(analyzer.verify_if(node));
}

TEST_F(SemanticsTester, TestMalformedIf) {
    EXPECT_ANY_THROW(analyzer.verify_if(node));
}

TEST_F(SemanticsTester, TestDefine) {
    EXPECT_NO_THROW(analyzer.verify_define(node));
}

TEST_F(SemanticsTester, NestedDefine) {
    EXPECT_ANY_THROW(analyzer.verify_define(node));
}

