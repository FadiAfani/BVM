#include <lisp/codegen.hpp>


namespace Lisp {

    class Disassembler {
        private:
            std::string out_;
            const FuncObj* func_;

        public:
            Disassembler(const FuncObj* func);
            const std::string& disassemble();
            void decode_mov();
            void decode_jmp_if_false();
            void decode_add();
            void decode_sub();
            void decode_mul();
            void decode_div();
            void decode_const();

    };
}
