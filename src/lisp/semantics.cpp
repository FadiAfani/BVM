#include <cassert>
#include<lisp/semantics.hpp>
#include <stdexcept>

namespace Lisp {



    SemanticAnalyzer::SemanticAnalyzer(Program& program) : program_(program) {}

    
    std::unique_ptr<Lambda> SemanticAnalyzer::verify() {
        auto main = std::make_unique<Lambda>();
        scopes_.push(&main->get_scope());

        for (auto& expr : program_) {
            auto node = verify_sexpr(std::move(expr));
            main->insert_expr(std::move(node));
        }

        return std::move(main);
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

        Scope* env = scopes_.top();
        if (!reserved_keywords.contains(sexpr->get_value())) {
            if (env->symbol_table.size() >= MAX_REGS)
                throw std::runtime_error("Can't have more than 255 active locals");
            auto name = sexpr->get_value();
            uint8_t reg = env->symbol_table.size();
            auto node = std::make_unique<AtomicNode>(std::move(sexpr));
            env->symbol_table[name] = {reg, node.get()};
            return std::move(node);
        }
        return nullptr;
    }

    std::unique_ptr<ASTNode> SemanticAnalyzer::verify_list(std::unique_ptr<List> sexpr) {
        auto first = sexpr->get_elems().at(0).get();
        if (!first->is_atom())
            throw std::runtime_error("improper list structure");

        const std::string& name = static_cast<SymbolAtom*>(first)->get_value();

        switch(reserved_funcs.at(name)) {
            case ExprType::Define:
                return verify_define(std::move(sexpr));
            case ExprType::Lambda:
                return verify_lambda(std::move(sexpr));
            case ExprType::If:
                return verify_if(std::move(sexpr));
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
                return verify_binary(std::move(sexpr));
            case ExprType::List:
                return verify_list_expr(std::move(sexpr));

            default:
                std::logic_error("verify_list: Not Implemented");
        }
        return nullptr;
    }


    std::unique_ptr<Define> SemanticAnalyzer::verify_define(std::unique_ptr<List> sexpr) {
        auto& elems = sexpr->get_elems();
        auto node = std::make_unique<Define>();
        if (elems.size() != 3 || elems[1]->get_type() != SExprType::SymbolLiteral)
            throw std::runtime_error("malformed define");
        auto sym = static_cast<SymbolAtom*>(elems[1].get());
        auto symbol = verify_sexpr(sexpr->move_elem(1));
        auto e = sexpr->move_elem(2);
        auto expr = verify_sexpr(std::move(e));

        node->set_id(sym->get_value());
        node->set_expr(std::move(expr));

        return std::move(node);
    }

    std::unique_ptr<BinaryExpr> SemanticAnalyzer::verify_binary(std::unique_ptr<List> sexpr) {
        auto& elems = sexpr->get_elems();
        auto node = std::make_unique<BinaryExpr>();
        if (elems.size() != 3) 
            throw std::runtime_error("malformed operation");

        auto lhs = verify_sexpr(sexpr->move_elem(1));
        auto rhs = verify_sexpr(sexpr->move_elem(2));
        node->set_left(std::move(lhs));
        node->set_right(std::move(rhs));
        auto sym = elems[0].get();
        node->set_op(reserved_funcs.at(static_cast<SymbolAtom*>(sym)->get_value()));

        return std::move(node);
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

        return std::move(node);

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


        return std::move(node);
    }

    std::unique_ptr<ListExpr> SemanticAnalyzer::verify_list_expr(std::unique_ptr<List> sexpr) {
        auto node = std::make_unique<ListExpr>();

        for (size_t i = 1; i < sexpr->get_elems().size(); i++) {
            auto e = verify_sexpr(sexpr->move_elem(i));
            node->add_elem(std::move(e));
        }

        return node;
    }

}
