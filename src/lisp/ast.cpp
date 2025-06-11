#include "lisp/ast.hpp"
#include <stdexcept>

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

    std::string Atom::print() const {
        switch(type_) {
            case NodeType::IntegerLiteral: return std::to_string(std::get<int64_t>(value_));
            case NodeType::FloatLiteral: return std::to_string(std::get<double>(value_));
            case NodeType::BooleanLiteral: return std::get<bool>(value_) ? "#t" : "#f";
            case NodeType::SymbolLiteral: return std::get<std::string>(value_);
            default:
                std::runtime_error("print: Not Implemented");
        }
        return ""; // to supress compiler warnings
    }

    List::List() {}

    List::List(std::vector<Expr> elems) : elems_(std::move(elems)) {}
    const std::vector<Expr>& List::get_elems() const { return elems_; }

    std::string List::print() const {
        std::string res = "";
        for (auto& expr : elems_) {
            std::visit([&](auto&& expr) {
                res += expr.print() + " ";
            }, expr);
        }
        return "(" + res + ")";
    }

} 
