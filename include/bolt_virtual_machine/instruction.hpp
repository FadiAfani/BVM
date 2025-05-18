#ifndef BVM_INSTRUCTION_H
#define BVM_INSTRUCTION_H

namespace BVM {
    enum class Opcode {
        OpAdd,
        OpFadd,
        OpSdiv,
        OpUdiv,
        OpFdiv,
        OpMul,
        OpFmul,
        OpSub,
        OpFsub,
        OpMov,
        OpSchedule,
    };
}




#endif
