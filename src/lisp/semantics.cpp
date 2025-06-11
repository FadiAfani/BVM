#include <cassert>
#include<lisp/semantics.hpp>
#include <stdexcept>
#include <variant>

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

    SemanticAnalyzer::SemanticAnalyzer() {}

    SemanticAnalyzer::SemanticAnalyzer(Program program) {
        annotated_program_ = std::make_unique<AnnotatedProgram>();
        annotated_program_->program = std::move(program);
    }

    
    std::unique_ptr<AnnotatedProgram> SemanticAnalyzer::analyze_program() {
        auto gs = std::make_unique<Scope>();
        scopes_.push(gs.get());
        annotated_program_->globals = std::move(gs);

        for (const Expr& sexpr : annotated_program_->program) {
            if (std::holds_alternative<List>(sexpr))
                visit(std::get<List>(sexpr));
            else
                visit(std::get<Atom>(sexpr));
        }

        return std::move(annotated_program_);
    }

    void SemanticAnalyzer::visit(const Atom& node) {
        Scope* env = scopes_.top();
        if (node.get_type() == NodeType::SymbolLiteral 
                && !reserved_keywords.contains(node.get_value<std::string>())) {

            if (env->symbol_table.size() >= 255)
                throw std::runtime_error("Can't have more than 255 active locals");
            auto name = node.get_value<std::string>();
            uint8_t reg = env->symbol_table.size();
            env->symbol_table[name] = {reg, &node};
        }
    }

    void SemanticAnalyzer::visit(const List& node) {
        auto first = node.get_elems().at(0);
        if (std::holds_alternative<Atom>(first)) {
            const std::string& name = std::get<Atom>(first).get_value<std::string>();
            //printf("name: %s, %ld\n", name.data(), name.length());
            switch(expr_types.at(name)) {
                case ExprType::Define:
                    verify_define(node);
                    break;
                case ExprType::Lambda:
                {
                    annotated_program_->scopes[&node] = std::make_unique<Scope>();
                    Scope* ns = annotated_program_->scopes[&node].get();
                    ns->parent = scopes_.top();
                    scopes_.push(ns);
                    verify_lambda(node);
                    break;
                }

                case ExprType::If:
                    verify_if(node);
                    break;
                case ExprType::Plus:
                case ExprType::Minus:
                case ExprType::Div:
                case ExprType::Mul:
                case ExprType::Bt:
                case ExprType::Lt:
                case ExprType::Bte:
                case ExprType::Lte:
                case ExprType::Eq:
                case ExprType::Ne:
                    verify_binary(node);
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

        if (std::holds_alternative<List>(elems[3]) && !has_value(std::get<List>(elems[3])))
            throw std::runtime_error("expression does not produce a value");

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

            default:
                std::logic_error("SemanticAnalyzer::has_value - Not Implemented");

        }

        return false;

    }

    void SemanticAnalyzer::verify_binary(const List& node) {
        auto elems = node.get_elems();
        if (elems.size() != 3)
            throw std::runtime_error("binary expression cannot have more than two operands");
    }


}
