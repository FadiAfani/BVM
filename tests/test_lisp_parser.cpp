#include <gtest/gtest.h>


#include <lisp/parser.hpp>

TEST(ParserTests, TestIntegerParsing) {
    Lisp::Lexer lexer("715");
    lexer.tokenize();
    auto toks = lexer.get_tokens();

    Lisp::Parser parser(toks);
    Lisp::Integer integer = parser.parse_integer();
    EXPECT_EQ(integer.get_value(), 715);
}


TEST(ParserTests, TestFloatParsing) {
    Lisp::Lexer lexer("15.3214");
    lexer.tokenize();
    auto toks = lexer.get_tokens();

    Lisp::Parser parser(toks);
    Lisp::Double f = parser.parse_double();
    EXPECT_EQ(f.get_value(), 15.3214);
}

TEST(ParserTests, TestBooleanParsing) {
    Lisp::Lexer lexer("#t #f");
    lexer.tokenize();
    auto toks = lexer.get_tokens();

    Lisp::Parser parser(toks);
    Lisp::Boolean b = parser.parse_boolean();
    Lisp::Boolean b1 = parser.parse_boolean();
    EXPECT_EQ(b.get_value(), true);
    EXPECT_EQ(b1.get_value(), false);
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

// GPT-Generated test - too lazy to write one this big (use with caution)
TEST(ParserTests, ParseSimpleProgram) {
    Lisp::Lexer lexer(R"((define (fib n)
                (if (< n 2) 
                    n
                    (+ (fib (- n 1)) (fib (- n 2)))))
            )");

    lexer.tokenize();
    auto toks = lexer.get_tokens();
    EXPECT_EQ(toks.size(), 36);

    Lisp::Parser parser(toks);
    std::vector<Lisp::Expr> exprs = parser.parse();
    ASSERT_EQ(exprs.size(), 1);

    // Helper lambdas
    auto is_symbol = [](const Lisp::Expr& expr, const std::string& expected) -> bool {
        if (!std::holds_alternative<Lisp::Atom>(expr)) return false;
        const auto& atom = std::get<Lisp::Atom>(expr);
        if (!std::holds_alternative<Lisp::Symbol>(atom)) return false;
        return std::get<Lisp::Symbol>(atom).get_value() == expected;
    };

    auto is_integer = [](const Lisp::Expr& expr, int64_t expected) -> bool {
        if (!std::holds_alternative<Lisp::Atom>(expr)) return false;
        const auto& atom = std::get<Lisp::Atom>(expr);
        if (!std::holds_alternative<Lisp::Integer>(atom)) return false;
        return std::get<Lisp::Integer>(atom).get_value() == expected;
    };

    auto as_list = [](const Lisp::Expr& expr) -> const std::vector<Lisp::Expr>& {
        return std::get<Lisp::List>(expr).get_elems();
    };

    const auto& top_list = as_list(exprs[0]);

    // Check: (define (fib n) ...)
    ASSERT_EQ(top_list.size(), 3);
    EXPECT_TRUE(is_symbol(top_list[0], "define"));

    // Check: (fib n)
    const auto& func_sig = as_list(top_list[1]);
    ASSERT_EQ(func_sig.size(), 2);
    EXPECT_TRUE(is_symbol(func_sig[0], "fib"));
    EXPECT_TRUE(is_symbol(func_sig[1], "n"));

    // Check: (if (< n 2) n (+ ...))
    const auto& if_expr = as_list(top_list[2]);
    ASSERT_GE(if_expr.size(), 4);
    EXPECT_TRUE(is_symbol(if_expr[0], "if"));

    // Check: (< n 2)
    const auto& cond = as_list(if_expr[1]);
    ASSERT_EQ(cond.size(), 3);
    EXPECT_TRUE(is_symbol(cond[0], "<"));
    EXPECT_TRUE(is_symbol(cond[1], "n"));
    EXPECT_TRUE(is_integer(cond[2], 2));

    // Check: then branch is just n
    EXPECT_TRUE(is_symbol(if_expr[2], "n"));

    // Check: (+ (fib (- n 1)) (fib (- n 2)))
    const auto& plus_expr = as_list(if_expr[3]);
    ASSERT_EQ(plus_expr.size(), 3);
    EXPECT_TRUE(is_symbol(plus_expr[0], "+"));

    // Check first recursive call: (fib (- n 1))
    const auto& first_call = as_list(plus_expr[1]);
    EXPECT_TRUE(is_symbol(first_call[0], "fib"));
    const auto& first_sub = as_list(first_call[1]);
    EXPECT_TRUE(is_symbol(first_sub[0], "-"));
    EXPECT_TRUE(is_symbol(first_sub[1], "n"));
    EXPECT_TRUE(is_integer(first_sub[2], 1));

    // Check second recursive call: (fib (- n 2))
    const auto& second_call = as_list(plus_expr[2]);
    EXPECT_TRUE(is_symbol(second_call[0], "fib"));
    const auto& second_sub = as_list(second_call[1]);
    EXPECT_TRUE(is_symbol(second_sub[0], "-"));
    EXPECT_TRUE(is_symbol(second_sub[1], "n"));
    EXPECT_TRUE(is_integer(second_sub[2], 2));
}

