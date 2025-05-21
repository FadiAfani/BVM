#include "bolt_virtual_machine/vm.hpp"
#include "bolt_virtual_machine/emitter.h"
#include <gtest/gtest.h>

BVM::VirtualMachine vm;
const uint8_t rd = 0, rt = 0, rs = 0;

#include <cstdint>

TEST(InstructionTests, TestAdd) {
    std::vector<uint32_t> program = {
        BVM::Emitter::add(rd, rt, rs)
    };
    vm.load_program(program);
    vm.set_register_value(rt, 1);
    vm.set_register_value(rs, 1);
    BVM::Interrupt interrupt = vm.execute(BVM::Emitter::add(rd, rt, rs));
    EXPECT_EQ(interrupt, BVM::Interrupt::Ok);
    EXPECT_EQ(vm.get_register_value(rd), 2);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
