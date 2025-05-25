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
    EXPECT_EQ(vm.get_stack_entry(STACK_SIZE - 1).as_int, -1);
    EXPECT_EQ(vm.get_stack_entry(STACK_SIZE - 2).as_int, 0);
    BVM::Callable* main_func = vm.get_callable(0);
    BVM::BoltValue ref = vm.get_stack_entry(STACK_SIZE - 3);
    EXPECT_EQ(ref.as_func, main_func);
}

TEST_F(InstructionTests, TestAdd) {
    vm.set_register_value(rt, {.as_int = 1, .type = BVM::BoltType::Integer});
    vm.set_register_value(rs, {.as_int = 1, .type = BVM::BoltType::Integer});
    BVM::Interrupt interrupt = vm.execute(BVM::Emitter::add(rd, rt, rs));
    EXPECT_EQ(interrupt, BVM::Interrupt::Ok);
    EXPECT_EQ(vm.get_register_value(rd).as_int, 2);
}

TEST_F(InstructionTests, TestFadd) {
    vm.set_register_value(rt, {.as_double = 2.5, .type = BVM::BoltType::Float});
    vm.set_register_value(rs, {.as_double = 2.6, .type = BVM::BoltType::Float});
    BVM::Interrupt interrupt = vm.execute(BVM::Emitter::add(rd, rt, rs));
    EXPECT_EQ(interrupt, BVM::Interrupt::Ok);
    EXPECT_EQ(vm.get_register_value(rd).as_double, 5.1);
}

TEST_F(InstructionTests, TestMov) {
    vm.set_register_value(rt, {.as_int = 10, .type = BVM::BoltType::Integer});
    BVM::Interrupt interrupt = vm.execute(BVM::Emitter::mov(rd, rt));
    EXPECT_EQ(interrupt, BVM::Interrupt::Ok);
    EXPECT_EQ(vm.get_register_value(rd).as_int, 10);
}

