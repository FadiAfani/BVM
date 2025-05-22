#include "bolt_virtual_machine/emitter.h"
#include "bolt_virtual_machine/instruction.hpp"

#define EMIT_BINOP_IMPL(opname, op) \
    uint32_t Emitter::opname(uint8_t rd, uint8_t rt, uint8_t rs) { \
        return static_cast<uint8_t>(op) | rd << 8 | rt << 16 | rs << 24; \
    } \

namespace BVM {

    EMIT_BINOP_IMPL(mul, Opcode::OpMul);
    EMIT_BINOP_IMPL(add, Opcode::OpAdd);
    EMIT_BINOP_IMPL(fadd, Opcode::OpFadd);
    uint32_t Emitter::mov(uint8_t rd, uint8_t rt) {
        return static_cast<uint8_t>(Opcode::OpMov) | rd << 8 | rt << 16;
    }
    uint32_t Emitter::ret() { return static_cast<uint32_t>(Opcode::OpRet); }

}
