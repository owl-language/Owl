#include "memstore.hpp"
MemStore::MemStore() {
    reset();
}

void MemStore::reset() {
    liveCellCount = 0;
    nextFreeAddress = 0;
    free_list_count = 0;
    for (int i = 0; i < MAX_MEM_STORE/2; i++)
        freedList[i] = 0;
    for (int i = 0; i < MAX_MEM_STORE; i++) {
        objmem[i] = Object();
    }
}

float MemStore::usage() {
    float pct = (float)liveCellCount/(float)MAX_MEM_STORE;
    return pct;
}

bool MemStore::isFull() {
    return nextFreeAddress == MAX_MEM_STORE && free_list_count == 0;
}

void MemStore::display() {
    for (int i = 0; i <= nextFreeAddress; i++) {
        if (objmem[i].attr.isLive)
            cout<<"["<<i<<": "<<objmem[i].toString()<<"] ";
    }
    cout<<endl;
}

Object& MemStore::get(int addr) {
    if (addr >= 1 && addr < MAX_MEM_STORE)
        return objmem[addr];
    logError("[INVALID ADDRESS: " + to_string(addr) + "]");
    return objmem[0];
}

void MemStore::store(int addr, Object toStore) {
    if (addr < 1 && addr > MAX_MEM_STORE) {
        logError("Invalid Memory Address provided: " + to_string(addr));
        return;
    }
    objmem[addr].data._value = toStore.data._value;
    objmem[addr].type = toStore.type;
}

int MemStore::storeAtNextFree(Object o) {
    if (nextFreeAddress+1 >= MAX_MEM_STORE) {
        logError("ERROR: OUT OF MEMORY");
        return -1;
    }
    int addr = allocate(1);
    store(addr, o);
    return nextFreeAddress;
}

void MemStore::free(int cell) {
    objmem[cell].type = EMPTY;
    objmem[cell].data._value = "0";
    freedList[free_list_count++] = cell;
    liveCellCount--;
}

int MemStore::allocate(int cells) {
    if (nextFreeAddress+cells >= MAX_MEM_STORE) {
        logError("ERROR: OUT OF MEMORY");
        return -1;
    }
    int baseAddr;
    //if we only need a single cell, chances are
    //we have at some point freed a block of that size
    //so check freed list first.
    if (cells == 1) {
        if (free_list_count > 0) {
            baseAddr = freedList[free_list_count-1];
            free_list_count--;
        } else {
            baseAddr = ++nextFreeAddress;
        }
        objmem[baseAddr].attr.isLive = true;
        liveCellCount++;
    } else {
        baseAddr = ++nextFreeAddress;
        for (; nextFreeAddress < baseAddr + cells; nextFreeAddress++) {
            objmem[nextFreeAddress].attr.isLive = true;
            objmem[nextFreeAddress].attr.size = cells;
            objmem[nextFreeAddress].type = INTEGER;
            liveCellCount++;
        }
    }
    return baseAddr;
}
