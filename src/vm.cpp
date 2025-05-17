#include "vm.hpp"

namespace BVM {

    VirtualMachine::VirtualMachine() {}
    VirtualMachine::~VirtualMachine() {}

    Interrupt VirtualMachine::execute(const std::vector<uint32_t>& program) {

        for (;;) {

        }

        return Interrupt::Ok;
    }
}
