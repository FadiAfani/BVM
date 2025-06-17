#ifndef LIST_AST_H
#define LIST_AST_H

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>
namespace Lisp {

    enum class SExprType {
        IntLiteral,
        FloatLiteral,
        BoolLiteral,
        SymbolLiteral,
        StringLiteral,
        List,
    };

    class SExpr {
        protected:
            SExprType type_;
            size_t row_;
            size_t col_;
        public:
            virtual ~SExpr() = default;
            virtual const std::string print() = 0;
            SExprType get_type() { return type_; }
    };

    template<typename T>
    class Atom : public SExpr {
        protected:
            T value_;
        public:
            const T& get_value() {
                return value_;
            }
    };

    class IntAtom : public Atom<int> {
        public:
            IntAtom(int value);
            const std::string print() override;
    };

    class FloatAtom : public Atom<double> {
        public:
            FloatAtom(double value);
            const std::string print() override;
    };

    class StringAtom : public Atom<std::string> {
        public:
            StringAtom(std::string value);
            const std::string print() override;
    };

    class SymbolAtom : public Atom<std::string> {
        public:
            SymbolAtom(std::string value);
            const std::string print() override;
    };

    class BoolAtom : public Atom<bool> {
        public:
            BoolAtom(bool value);
            const std::string print() override;
    };

    class List : public SExpr {
        private:
            std::vector<std::unique_ptr<SExpr>> elems_;

        public:
            List();
            List(std::vector<std::unique_ptr<SExpr>> elems);
            void add_elem(std::unique_ptr<SExpr> expr);
            const std::vector<SExpr>& get_elems() const;
            const std::string print() override;
    };


    enum class NodeType {
        Lambda,
        Define,
        Cons,
        Qoute,
        Set,
        IfExpr,
        BinaryExpr,
        Plus,
        Minus,
        Mul,
        Div,
        Bt,
        Lt,
        Bte,
        Lte,
        Eq,
        Ne,
        ListExpr,
    };

    class ASTNode {
        protected:
            NodeType type_;
            size_t row;
            size_t col;
        public:
            NodeType get_type() const;
            virtual ~ASTNode() = default;
            //virtual std::string print() = 0;

    };

    struct Symbol {
        uint8_t reg;
        const ASTNode* node;
    };

    struct Scope {
        Scope* parent;
        std::unordered_map<std::string, Symbol> symbol_table;

        Symbol* lookup(const std::string& name);
    };

    class Lambda : public ASTNode {
        private:
            Scope scope_;
            std::unique_ptr<ASTNode> expr_;
        public:
            void set_expr(std::unique_ptr<ASTNode> expr);
            const ASTNode* get_expr() const;
            Lambda();
    };

    class ListExpr : public ASTNode {
        private:
            std::vector<std::unique_ptr<ASTNode>> elems_;
        public:
            const std::vector<std::unique_ptr<ASTNode>>& get_elems();
            void add_elem(std::unique_ptr<ASTNode> elem);
            ListExpr();
    };

    class BinaryExpr : public ASTNode {
        private:
            std::unique_ptr<ASTNode> left_;
            std::unique_ptr<ASTNode> right_;
        public:
            void set_left(std::unique_ptr<ASTNode> left);
            void set_right(std::unique_ptr<ASTNode> right);
            const ASTNode* get_left();
            const ASTNode* get_right();
            BinaryExpr();
    };

    class IfExpr : public ASTNode {
        private:
            std::unique_ptr<ASTNode> cond_;
            std::unique_ptr<ASTNode> texpr_;
            std::unique_ptr<ASTNode> fexpr_;
        public:
            void set_cond(std::unique_ptr<ASTNode> cond);
            void set_texpr(std::unique_ptr<ASTNode> true_expr);
            void set_fexpr(std::unique_ptr<ASTNode> false_expr);
            const ASTNode* get_cond();
            const ASTNode* get_texpr();
            const ASTNode* get_fexpr();
            IfExpr();
    };

    class Define : public ASTNode {
        private:
            std::string id_;
            std::unique_ptr<ASTNode> expr_;
        public:
            void set_id(std::string id);
            void set_expr(std::unique_ptr<ASTNode> expr);
            const std::string& get_id();
            const ASTNode* get_expr();
            Define();
    };

    struct Cons : public ASTNode {
        std::unique_ptr<ASTNode> a_;
        std::unique_ptr<ASTNode> b_;
        Cons();
    };

}

#endif
