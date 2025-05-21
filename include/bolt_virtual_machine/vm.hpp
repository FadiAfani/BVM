#ifndef BVM_VIRTUAL_MACHINE_H
#define BVM_VIRTUAL_MACHINE_H

#include <string>
#define STACK_SIZE (1 << 12)

#include <cstdint>
#include <vector>
#include "instruction.hpp"

namespace BVM {

    enum class Interrupt {
        StackOverFlow,
        StackUnderFlow,
        DivisionByZero,
        Ok
    };

    // not currently used
    union BoltValue {
        uint8_t as_u8;
        uint16_t as_u16;
        uint32_t as_u32;
        uint64_t as_u64;
        int8_t as_i8;
        int16_t as_i16;
        int32_t as_i32;
        int64_t as_i64;
        float as_f32;
        double as_f64;
    };

    struct Callable {
        int n_locals;
        std::string name;
        std::vector<uint32_t> code;
    };


    class VirtualMachine {
        private:
            // maybe a union is a bit cleaner compared to a u64..
            uint64_t stack_[STACK_SIZE];
            size_t ip_ = 0;
            int16_t sp_ = STACK_SIZE;
            int16_t fp_ = STACK_SIZE;
            std::vector<Callable> callables_;
        public:
            VirtualMachine();
            ~VirtualMachine();
            void load_program(std::vector<uint32_t> program);

            // god help us all if the compiler decides not to inline these

            inline uint32_t fetch() noexcept {
                return reinterpret_cast<Callable*>(stack_[fp_])->code[ip_++];
            }

            inline Opcode decode_op(uint32_t inst) noexcept {
                return static_cast<Opcode>(inst);
            }

            inline uint8_t decode_rd(uint32_t inst) noexcept {
                return (uint8_t) (inst >> 8);
            }

            inline uint8_t decode_rt(uint32_t inst) noexcept {
                return (uint8_t) (inst >> 16);
            }

            inline uint8_t decode_rs(uint32_t inst) noexcept {
                return (uint8_t) (inst >> 24);
            }

            inline uint64_t get_register_value(uint8_t r) noexcept { return stack_[fp_ - 3 - r]; }

            inline void set_register_value(uint8_t r, uint64_t value) noexcept {
                stack_[fp_ - 3 - r] = value;
            }

            Interrupt execute(uint32_t inst);
            void run();
            void handle_interrupt(Interrupt interrupt);

    };
}

#endif
