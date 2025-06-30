#include "bolt_virtual_machine/vm.hpp"
#include <format>
#include <lisp/disassembler.hpp>
#include <bolt_virtual_machine/instruction.hpp>

namespace Lisp {

    Disassembler::Disassembler(const BVM::FuncObj* func) : func_(func) {}

    const std::string& Disassembler::disassemble() {
        for (size_t i = 0; i < func_->instructions.size();i++) {
            uint32_t inst = func_->instructions[i];
            uint8_t rd, rt, rs;
            rd = BVM::VirtualMachine::decode_rd(inst);
            rt = BVM::VirtualMachine::decode_rt(inst);
            rs = BVM::VirtualMachine::decode_rs(inst);
            switch(BVM::VirtualMachine::decode_op(inst)) {
                case BVM::Opcode::OpAdd:
                    out_ += std::format("add {}, {}, {}\n", rd, rt, rs).data();
                    break;
                case BVM::Opcode::OpSub:
                    out_ += std::format("sub {}, {}, {}\n", rd, rt, rs).data();
                    break;
                case BVM::Opcode::OpMul:
                    out_ += std::format("mul {}, {}, {}\n", rd, rt, rs).data();
                    break;
                case BVM::Opcode::OpDiv:
                    out_ += std::format("div {}, {}, {}\n", rd, rt, rs).data();
                    break;
                case BVM::Opcode::OpEq:
                    out_ += std::format("eq {}, {}, {}\n", rd, rt, rs).data();
                    break;
                case BVM::Opcode::OpNe:
                    out_ += std::format("ne {}, {}, {}\n", rd, rt, rs).data();
                    break;
                case BVM::Opcode::OpBt:
                    out_ += std::format("bt {}, {}, {}\n", rd, rt, rs).data();
                    break;
                case BVM::Opcode::OpBte:
                    out_ += std::format("bte {}, {}, {}\n", rd, rt, rs).data();
                    break;
                case BVM::Opcode::OpLt:
                    out_ += std::format("lt {}, {}, {}\n", rd, rt, rs).data();
                    break;
                case BVM::Opcode::OpLte:
                    out_ += std::format("lte {}, {}, {}\n", rd, rt, rs).data();
                    break;
                case BVM::Opcode::OpMov:
                    out_ += std::format("mov {}, {} \n", rd, rt).data();
                    break;
                case BVM::Opcode::OpRet:
                    out_ += "ret\n";
                    break;
                case BVM::Opcode::OpJmp:
                    out_ += std::format("jmp {}\n", inst >> 8);
                    break;
                case BVM::Opcode::OpJmpIfFalse:
                    out_ += std::format("jmp_false {}, {} \n", rd, inst >> 16);
                    break;
                case BVM::Opcode::OpDefine:
                    out_ += std::format("define {}, {} \n", rd, rs);
                    break;
                case BVM::Opcode::OpConst:
                    out_ += std::format("const {}, {}\n", rd, inst >> 16);
                    break;
                case BVM::Opcode::OpCallNative:
                    out_ += std::format("call_native {}, {}, {}\n", rd, rt, rs);
                    break;
                case BVM::Opcode::OpCall:
                    out_ += std::format("call {}\n", rd);
                    break;
                default:
                    throw std::runtime_error("disassembler: Not Implemented");
            }

        }
        return out_;
    }
}
