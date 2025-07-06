#include "bolt_virtual_machine/vm.hpp"
#include "bolt_virtual_machine/instruction.hpp"
#include <cstdint>
#include <fstream>
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


    /* Bolt File Layout 
     * n_funcs
     * [Prototypes]
     * */

    /* Protoype Structure
     * n_locals
     * n_consts
     * [constants]
     * n_insts
     * [instructions]
     * */

    void VirtualMachine::load_program(const char* file) {
        std::ifstream in(file, std::ios::binary);
        int n;
        in.read(reinterpret_cast<char*>(&n), 4);

    }

    void VirtualMachine::setup_entry_point() {
        Prototype* main = callables_.at(0).get();
        ClosureObj main_clsr = {.type = ClosureObj::CLSR_VIRTUAL, .as_virtual = {.proto = main} };
        BoltValue prev_fp = {.as_int = -1, .type = BoltType::Integer };
        BoltValue ret_addr = {.as_int = 0, .type = BoltType::Integer };
        BoltValue func = {.as_func = &main_clsr, .type = BoltType::Closure};
        push(prev_fp);
        push(ret_addr);
        push(func);
        fp_ = STACK_SIZE - 1;
        sp_ -= main->next_reg; // callable_ref + prev_fp + return addr

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
            case Opcode::OpMov:
                set_register_value(rd, get_register_value(rt));
                break;

            case Opcode::OpRet:
                ip_ = stack_[fp_ + 1].as_int;
                sp_ = fp_;
                fp_ = stack_[fp_].as_int;
                stack_[sp_] = get_register_value(rd);
                break;

            case Opcode::OpCall:
            {
                BoltValue f = get_register_value(rd);
                uint16_t n_args = static_cast<uint16_t>(inst); // get lowest 16 bits
                if (f.as_func->type == ClosureObj::CLSR_VIRTUAL) {
                    push({.as_int = fp_, .type = BoltType::Integer});
                    push({.as_int = static_cast<int>(ip_), .type = BoltType::Integer});
                    push({.as_func = f.as_func, .type = BoltType::Closure});
                    fp_ = sp_ + 3;
                    sp_ -= n_args;
                    ip_ = 0;
                } else {
                    push({.as_int = fp_, .type = BoltType::Integer});
                    push({.as_int = static_cast<int>(ip_), .type = BoltType::Integer});
                    fp_ = sp_ + 2;
                    sp_ -= n_args;
                }
                break;
            }
            default:
                throw std::runtime_error("opcode not implemented or recognized");

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

