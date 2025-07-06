#include "lisp/lexer.hpp"
#include "lisp/parser.hpp"
#include "lisp/disassembler.hpp"
#include <iostream>
int main(int argc, char** argv) {
    Lisp::Lexer lexer("(define x (if (< x 10) (* x 20) (* x 10)))");
    lexer.tokenize();
    auto toks = lexer.get_tokens();
    Lisp::Parser parser(toks);
    auto nodes = parser.parse();
    Lisp::SemanticAnalyzer sa(nodes);
    std::unique_ptr<Lisp::Lambda> ap = sa.verify();
    Lisp::Compiler compiler("main.lsp");

    printf("%ld\n", ap->get_exprs().size());
    compiler.compile(ap.get());
    Lisp::Disassembler dis(compiler.get_objs()[0].get());
    const std::string& out = dis.disassemble();
    std::cout << out;
    return 0;
}
