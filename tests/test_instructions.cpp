#include "bolt_virtual_machine/vm.hpp"
#include "bolt_virtual_machine/emitter.h"
#include <gtest/gtest.h>

BVM::VirtualMachine vm;
const uint8_t rd = 0, rt = 1, rs = 2;

#include <cstdint>

class InstructionTests: public ::testing::Test {
    protected:
        void SetUp() override {
            std::unique_ptr<BVM::Callable> main_func = std::make_unique<BVM::Callable>();
            main_func->name = "main";
            main_func->n_locals = 3;
            vm.load_callable(std::move(main_func));
            vm.setup_entry_point();
        }

};


TEST_F(InstructionTests, TestSetup) {
    EXPECT_EQ(vm.get_stack_entry(STACK_SIZE - 1), -1);
    EXPECT_EQ(vm.get_stack_entry(STACK_SIZE - 2), 0);
    uint64_t main_func = std::bit_cast<uint64_t>(vm.get_callable(0));
    uint64_t ref = vm.get_stack_entry(STACK_SIZE - 3);
    EXPECT_EQ(ref, main_func);
}

TEST_F(InstructionTests, TestAdd) {
    vm.set_register_value(rt, 1);
    vm.set_register_value(rs, 1);
    BVM::Interrupt interrupt = vm.execute(BVM::Emitter::add(rd, rt, rs));
    EXPECT_EQ(interrupt, BVM::Interrupt::Ok);
    EXPECT_EQ(vm.get_register_value(rd), 2);
}

TEST_F(InstructionTests, TestFadd) {
    vm.set_register_value(rt, std::bit_cast<uint64_t>(2.5));
    vm.set_register_value(rs, std::bit_cast<uint64_t>(2.6));
    BVM::Interrupt interrupt = vm.execute(BVM::Emitter::fadd(rd, rt, rs));
    EXPECT_EQ(interrupt, BVM::Interrupt::Ok);
    EXPECT_EQ(vm.get_register_value(rd), std::bit_cast<uint64_t>(2.5 + 2.6));
}

TEST_F(InstructionTests, TestMov) {
    vm.set_register_value(rt, 10);
    BVM::Interrupt interrupt = vm.execute(BVM::Emitter::mov(rd, rt));
    EXPECT_EQ(interrupt, BVM::Interrupt::Ok);
    EXPECT_EQ(vm.get_register_value(rd), 10);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
