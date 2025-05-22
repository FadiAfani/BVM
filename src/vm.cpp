#include "bolt_virtual_machine/vm.hpp"
#include "bolt_virtual_machine/emitter.h"
#include "bolt_virtual_machine/instruction.hpp"
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


    void VirtualMachine::setup_entry_point() {
        Callable* main = callables_.at(0).get();
        push(std::bit_cast<uint64_t>(static_cast<int64_t>(-1)));
        push(0);
        push(std::bit_cast<uint64_t>(main));
        fp_ = STACK_SIZE - 1;
        sp_ -= main->n_locals; // callable_ref + prev_fp + return addr

    };

    void VirtualMachine::handle_interrupt(Interrupt interrupt) {
        return;
    }


    Interrupt VirtualMachine::execute(uint32_t inst) {
        uint8_t rd, rs, rt;
        uint64_t rdv, rsv, rtv;
        Opcode op;

        op = decode_op(inst);
        rd = decode_rd(inst);
        rs = decode_rs(inst);
        rt = decode_rt(inst);

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
        return Interrupt::Ok;
    }

    void VirtualMachine::run() {
        uint32_t inst = fetch();
        Interrupt interrupt;
        for (;;) {
            interrupt = execute(inst);
            if (interrupt != Interrupt::Ok) {
                handle_interrupt(interrupt);
                return;
            }
            
        }
    }
}

