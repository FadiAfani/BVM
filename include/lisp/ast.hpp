#ifndef LIST_AST_H
#define LIST_AST_H

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>
namespace Lisp {


    enum class ExprType {
        Lambda,
        Define,
        Cons,
        Qoute,
        Set,
        If,
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
    };


    const std::unordered_map<std::string, ExprType> reserved_funcs = {
        {"lambda", ExprType::Lambda},
        {"define", ExprType::Define},
        {"cons", ExprType::Cons},
        {"qoute", ExprType::Qoute},
        {"set!", ExprType::Set},
        {"if", ExprType::If},
        {"+", ExprType::Plus},
        {"-", ExprType::Minus},
        {"*", ExprType::Mul},
        {">", ExprType::Bt},
        {">=", ExprType::Bte},
        {"<", ExprType::Lt},
        {"<=", ExprType::Lte},
        {"/=", ExprType::Ne},
        {"=", ExprType::Eq},
    };

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
            virtual const std::string print() const = 0;
            SExprType get_type() const { return type_; }
            inline bool is_list() {
                return type_ == SExprType::List;
            }
            inline bool is_atom() {
                return type_ != SExprType::List;
            }
    };

    template<typename T>
    class Atom : public SExpr {
        protected:
            T value_;
        public:
            const T& get_value() const {
                return value_;
            }
    };

    class IntAtom : public Atom<int> {
        public:
            IntAtom(int value);
            const std::string print() const override;
    };

    class FloatAtom : public Atom<double> {
        public:
            FloatAtom(double value);
            const std::string print() const override;
    };

    class StringAtom : public Atom<std::string> {
        public:
            StringAtom(std::string value);
            const std::string print() const override;
    };

    class SymbolAtom : public Atom<std::string> {
        public:
            SymbolAtom(std::string value);
            const std::string print() const override;
    };

    class BoolAtom : public Atom<bool> {
        public:
            BoolAtom(bool value);
            const std::string print() const override;
    };

    class List : public SExpr {
        private:
            std::vector<std::unique_ptr<SExpr>> elems_;

        public:
            List();
            List(std::vector<std::unique_ptr<SExpr>> elems);
            void add_elem(std::unique_ptr<SExpr> expr);
            const std::vector<std::unique_ptr<SExpr>>& get_elems() const;
            std::unique_ptr<SExpr> move_elem(size_t i);
            const std::string print() const override;
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
        Atomic,
    };

    class ASTNode {
        protected:
            NodeType type_;
            size_t row;
            size_t col;
        public:
            NodeType get_type() const;
            virtual ~ASTNode() = default;
            virtual const std::string print() const = 0;

    };

    struct Symbol {
        uint8_t reg;
        const ASTNode* node;
    };

    struct Scope {
        Scope* parent;
        std::unordered_map<std::string, Symbol> symbol_table;

        const Symbol* lookup(const std::string& name) const;
        void insert(const std::string id, Symbol sym);
    };

    class AtomicNode : public ASTNode {
        private:
            std::unique_ptr<SExpr> value_;
        public:
            AtomicNode(std::unique_ptr<SExpr> value);
            const SExpr* get_value() const;
            const std::string print() const override;
            
    };

    class Lambda : public ASTNode {
        private:
            Scope scope_;
            std::vector<std::unique_ptr<AtomicNode>> parameters_;
            std::vector<std::unique_ptr<ASTNode>> exprs_;
        public:
            Lambda();
            void insert_parameter(std::unique_ptr<AtomicNode> p);
            void insert_expr(std::unique_ptr<ASTNode> expr);
            const std::vector<std::unique_ptr<AtomicNode>>& get_parameters() const;
            const std::vector<std::unique_ptr<ASTNode>>& get_exprs() const;
            std::unique_ptr<ASTNode> move_elem(size_t i);
            Scope& get_scope();
            const Scope& get_const_scope() const;
            const std::string print() const override;
    };

    class ListExpr : public ASTNode {
        private:
            std::vector<std::unique_ptr<ASTNode>> elems_;
        public:
            const std::vector<std::unique_ptr<ASTNode>>& get_elems() const;
            void add_elem(std::unique_ptr<ASTNode> elem);
            const std::string print() const override;
            ListExpr();
    };


    class BinaryExpr : public ASTNode {
        private:
            ExprType op_;
            std::unique_ptr<ASTNode> left_;
            std::unique_ptr<ASTNode> right_;
        public:
            void set_left(std::unique_ptr<ASTNode> left);
            void set_right(std::unique_ptr<ASTNode> right);
            void set_op(ExprType op);
            const ASTNode* get_left() const;
            const ASTNode* get_right() const;
            ExprType get_op() const;
            const std::string print() const override;
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
            const ASTNode* get_cond() const;
            const ASTNode* get_texpr() const;
            const ASTNode* get_fexpr() const;
            const std::string print() const override;
            IfExpr();
    };

    class Define : public ASTNode {
        private:
            std::string id_;
            std::unique_ptr<ASTNode> expr_;
        public:
            void set_id(std::string id);
            void set_expr(std::unique_ptr<ASTNode> expr);
            const std::string& get_id() const;
            const ASTNode* get_expr() const;
            const std::string print() const override;
            Define();
    };

}

#endif
