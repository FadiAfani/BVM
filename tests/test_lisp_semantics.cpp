#include <gtest/gtest.h>
#include <lisp/semantics.hpp>
#include <stdexcept>

#define TEST_SETUP(in) 

class SemanticsTester : public ::testing::Test {
protected:
    std::string param;
    Lisp::SemanticAnalyzer analyzer;
    Lisp::Program node;

    void SetUp() override {
        param = param_map_[::testing::UnitTest::GetInstance()
                                ->current_test_info()
                                ->name()];

        Lisp::Lexer lexer(param); 
        lexer.tokenize(); 
        auto toks = lexer.get_tokens(); 
        Lisp::Parser parser(std::move(toks)); 
        node = parser.parse();
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

