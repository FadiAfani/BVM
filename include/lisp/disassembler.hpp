#include <lisp/codegen.hpp>


namespace Lisp {

    class Disassembler {
        private:
            std::string out_;
            const BVM::FuncObj* func_;

        public:
            Disassembler(const BVM::FuncObj* func);
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
