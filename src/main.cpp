#include "lisp/lexer.hpp"
#include "lisp/parser.hpp"
#include "lisp/disassembler.hpp"
#include <iostream>
int main(int argc, char** argv) {
    Lisp::Lexer lexer("(define x 3) \n (define y (if (< x 10) (* x 2) (- x 2)))");
    lexer.tokenize();
    auto toks = lexer.get_tokens();
    Lisp::Parser parser(toks);
    auto nodes = parser.parse();
    Lisp::SemanticAnalyzer sa(std::move(nodes));
    auto ap = sa.analyze_program();
    Lisp::Compiler compiler(std::move(ap));
    compiler.compile();
    Lisp::Disassembler dis(compiler.get_objs()[0].get());
    const std::string& out = dis.disassemble();
    std::cout << out;
    return 0;
}
