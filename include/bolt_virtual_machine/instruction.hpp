#ifndef BVM_INSTRUCTION_H
#define BVM_INSTRUCTION_H

namespace BVM {
    enum class Opcode {
        OpAdd,
        OpDiv,
        OpMul,
        OpSub,
        OpMov,
        OpSchedule,
        OpRet,
        OpDefine,
        OpJmp,
        OpEq,
        OpNe,
        OpBt,
        OpLt,
        OpBte,
        OpLte,
        OpJmpIfFalse,
        OpConst,
    };
}




#endif
