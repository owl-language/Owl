#ifndef memstore_hpp
#define memstore_hpp
#include <iostream>
#include <list>
#include <unordered_map>
#include "memobject.hpp"
#include "../../tools/tracer.hpp"
using std::string;
using std::list;

// 3/6/24
//Both MemStore and Object should probably have a complete overhaul
//this entire set up was a quick hack to get started, and has only
//accrued technical debt since

class MemStore {
    private:
        int MAX_MEM_STORE;
        int free_list_count;
        int *freedList;
        Object *objmem;
        int nextFreeAddress;
        int nextHeapAddress;
        int liveCellCount;
    public:
        MemStore();
        ~MemStore();
        Object& get(int addr);
        void store(int addr, Object& o);
        int storeAtNextFree(Object& o);
        int allocate(int cells);
        int heap_allocate(int cells);
        void free(int cell);
        void display();
        float usage();
        bool isFull();
        void reset();
};

#endif