#ifndef LISP_SEMANTICS_H
#define LISP_SEMANTICS_H

#include <lisp/parser.hpp>
#include <stack>
#include <unordered_map>

namespace Lisp {

    struct Scope {
        std::unordered_set<std::string> symbol_table;
        std::unordered_map<const Atom*, unsigned int> locals;
    };

    class SemanticAnalyzer {

        private:
            std::stack<Scope> scopes;
            Program ast_;

        public:
            SemanticAnalyzer(Program progam);
            void visit(const List& node);
            void visit(const Atom& node);
    };

}


#endif
