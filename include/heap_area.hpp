#ifndef BVM_HEAP_AREA_H
#define BVM_HEAP_AREA_H 

#include <cstddef>
#include <cstdint>
#include <list>
namespace BVM {

    struct Chunk {
        size_t offset;
        size_t len;
        bool is_free;
    };

    class HeapArea {
        private:
            uint8_t* mem;
            size_t capacity;
            std::list<Chunk> free_list;

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
