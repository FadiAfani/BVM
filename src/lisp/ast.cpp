#include "lisp/ast.hpp"
#include <string>

namespace Lisp {

    StringAtom::StringAtom(std::string value)  {
        value_ = std::move(value);
        type_ = SExprType::StringLiteral;
    }

    const std::string StringAtom::print() {
        return value_;
    }

    SymbolAtom::SymbolAtom(std::string value)  {
        value_ = std::move(value);
        type_ = SExprType::SymbolLiteral;
    }

    const std::string SymbolAtom::print() {
        return value_;
    }

    IntAtom::IntAtom(int value) {
        type_ = SExprType::IntLiteral;
        value_ = value;
    } 

    const std::string IntAtom::print() {
        return std::to_string(value_);
    }

    FloatAtom::FloatAtom(double value) {
        type_ = SExprType::FloatLiteral;
        value_ = value;
    }

    const std::string FloatAtom::print() {
        return std::to_string(value_);
    }
    
    BoolAtom::BoolAtom(bool value) {
        type_ = SExprType::BoolLiteral;
        value_ = value;
    }

    const std::string BoolAtom::print() {
        return value_ ? "#t" : "#f";
    }


    List::List() {}
    
    List::List(std::vector<std::unique_ptr<SExpr>> elems) : elems_(std::move(elems)) {}

    void List::add_elem(std::unique_ptr<SExpr> expr) {}

    NodeType ASTNode::get_type() const {
        return type_;
    }

    Lambda::Lambda() { type_ = NodeType::Lambda; }

    void Lambda::set_expr(std::unique_ptr<ASTNode> expr) {
        expr_ = std::move(expr);
    }

    const ASTNode* Lambda::get_expr() const {
        return expr_.get();
    }

    ListExpr::ListExpr() { type_ = NodeType::ListExpr; };


    const std::vector<std::unique_ptr<ASTNode>>& ListExpr::get_elems() {
        return elems_;
    }

    void ListExpr::add_elem(std::unique_ptr<ASTNode> elem) {
        elems_.push_back(std::move(elem));
    }

    BinaryExpr::BinaryExpr() { type_ = NodeType::BinaryExpr; }

    const ASTNode* BinaryExpr::get_left() { 
        return left_.get();
    }

    const ASTNode* BinaryExpr::get_right() { 
        return right_.get();
    }

    void BinaryExpr::set_left(std::unique_ptr<ASTNode> left) {
        left_ = std::move(left);
    }

    void BinaryExpr::set_right(std::unique_ptr<ASTNode> right) {
        right_ = std::move(right);
    }

    IfExpr::IfExpr() { type_ = NodeType::IfExpr; }

    const ASTNode* IfExpr::get_fexpr() {
        return fexpr_.get();
    }

    const ASTNode* IfExpr::get_texpr() {
        return texpr_.get();
    }

    const ASTNode* IfExpr::get_cond() {
        return cond_.get();
    }

    void IfExpr::set_cond(std::unique_ptr<ASTNode> cond) {
        cond_ = std::move(cond);
    }

    void IfExpr::set_fexpr(std::unique_ptr<ASTNode> fexpr) {
        fexpr_ = std::move(fexpr);
    }

    void IfExpr::set_texpr(std::unique_ptr<ASTNode> texpr) {
        texpr_ = std::move(texpr);
    }

    Define::Define() { type_ = NodeType::Define; }

    const std::string& Define::get_id() {
        return id_;
    }

    const ASTNode* Define::get_expr() {
        return expr_.get();
    }

    void Define::set_id(std::string id) {
        id_ = std::move(id);
    }

    void Define::set_expr(std::unique_ptr<ASTNode> expr) {
        expr_ = std::move(expr);
    }
}
