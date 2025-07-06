#ifndef BVM_VIRTUAL_MACHINE_H
#define BVM_VIRTUAL_MACHINE_H

#include "bolt_virtual_machine/emitter.h"
#include "lisp/lexer.hpp"
#include <functional>
#include <istream>
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

    enum class Primitives {
        Add,
        Sub,
        Mul,
        Div,
        Lt,
        Lte,
        Bt,
        Bte,
        Ne,
        Eq,
    };

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
        Closure,
        Boolean,
    };

    /*GC objects must be manually managed - use with caution*/
    struct GCObj {
        bool is_marked = false;
        GCObj* next;
    };

    struct Prototype {
        int arity;
        unsigned int n_locals;
        std::vector<BoltValue> consts;
        std::vector<uint32_t> instructions;
        unsigned int next_reg;
    };


    class VirtualMachine;

    struct NativeClosure {
        void (*cfunc)(VirtualMachine*);
    };
    
    struct VirtualClosure {
        const Prototype* proto;
    };

    struct ClosureObj : GCObj {
        enum {
            CLSR_NATIVE,
            CLSR_VIRTUAL,
        } type;
        union {
            NativeClosure as_native;
            VirtualClosure as_virtual;
        };
    };


    struct BoltValue {
        union {
            bool as_bool;
            int as_int;
            double as_double;
            Cons* as_cons;
            char* as_symbol;
            ClosureObj* as_func;
        };
        BoltType type;

        bool operator==(const BoltValue& other) const {
            if (this->type != other.type)
                return false;
            switch(this->type) {
                case BoltType::Integer: return this->as_int == other.as_int;
                case BoltType::Float: return this->as_double == other.as_double;
                case BoltType::Boolean: return this->as_bool == other.as_bool;
                case BoltType::Closure: return this->as_func == other.as_func;
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
            std::vector<std::unique_ptr<Prototype>> callables_;
        public:
            VirtualMachine();
            ~VirtualMachine();
            void load_program(const char* file);
            void load_program(std::vector<uint32_t> program);
            inline void load_callable(std::unique_ptr<Prototype> callable) {
                callables_.push_back(std::move(callable));
            }
            void setup_entry_point();
            inline Prototype* get_callable(size_t id) {
                return callables_.at(id).get();
            }
            inline BoltValue get_stack_entry(size_t entry) {
                return stack_[entry];
            }

            // god help us all if the compiler decides not to inline these
            inline uint32_t fetch() noexcept {
                return stack_[fp_].as_func->as_virtual.proto->instructions[ip_++];
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

            inline void push(BoltValue v) {
                stack_[--sp_] = v;
            }
            inline BoltValue pop() {
                return stack_[sp_++];
            }

            inline void native_add(int n_args) {
                BoltValue res = {.as_int = 0, .type = BoltType::Integer};
                for (int i = 0; i < n_args; i++) {
                    BoltValue r = get_register_value(i);
                    if (res.type != BoltType::Float && r.type == BoltType::Float)
                        res.type = BoltType::Float;
                    res.as_double += r.as_double;
                }
                set_register_value(n_args, res);
            }

            inline void native_sub(int n_args) {
                BoltValue res = {.as_int = 0, .type = BoltType::Integer};
                for (int i = 0; i < n_args; i++) {
                    BoltValue r = get_register_value(i);
                    if (res.type != BoltType::Float && r.type == BoltType::Float)
                        res.type = BoltType::Float;
                    res.as_double -= r.as_double;
                }
                set_register_value(n_args, res);
            }

            inline void native_mul(uint8_t dst, int n_args) {
                BoltValue res = {.as_int = 1, .type = BoltType::Integer};
                for (int i = 0; i < n_args; i++) {
                    BoltValue r = get_register_value(i);
                    if (res.type != BoltType::Float && r.type == BoltType::Float)
                        res.type = BoltType::Float;
                    res.as_double *= r.as_double;
                }
                set_register_value(n_args, res);
            }

            inline void native_div(int n_args) {
                BoltValue res = {.as_int = 1, .type = BoltType::Integer};
                for (int i = 0; i < n_args; i++) {
                    BoltValue r = get_register_value(i);
                    if (res.type != BoltType::Float && r.type == BoltType::Float)
                        res.type = BoltType::Float;
                    res.as_double /= r.as_double;
                }

                set_register_value(n_args, res);
            }

            inline void native_lt(int n_args) {
                BoltValue res = {.as_bool = true, .type = BoltType::Boolean};
                BoltValue prev = get_register_value(0);
                for (int i = 1; i < n_args; i++) {
                    BoltValue r = get_register_value(i);
                    if (prev.as_double >= r.as_double) {
                        res.as_bool = false;
                        break;
                    }
                }
                set_register_value(n_args, res);
            }

            inline void native_lte(int n_args) {
                BoltValue res = {.as_bool = true, .type = BoltType::Boolean};
                BoltValue prev = get_register_value(0);
                for (int i = 1; i < n_args; i++) {
                    BoltValue r = get_register_value(i);
                    if (prev.as_double > r.as_double) {
                        res.as_bool = false;
                        break;
                    }
                }
                set_register_value(n_args, res);
            }

            inline void native_bt(int n_args) {
                BoltValue res = {.as_bool = true, .type = BoltType::Boolean};
                BoltValue prev = get_register_value(0);
                for (int i = 1; i < n_args; i++) {
                    BoltValue r = get_register_value(i);
                    if (prev.as_double <= r.as_double) {
                        res.as_bool = false;
                        break;
                    }
                }
                set_register_value(n_args, res);
            }

            inline void native_bte(int n_args) {
                BoltValue res = {.as_bool = true, .type = BoltType::Boolean};
                BoltValue prev = get_register_value(0);
                for (int i = 1; i < n_args; i++) {
                    BoltValue r = get_register_value(i);
                    if (prev.as_double < r.as_double) {
                        res.as_bool = false;
                        break;
                    }
                }
                set_register_value(n_args, res);
            }


            inline void native_ne(int n_args) {
                BoltValue res = {.as_bool = true, .type = BoltType::Boolean};
                BoltValue prev = get_register_value(0);
                for (int i = 1; i < n_args; i++) {
                    BoltValue r = get_register_value(i);
                    if (prev.as_double == r.as_double) {
                        res.as_bool = false;
                        break;
                    }
                }
                set_register_value(n_args, res);
            }

            inline void native_eq(int n_args) {
                BoltValue res = {.as_bool = true, .type = BoltType::Boolean};
                BoltValue prev = get_register_value(0);
                for (int i = 1; i < n_args; i++) {
                    BoltValue r = get_register_value(i);
                    if (prev.as_double != r.as_double) {
                        res.as_bool = false;
                        break;
                    }
                }
                set_register_value(n_args, res);
            }

    };
}


#endif
