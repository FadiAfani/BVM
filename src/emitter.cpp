#include "bolt_virtual_machine/emitter.h"
#include "bolt_virtual_machine/instruction.hpp"

#define EMIT_BINOP_IMPL(opname, op) \
    uint32_t Emitter::opname(uint8_t rd, uint8_t rt, uint8_t rs) { \
        return static_cast<uint8_t>(op) | rd >> 8 | rt >> 16 | rs >> 24; \
    } \

namespace BVM {

    EMIT_BINOP_IMPL(mul, Opcode::OpMul);
    EMIT_BINOP_IMPL(add, Opcode::OpAdd);

}
