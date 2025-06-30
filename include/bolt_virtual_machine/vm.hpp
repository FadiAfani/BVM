#ifndef BVM_VIRTUAL_MACHINE_H
#define BVM_VIRTUAL_MACHINE_H

#include "bolt_virtual_machine/emitter.h"
#include <functional>
#include <string>
#define STACK_SIZE (1 << 12)
#define METADATA_SIZE 3

/** Stack Layout (top to bottom)
 * old_fp
 * ret_addr
 * callable_ref
 * locals
 */


#include <cstdint>
#include <vector>
#include "instruction.hpp"
#include <memory>

namespace BVM {

    enum class Interrupt {
        StackOverFlow,
        StackUnderFlow,
        DivisionByZero,
        IncompatibleTypes,
        Ok
    };

    struct BoltValue;

    using Cons = std::pair<BoltValue, BoltValue>;

    enum class BoltType {
        Integer,
        Float,
        Cons,
        Symbol,
        Nil,
        Function,
        Boolean,

    };

    enum class ObjType {
        List,
        Function,
    };

    struct Obj {
        ObjType type;
    };

    struct FuncObj : Obj {
        int arity;
        unsigned int n_locals;
        std::vector<BoltValue> consts;
        std::vector<uint32_t> instructions;
        unsigned int next_reg;
    };

    struct ListObj : Obj {
        std::vector<int> erefs;

    };




    struct BoltValue {
        union {
            bool as_bool;
            int64_t as_int;
            double as_double;
            Cons* as_cons;
            char* as_symbol;
            FuncObj* as_func;
        };
        BoltType type;

        bool operator==(const BoltValue& other) const {
            if (this->type != other.type)
                return false;
            switch(this->type) {
                case BoltType::Integer: return this->as_int == other.as_int;
                case BoltType::Float: return this->as_double == other.as_double;
                case BoltType::Boolean: return this->as_bool == other.as_bool;
                case BoltType::Function: return this->as_func == other.as_func;
                default: throw std::runtime_error("BoltValue: Comparison Not Implemented");
            }

        }
    };

    class VirtualMachine {
        private:
            BoltValue stack_[STACK_SIZE];
            size_t ip_ = 0;
            int16_t sp_ = STACK_SIZE;
            int16_t fp_ = STACK_SIZE;
            std::vector<std::unique_ptr<FuncObj>> callables_;
        public:
            VirtualMachine();
            ~VirtualMachine();
            void load_program(std::vector<uint32_t> program);
            inline void load_callable(std::unique_ptr<FuncObj> callable) {
                callables_.push_back(std::move(callable));
            }
            void setup_entry_point();
            inline FuncObj* get_callable(size_t id) {
                return callables_.at(id).get();
            }
            inline BoltValue get_stack_entry(size_t entry) {
                return stack_[entry];
            }

            // god help us all if the compiler decides not to inline these
            inline uint32_t fetch() noexcept {
                return stack_[fp_].as_func->instructions[ip_++];
            }

            static inline Opcode decode_op(uint32_t inst) noexcept {
                return static_cast<Opcode>(static_cast<uint8_t>(inst));
            }

            static inline uint8_t decode_rd(uint32_t inst) noexcept {
                return (uint8_t) (inst >> 8);
            }

            static inline uint8_t decode_rt(uint32_t inst) noexcept {
                return (uint8_t) (inst >> 16);
            }

            static inline uint8_t decode_rs(uint32_t inst) noexcept {
                return (uint8_t) (inst >> 24);
            }

            inline BoltValue get_register_value(uint8_t r) noexcept { return stack_[fp_ - METADATA_SIZE - r]; }

            inline void set_register_value(uint8_t r, BoltValue value) noexcept {
                stack_[fp_ - METADATA_SIZE - r] = value;
            }

            Interrupt execute(uint32_t inst);
            void run();
            void handle_interrupt(Interrupt interrupt);
            inline void push(BoltValue value) {
                stack_[--sp_] = value;
            }
            inline BoltValue pop() {
                return stack_[sp_++];
            }
                    
    };
}


#endif
