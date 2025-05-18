#ifndef BVM_VIRTUAL_MACHINE_H
#define BVM_VIRTUAL_MACHINE_H

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



    class VirtualMachine {
        private:
            uint64_t stack_[STACK_SIZE];
            size_t ip_ = 0;
            int16_t sp_ = -1;
            int16_t lp_ = -1;
            int16_t fp_ = -1;
            const std::vector<uint32_t>* program_;
        public:
            VirtualMachine();
            ~VirtualMachine();
            void load_program(const std::vector<uint32_t>* program);
            uint32_t fetch() noexcept;
            Opcode decode_op(uint32_t inst) noexcept;
            uint8_t decode_rd(uint32_t inst) noexcept;
            uint8_t decode_rt(uint32_t inst) noexcept;
            uint8_t decode_rs(uint32_t inst) noexcept;
            inline uint64_t get_register_value(uint8_t r) noexcept;
            inline void set_register_value(uint8_t r, uint64_t value) noexcept;
            Interrupt execute();

    };
}

#endif
