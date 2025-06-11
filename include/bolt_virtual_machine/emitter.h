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
            EMIT_BINOP_DEF(div);
            EMIT_BINOP_DEF(sub);
            EMIT_BINOP_DEF(add);
            EMIT_BINOP_DEF(eq);
            EMIT_BINOP_DEF(ne);
            EMIT_BINOP_DEF(lte);
            EMIT_BINOP_DEF(bte);
            EMIT_BINOP_DEF(lt);
            EMIT_BINOP_DEF(bt);
            static uint32_t mov(uint8_t rd, uint8_t rt);
            static uint32_t ret();
            static uint32_t jmp_if_false(uint8_t rd, uint32_t rt);
            static uint32_t load_const(uint8_t rd, uint16_t idx);
    };

}

#endif 
