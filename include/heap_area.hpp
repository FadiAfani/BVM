#ifndef BVM_HEAP_AREA_H
#define BVM_HEAP_AREA_H 

#include <cstddef>
#include <cstdint>
namespace BVM {

    struct Chunk {
        uint8_t* offset;
        size_t len;
        bool is_free;
    };

    class HeapArea {
        private:
            uint8_t* mem;
            size_t capacity;

        public:
            HeapArea();
            ~HeapArea();
            void allocate(size_t block_size);
            void write();
            template<typename T>
            T read_as();


    };
}

#endif
