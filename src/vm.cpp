#include "bolt_virtual_machine/vm.hpp"
#include "bolt_virtual_machine/instruction.hpp"
#include <cstdint>
#include <stdexcept>


#define BINARY_OP(op, rd, x, y) \
    if (x.type == BoltType::Integer && y.type == BoltType::Integer) \
        set_register_value(rd, {.as_int = x.as_int op y.as_int, .type = BoltType::Integer}); \
    else if ((x.type == BoltType::Integer && y.type == BoltType::Float) \
            || (x.type == BoltType::Float && y.type == BoltType::Integer) \
            || (x.type == BoltType::Float && y.type == BoltType::Float)) \
        set_register_value(rd, {.as_double = x.as_double op y.as_double, .type = BoltType::Float}); \
    else \
        return Interrupt::IncompatibleTypes; \

namespace BVM {

    VirtualMachine::VirtualMachine() {}
    VirtualMachine::~VirtualMachine() {}

    void VirtualMachine::setup_entry_point() {
        Callable* main = callables_.at(0).get();
        BoltValue prev_fp = {.as_int = -1, .type = BoltType::Integer };
        BoltValue ret_addr = {.as_int = 0, .type = BoltType::Integer };
        BoltValue func = {.as_func = main, .type = BoltType::Function };
        push(prev_fp);
        push(ret_addr);
        push(func);
        fp_ = STACK_SIZE - 1;
        sp_ -= main->n_locals; // callable_ref + prev_fp + return addr

    };

    void VirtualMachine::handle_interrupt(Interrupt interrupt) {
        return;
    }


    Interrupt VirtualMachine::execute(uint32_t inst) {
        uint8_t rd, rs, rt;
        BoltValue rdv, rsv, rtv;
        Opcode op;

        op = decode_op(inst);
        rd = decode_rd(inst);
        rs = decode_rs(inst);
        rt = decode_rt(inst);

        switch(op) {
            case Opcode::OpMul:
                rtv = get_register_value(rt);
                rsv = get_register_value(rs);
                BINARY_OP(*, rd, rtv, rsv);
                break;
            case Opcode::OpDiv:
                rtv = get_register_value(rt);
                rsv = get_register_value(rs);
                BINARY_OP(/, rd, rtv, rsv);
                break;

            case Opcode::OpSub:
                rtv = get_register_value(rt);
                rsv = get_register_value(rs);
                BINARY_OP(-, rd, rtv, rsv);
                break;

                break;
            case Opcode::OpAdd:
                rtv = get_register_value(rt);
                rsv = get_register_value(rs);
                BINARY_OP(+, rd, rtv, rsv);
                break;
            case Opcode::OpMov:
                set_register_value(rd, get_register_value(rt));
                break;

            case Opcode::OpRet:
                ip_ = stack_[fp_ + 1].as_int;
                sp_ = fp_;
                fp_ = stack_[fp_].as_int;
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

