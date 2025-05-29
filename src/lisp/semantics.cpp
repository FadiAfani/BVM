#include<lisp/semantics.hpp>
#include <variant>

namespace Lisp {

    SemanticAnalyzer::SemanticAnalyzer(Program ast) : ast_(ast) {

    }

    void SemanticAnalyzer::visit(const Atom& node) {
        Scope& env = scopes.top();
        if (node.get_type() != NodeType::SymbolLiteral)
            env.locals[&node] = env.locals.size();
    }

    void SemanticAnalyzer::visit(const List& node) {
        for (const Expr& expr : node.get_elems()) {
            if (std::holds_alternative<List>(expr))
                visit(std::get<List>(expr));
            else
                visit(std::get<Atom>(expr));
        }
    }
}
