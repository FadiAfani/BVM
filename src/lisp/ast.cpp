#include "lisp/ast.hpp"

namespace Lisp {

    NodeType ASTNode::get_type() { return type_; }

    Integer::Integer(int64_t value) : value_(value) {
        type_ = NodeType::IntegerLiteral;
    }
    int64_t Integer::get_value() const { return value_; }

    Double::Double(double value) : value_(value) {
        type_ = NodeType::FloatLiteral;
    }
    double Double::get_value() const { return value_; }

    Boolean::Boolean(bool value) : value_(value) {}
    bool Boolean::get_value() const { return value_; }

    Symbol::Symbol(std::string value) : value_(std::move(value)) {}
    const std::string& Symbol::get_value() const { return value_; }

    List::List() {}

    List::List(std::vector<Expr> elems) : elems_(std::move(elems)) {}
    const std::vector<Expr>& List::get_elems() const { return elems_; }


} 
