#include "lisp/ast.hpp"

namespace Lisp {

    NodeType ASTNode::get_type() const { return type_; }

    Atom::Atom(std::string value, bool symbol) : value_(value) {
        if (symbol)
            type_ = NodeType::SymbolLiteral;
    }
    Atom::Atom(int64_t value) : value_(value) {
        type_ = NodeType::IntegerLiteral;
    }
    Atom::Atom(double value) : value_(value) {
        type_ = NodeType::FloatLiteral;
    }
    Atom::Atom(bool value) : value_(value) {
        type_ = NodeType::BooleanLiteral;
    }

    List::List() {}

    List::List(std::vector<Expr> elems) : elems_(std::move(elems)) {}
    const std::vector<Expr>& List::get_elems() const { return elems_; }

} 
