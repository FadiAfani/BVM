#include "lisp/ast.hpp"
#include <cassert>
#include <format>
#include <string>

namespace Lisp {

    const Symbol* Scope::lookup(const std::string& name) const {
        const Scope* cur = static_cast<const Scope*>(this);
        while (cur) {
            if (cur->symbol_table.contains(name))
                return &cur->symbol_table.at(name);
            cur = cur->parent;
        }
        return nullptr;
    }
    

    StringAtom::StringAtom(std::string value)  {
        value_ = std::move(value);
        type_ = SExprType::StringLiteral;
    }

    const std::string StringAtom::print() const {
        return value_;
    }

    SymbolAtom::SymbolAtom(std::string value)  {
        value_ = std::move(value);
        type_ = SExprType::SymbolLiteral;
    }

    const std::string SymbolAtom::print() const {
        return value_;
    }

    IntAtom::IntAtom(int value) {
        type_ = SExprType::IntLiteral;
        value_ = value;
    } 

    const std::string IntAtom::print() const {
        return std::to_string(value_);
    }

    FloatAtom::FloatAtom(double value) {
        type_ = SExprType::FloatLiteral;
        value_ = value;
    }

    const std::string FloatAtom::print() const {
        return std::to_string(value_);
    }
    
    BoolAtom::BoolAtom(bool value) {
        type_ = SExprType::BoolLiteral;
        value_ = value;
    }

    const std::string BoolAtom::print() const {
        return value_ ? "#t" : "#f";
    }


    List::List() { type_ = SExprType::List; }
    
    List::List(std::vector<std::unique_ptr<SExpr>> elems) : elems_(std::move(elems)) { type_ = SExprType::List; }

    void List::add_elem(std::unique_ptr<SExpr> expr) {}

    const std::string List::print() const {
        std::string res;
        for (auto& e : elems_) {
            res += " " + e->print() + " ";
        }
        return "( " + res + " )";
    }

    std::unique_ptr<SExpr> List::move_elem(size_t i) {
        return std::move(elems_.at(i));
    }

    const std::vector<std::unique_ptr<SExpr>>& List::get_elems() const {
        return elems_;
    }

    NodeType ASTNode::get_type() const {
        return type_;
    }

    AtomicNode::AtomicNode(std::unique_ptr<SExpr> value) : value_(std::move(value)) { type_ = NodeType::Atomic; }

    const SExpr* AtomicNode::get_value() const {
        return value_.get();
    }

    const std::string AtomicNode::print() const {
        return value_->print();
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

    Scope& Lambda::get_scope() {
        return scope_;
    }

    const Scope& Lambda::get_const_scope() const {
        return scope_;
    }

    const std::string Lambda::print() const {
        std::string res = "(Lambda ";
        for (auto& p : parameters_) {
            res += " " + p->print() + " ";
        }
        for (auto& e : exprs_) {
            res += " " + e->print() + " ";
        }
        return res + ")";
    }

    ListExpr::ListExpr() { type_ = NodeType::ListExpr; };


    const std::vector<std::unique_ptr<ASTNode>>& ListExpr::get_elems() const {
        return elems_;
    }

    void ListExpr::add_elem(std::unique_ptr<ASTNode> elem) {
        elems_.push_back(std::move(elem));
    }

    const std::string ListExpr::print() const {
        std::string res;
        for (auto& e : elems_) {
            res += " " + e->print() + " ";
        }
        return "( " + res + " )";
    }

    BinaryExpr::BinaryExpr() { type_ = NodeType::BinaryExpr; }

    const ASTNode* BinaryExpr::get_left() const { 
        return left_.get();
    }

    const ASTNode* BinaryExpr::get_right() const { 
        return right_.get();
    }

    ExprType BinaryExpr::get_op() const {
        return op_;
    }

    void BinaryExpr::set_left(std::unique_ptr<ASTNode> left) {
        left_ = std::move(left);
    }

    void BinaryExpr::set_right(std::unique_ptr<ASTNode> right) {
        right_ = std::move(right);
    }

    void BinaryExpr::set_op(ExprType op) {
        op_ = op;
    }

    const std::string BinaryExpr::print() const {
        std::string res;

        switch(op_) {
            case ExprType::Plus:
                res += "+";
                break;
            case ExprType::Minus:
                res += "-";
                break;
            case ExprType::Mul:
                res += "*";
                break;
            case ExprType::Div:
                res += "/";
                break;
            case ExprType::Eq:
                res += "=";
                break;
            case ExprType::Ne:
                res += "/=";
                break;
            case ExprType::Lte:
                res += "<=";
                break;
            case ExprType::Lt:
                res += "<";
                break;
            case ExprType::Bte:
                res += ">=";
                break;
            case ExprType::Bt:
                res += ">";
                break;
            default:
                throw std::logic_error("Not a valid binary op");
        }
        res += " " + left_->print() + " ";
        res += " " + right_->print() + " ";
        return "(" + res + ")";

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

    const std::string IfExpr::print() const {
        return "";
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

    const std::string Define::print() const {
        std::string res = "( define ";
        res += id_ + " ";
        res += expr_->print() + " )";

        return res;

    }
}
