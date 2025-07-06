#include <cassert>
#include<lisp/semantics.hpp>
#include <stdexcept>
#include <format>

namespace Lisp {



    SemanticAnalyzer::SemanticAnalyzer(Program& program) : program_(program) {}

    
    std::unique_ptr<Lambda> SemanticAnalyzer::verify() {
        auto main = std::make_unique<Lambda>();
        Scope& globals = main->get_scope();
        globals.symbol_table = {
            {"lambda", {0, nullptr, SymbolType::SpecialForm}},
            {"if", {0, nullptr, SymbolType::SpecialForm}},
            {"define", {0, nullptr, SymbolType::SpecialForm}},
            {"cons", {0, nullptr, SymbolType::SpecialForm}},
            {"+", {0, nullptr, SymbolType::NativeProc, BVM::Primitives::Add}},
            {"-", {0, nullptr, SymbolType::NativeProc, BVM::Primitives::Sub}},
            {"*", {0, nullptr, SymbolType::NativeProc, BVM::Primitives::Mul}},
            {"/", {0, nullptr, SymbolType::NativeProc, BVM::Primitives::Div}},
            {"=", {0, nullptr, SymbolType::NativeProc, BVM::Primitives::Eq}},
            {"/=", {0, nullptr, SymbolType::NativeProc, BVM::Primitives::Ne}},
            {">", {0, nullptr, SymbolType::NativeProc, BVM::Primitives::Bt}},
            {">=", {0, nullptr, SymbolType::NativeProc, BVM::Primitives::Bte}},
            {"<", {0, nullptr, SymbolType::NativeProc, BVM::Primitives::Lt}},
            {"<=", {0, nullptr, SymbolType::NativeProc, BVM::Primitives::Lte}},
        };
        scopes_.push(&main->get_scope());

        for (auto& expr : program_) {
            auto node = verify_sexpr(std::move(expr));
            main->insert_expr(std::move(node));
        }

        return main;
    }


    std::unique_ptr<ASTNode> SemanticAnalyzer::verify_sexpr(std::unique_ptr<SExpr> sexpr) {
        switch(sexpr->get_type()) {
            case SExprType::SymbolLiteral:
                return verify_symbol(std::unique_ptr<SymbolAtom>(static_cast<SymbolAtom*>(sexpr.release())));
            case SExprType::List:
                return verify_list(std::unique_ptr<List>(static_cast<List*>(sexpr.release())));
            case SExprType::BoolLiteral:
            case SExprType::FloatLiteral:
            case SExprType::IntLiteral:
            case SExprType::StringLiteral:
                return std::make_unique<AtomicNode>(std::move(sexpr));
            case Lisp::SExprType::QuotedExpr:
                throw std::logic_error("verify_sexpr: qouted expressions are yet to be supported");

        }

    }

    std::unique_ptr<AtomicNode> SemanticAnalyzer::verify_symbol(std::unique_ptr<SymbolAtom> sexpr) {
        Scope* cur = scopes_.top();
        const std::string& sym = sexpr->get_value();
        if (cur->lookup(sym) == nullptr)
            throw std::runtime_error(std::format("symbol '{}' is not defined", sym.data()));

        return std::make_unique<AtomicNode>(std::move(sexpr));
    }

    std::unique_ptr<ASTNode> SemanticAnalyzer::verify_list(std::unique_ptr<List> sexpr) {
        auto first = sexpr->get_elems().at(0).get();
        if (!first->is_atom())
            throw std::runtime_error("improper list structure");

        const std::string& name = static_cast<SymbolAtom*>(first)->get_value();

        if (reserved_funcs.contains(name)) {

            switch(reserved_funcs.at(name)) {
                case ExprType::Define:
                    return verify_define(std::move(sexpr));
                case ExprType::Lambda:
                    return verify_lambda(std::move(sexpr));
                case ExprType::If:
                    return verify_if(std::move(sexpr));
                default:
                    std::logic_error("verify_list: Not Implemented");
            }

        }

        return verify_proc_call(std::move(sexpr));
    }


    std::unique_ptr<Define> SemanticAnalyzer::verify_define(std::unique_ptr<List> sexpr) {
        Scope* scope = scopes_.top();
        auto& elems = sexpr->get_elems();
        auto node = std::make_unique<Define>();
        if (elems.size() != 3 || elems[1]->get_type() != SExprType::SymbolLiteral)
            throw std::runtime_error("malformed define");
        SymbolAtom* sym = static_cast<SymbolAtom*>(elems[1].get());
        scope->insert(sym->get_value(), {static_cast<uint8_t>(scope->n_vars), nullptr, SymbolType::Variable});
        auto symbol = verify_sexpr(sexpr->move_elem(1));
        auto e = sexpr->move_elem(2);
        auto expr = verify_sexpr(std::move(e));

        node->set_id(sym->get_value());
        node->set_expr(std::move(expr));

        return node;
    }


    std::unique_ptr<IfExpr> SemanticAnalyzer::verify_if(std::unique_ptr<List> sexpr) {
        auto& elems = sexpr->get_elems();
        auto node = std::make_unique<IfExpr>();
        if (elems.size() != 4)
            throw std::runtime_error("malformed if expression");

        auto cond = verify_sexpr(sexpr->move_elem(1));
        auto texpr = verify_sexpr(sexpr->move_elem(2));
        auto fexpr = verify_sexpr(sexpr->move_elem(3));
        node->set_cond(std::move(cond));
        node->set_texpr(std::move(texpr));
        node->set_fexpr(std::move(fexpr));

        return node;

    }

    std::unique_ptr<Lambda> SemanticAnalyzer::verify_lambda(std::unique_ptr<List> sexpr) {
        auto& elems = sexpr->get_elems();
        auto node = std::make_unique<Lambda>();
        if (elems.size() != 3 || !elems[1]->is_list())
            throw std::runtime_error("malformed lambda");

        List* ps_list = static_cast<List*>(elems[1].get());
        auto& ps = ps_list->get_elems();
        for (size_t i = 0; i < ps.size(); i++) {
            auto e = ps_list->move_elem(i);
            if (!e->is_atom())
                throw std::runtime_error("not a valid parameter");
            scopes_.push(&node->get_scope());
            auto atom = verify_symbol(std::unique_ptr<SymbolAtom>(static_cast<SymbolAtom*>(e.release())));
            node->insert_parameter(std::move(atom));
        }

        for (size_t i = 2; i < sexpr->get_elems().size(); i++) {
            auto expr = verify_sexpr(sexpr->move_elem(i));
            node->insert_expr(std::move(expr));
        }


        return node;
    }
    
    std::unique_ptr<ProcCall> SemanticAnalyzer::verify_proc_call(std::unique_ptr<List> sexpr) {
        auto first = sexpr->get_elems().at(0).get();
        const std::string& name = static_cast<SymbolAtom*>(first)->get_value();
        auto scope = scopes_.top();

        if (!native_funcs.contains(name) && scope->lookup(name) == nullptr)
            throw std::runtime_error(std::format("'{}' is not defined", name.data()));

        auto& elems = sexpr->get_elems();
        std::unique_ptr<ProcCall> node = std::make_unique<ProcCall>();
        std::unique_ptr<AtomicNode> id = verify_symbol(std::unique_ptr<SymbolAtom>(static_cast<SymbolAtom*>(sexpr->move_elem(0).release())));

        node->set_proc(std::move(id));

        for (size_t i = 1; i < elems.size(); i++) {
            auto e = verify_sexpr(sexpr->move_elem(i));
            node->add_arg(std::move(e));
        }

        return node;

    }

}
