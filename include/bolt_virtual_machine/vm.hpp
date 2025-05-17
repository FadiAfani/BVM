#ifndef BVM_VIRTUAL_MACHINE_H
#define BVM_VIRTUAL_MACHINE_H

#define STACK_SIZE 4096

#include <cstdint>
#include <vector>


namespace BVM {

    enum class Interrupt {
        StackOverFlow,
        StackUnderFlow,
        DivisionByZero,
        Ok
    };

    union Word {
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

    class VirtualMachine {
        private:
            Word stack[STACK_SIZE];
        public:
            VirtualMachine();
            ~VirtualMachine();
            Interrupt execute(const std::vector<uint32_t>& program);
    };
}

#endif
