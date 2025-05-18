#include "vm.hpp"
#include "instruction.hpp"
#include <bit>
#include <cstdint>
#include <stdexcept>

#define BINARY_OP(inst, value)  \
    rd = decode_rd(inst); \
    rt = decode_rt(inst); \
    rs = decode_rs(inst); \
    set_register_value(rd, value); \

namespace BVM {

    VirtualMachine::VirtualMachine() {}
    VirtualMachine::~VirtualMachine() {}

    void VirtualMachine::load_program(const std::vector<uint32_t>* program) {
        if (program == nullptr)
            throw std::runtime_error("invalid program reference");
        program_ = program;
    }

    uint32_t VirtualMachine::fetch() noexcept {
        return (*program_)[ip_++];
    }

    Opcode VirtualMachine::decode_op(uint32_t inst) noexcept {
        return static_cast<Opcode>(inst);
    }

    uint8_t VirtualMachine::decode_rd(uint32_t inst) noexcept {
        return (uint8_t) (inst >> 8);
    }

    uint8_t VirtualMachine::decode_rt(uint32_t inst) noexcept {
        return (uint8_t) (inst >> 16);
    }

    uint8_t VirtualMachine::decode_rs(uint32_t inst) noexcept {
        return (uint8_t) (inst >> 24);
    }

    inline uint64_t VirtualMachine::get_register_value(uint8_t r) noexcept { return stack_[lp_ - r]; }

    inline void VirtualMachine::set_register_value(uint8_t r, uint64_t value) noexcept {
        stack_[lp_ - r] = value;
    }



    Interrupt VirtualMachine::execute() {
        uint8_t inst, rd, rs, rt;
        uint64_t rdv, rsv, rtv;
        Opcode op;
        Interrupt status;

        for (;;) {
            inst = fetch();
            op = decode_op(inst);

            switch(op) {
                case Opcode::OpMul:
                    rtv = get_register_value(rt);
                    rsv = get_register_value(rs);
                    BINARY_OP(inst, rtv * rsv);
                    break;
                case Opcode::OpFmul:
                    rtv = get_register_value(rt);
                    rsv = get_register_value(rs);
                    BINARY_OP(inst, (std::bit_cast<uint64_t>(
                                    static_cast<double>(rtv) * static_cast<double>(rsv))
                                )
                    );
                    break;

                case Opcode::OpSdiv:
                    rtv = get_register_value(rt);
                    rsv = get_register_value(rs);
                    BINARY_OP(inst, (std::bit_cast<uint64_t>(
                                    static_cast<int64_t>(rtv) / static_cast<int64_t>(rsv))
                                )
                            );
                    break;

                case Opcode::OpUdiv:
                    rtv = get_register_value(rt);
                    rsv = get_register_value(rs);
                    BINARY_OP(inst, rtv / rsv);
                    break;

                case Opcode::OpFdiv:
                    rtv = get_register_value(rt);
                    rsv = get_register_value(rs);
                    BINARY_OP(inst, (std::bit_cast<uint64_t>(
                                    static_cast<double>(rtv) / static_cast<double>(rsv))
                                )
                    );
                    break;

                case Opcode::OpSub:
                    rtv = get_register_value(rt);
                    rsv = get_register_value(rs);
                    BINARY_OP(inst, rtv - rsv);
                    break;

                case Opcode::OpFsub:
                    rtv = get_register_value(rt);
                    rsv = get_register_value(rs);
                    BINARY_OP(inst, (std::bit_cast<uint64_t>(
                                    static_cast<double>(rtv) - static_cast<double>(rsv))
                                )
                    );
                    break;
                case Opcode::OpAdd:
                    rtv = get_register_value(rt);
                    rsv = get_register_value(rs);
                    BINARY_OP(inst, rtv + rsv);
                    break;
                case Opcode::OpFadd:
                    rtv = get_register_value(rt);
                    rsv = get_register_value(rs);
                    BINARY_OP(inst, (std::bit_cast<uint64_t>(
                                    static_cast<double>(rtv) + static_cast<double>(rsv))
                                )
                    );
                    break;
                case Opcode::OpMov:
                    rd = decode_rd(inst);
                    rs = decode_rs(inst);
                    set_register_value(rd, get_register_value(rs));
                    break;
                case Opcode::OpSchedule:
                    throw std::runtime_error("Not Implemented\n");
                    break;
            }
        }

        return Interrupt::Ok;
    }
}
