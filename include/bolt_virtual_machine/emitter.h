#ifndef BVM_EMITTER_H 
#define BVM_EMITTER_H

#include <cstdint>

//NOTE: mapping each instruction to a function seems to be easier to write and read - might replace these functionswith a single emit_binop function

#define EMIT_BINOP_DEF(opname) \
    static uint32_t opname(uint8_t rd, uint8_t rt, uint8_t rs);

namespace BVM {

    class Emitter {

        public:
            EMIT_BINOP_DEF(mul);
            EMIT_BINOP_DEF(fmul);
            EMIT_BINOP_DEF(sdiv);
            EMIT_BINOP_DEF(udiv);
            EMIT_BINOP_DEF(fdiv);
            EMIT_BINOP_DEF(sub);
            EMIT_BINOP_DEF(fsub);
            EMIT_BINOP_DEF(add);
            EMIT_BINOP_DEF(fadd);
            static uint32_t emit_mov(uint8_t rd, uint8_t rt);
    };

}

#endif 
