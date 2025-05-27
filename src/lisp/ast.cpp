#include "lisp/ast.hpp"

namespace Lisp {

    NodeType ASTNode::get_type() { return type_; }

    Integer::Integer(int64_t value) : value_(value) {}
    int64_t Integer::get_value() { return value_; }

    Double::Double(double value) : value_(value) {}
    double Double::get_value() { return value_; }

    List::List() {}

    List::List(std::vector<Expr> elems) : elems_(std::move(elems)) {}


}
