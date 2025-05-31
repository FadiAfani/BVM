#include <cassert>
#include<lisp/semantics.hpp>
#include <stdexcept>
#include <variant>

namespace Lisp {

    SemanticAnalyzer::SemanticAnalyzer() {

    }

    void SemanticAnalyzer::visit(const Atom& node) {
        Scope* env = scopes.back().get();
        if (node.get_type() != NodeType::SymbolLiteral)
            env->locals.insert(&node);
    }

    void SemanticAnalyzer::visit(const List& node) {
        auto first = node.get_elems().at(0);
        if (std::holds_alternative<Atom>(first)) {
            std::string& name = std::get<Atom>(first).get_value<std::string>();
            switch(expr_types.at(name.data())) {
                case ExprType::Define:
                    verify_define(node);
                    break;
                case ExprType::Lambda:
                {
                    auto ns = std::make_unique<Scope>();
                    ns->owner = &node;
                    scopes.push_back(std::move(ns));
                    verify_lambda(node);
                }
                break;

                default:
                    std::logic_error("Not Implemented");
            }
        }

        for (const Expr& expr : node.get_elems()) {
            if (std::holds_alternative<List>(expr))
                visit(std::get<List>(expr));
            else
                visit(std::get<Atom>(expr));
        }
    }

    void SemanticAnalyzer::verify_define(const List& node) {
        auto elems = node.get_elems();
        auto symbol = std::get<Atom>(elems.at(1));
        
        if (elems.size() < 3)
            throw std::runtime_error("ill-formed define");
        if (std::holds_alternative<Atom>(elems[1]) && std::get<Atom>(elems[1]).get_type() != NodeType::SymbolLiteral) {
            throw std::runtime_error("expected a symbol after 'define'");
        }

    }

    void SemanticAnalyzer::verify_lambda(const List& node) {
        auto elems = node.get_elems();

        if (elems.size() < 3) {
            throw std::runtime_error("malformed lambda expression");
        }

        if (!std::holds_alternative<List>(elems.at(1))) {
            throw std::runtime_error("expected a list of arguments after 'lambda'");
        }
    }

    void SemanticAnalyzer::verify_if(const List& node) {
        auto elems = node.get_elems();

        if (elems.size() < 4 || !std::holds_alternative<Atom>(elems[0]) || std::get<Atom>(elems[0]).get_value<std::string>() != "if")
            throw std::runtime_error("malformed if expression");
    }

    void SemanticAnalyzer::verify_qoute(const List& node) {
        auto elems = node.get_elems();

        if (elems.size() != 2)
            throw std::runtime_error("qoute is only applicable to a single expression");
    }

    void SemanticAnalyzer::verify_set(const List& node) {
        auto elems = node.get_elems();

        if (elems.size() != 3) {
            throw std::runtime_error("malformed set! expression");
        }
    }

    bool SemanticAnalyzer::has_value(const List& node) {
        if (node.get_elems().size() == 0)
            return true;

        auto symbol = node.get_elems()[0];

        if (std::holds_alternative<List>(symbol))
            return false;

        switch(expr_types.at(std::get<Atom>(symbol).get_value<std::string>().data())) {
            case ExprType::Lambda:
            case ExprType::Qoute:
            case ExprType::Cons:
                return true;
            case ExprType::Set:
            case ExprType::Define:
                    return false;

        }

        return false;

    }

}
