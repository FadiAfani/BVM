#include "lisp/ast.hpp"
#include <string>

namespace Lisp {

    Symbol* Scope::lookup(const std::string& name) {
        Scope* cur = this;
        while (cur) {
            if (cur->symbol_table.contains(name))
                return &cur->symbol_table[name];
            cur = cur->parent;
        }
        return nullptr;
    }
    

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

    std::unique_ptr<SExpr> List::move_elem(size_t i) {
        return std::move(elems_.at(i));
    }

    const std::vector<std::unique_ptr<SExpr>>& List::get_elems() const {
        return elems_;
    }

    NodeType ASTNode::get_type() const {
        return type_;
    }

    AtomicNode::AtomicNode(std::unique_ptr<SExpr> value) : value_(std::move(value)) {}

    const SExpr* AtomicNode::get_value() const {
        return value_.get();
    }

    Lambda::Lambda() { type_ = NodeType::Lambda; }

    void Lambda::insert_expr(std::unique_ptr<ASTNode> expr) {
        exprs_.push_back(std::move(expr));
    }

    const std::vector<std::unique_ptr<ASTNode>>& Lambda::get_exprs() const {
        return exprs_;
    }

    const std::vector<std::unique_ptr<AtomicNode>>& Lambda::get_parameters() const {
        return parameters_;
    }

    void Lambda::insert_parameter(std::unique_ptr<AtomicNode> p) {
        parameters_.push_back(std::move(p));
    }

    std::unique_ptr<ASTNode> Lambda::move_elem(size_t i) {
        return std::move(exprs_.at(i));
    }


    ListExpr::ListExpr() { type_ = NodeType::ListExpr; };


    const std::vector<std::unique_ptr<ASTNode>>& ListExpr::get_elems() const {
        return elems_;
    }

    void ListExpr::add_elem(std::unique_ptr<ASTNode> elem) {
        elems_.push_back(std::move(elem));
    }

    BinaryExpr::BinaryExpr() { type_ = NodeType::BinaryExpr; }

    const ASTNode* BinaryExpr::get_left() const { 
        return left_.get();
    }

    const ASTNode* BinaryExpr::get_right() const { 
        return right_.get();
    }

    void BinaryExpr::set_left(std::unique_ptr<ASTNode> left) {
        left_ = std::move(left);
    }

    void BinaryExpr::set_right(std::unique_ptr<ASTNode> right) {
        right_ = std::move(right);
    }

    IfExpr::IfExpr() { type_ = NodeType::IfExpr; }

    const ASTNode* IfExpr::get_fexpr() const {
        return fexpr_.get();
    }

    const ASTNode* IfExpr::get_texpr() const{
        return texpr_.get();
    }

    const ASTNode* IfExpr::get_cond() const {
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

    const std::string& Define::get_id() const {
        return id_;
    }

    const ASTNode* Define::get_expr() const {
        return expr_.get();
    }

    void Define::set_id(std::string id) {
        id_ = std::move(id);
    }

    void Define::set_expr(std::unique_ptr<ASTNode> expr) {
        expr_ = std::move(expr);
    }
}
