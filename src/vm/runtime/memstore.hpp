#ifndef memstore_hpp
#define memstore_hpp
#include <iostream>
#include <list>
#include <unordered_map>
#include "memobject.hpp"
#include "../../tools/tracer.hpp"
using std::string;
using std::list;

const int MAX_MEM_STORE = 3500; //completely arbitrary
class MemStore {
    private:
        int free_list_count;
        int freedList[MAX_MEM_STORE/2];
        Object objmem[MAX_MEM_STORE];
        int nextFreeAddress;
        int liveCellCount;
    public:
        MemStore();
        Object& get(int addr);
        void store(int addr, Object o);
        int storeAtNextFree(Object o);
        int allocate(int cells);
        void free(int cell);
        void display();
        float usage();
        bool isFull();
        void reset();
};

#endif