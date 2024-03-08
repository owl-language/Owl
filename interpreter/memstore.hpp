#ifndef memstore_hpp
#define memstore_hpp
#include <iostream>
using namespace std;

const int MAX_MEM_STORE = 1500; //completely arbitrary

//Run time Type of data stored in Object
enum RTType {
    INTEGER, REAL, STRING, POINTER, EMPTY
};

string rtTypeAsStr[] = { "INTEGER", "REAL", "STRING", "POINTER", "EMPTY"};

//All data, no matter the type is held as an "Object" in memory
//Objects are simple tagged structs, allowing for easy run time type checking
//and all values are passed by reference.
struct Object {
    RTType type;
    struct _data {
        int intValue;
        double realValue;
        string stringValue;
    } data;
    struct _attr {
        int size;
        bool isLive;
    } attr;
    Object(const Object& o) {
        type = o.type;
        data.intValue = o.data.intValue;
        data.realValue = o.data.realValue;
        data.stringValue = o.data.stringValue;
        attr.size = o.attr.size;
        attr.isLive = o.attr.isLive;
    }
    Object(int val) {
        data.intValue = val;
        data.realValue = (float) val;
        data.stringValue = to_string(val);
        attr.size = 1;
        attr.isLive = true;
        type = INTEGER;
    }
    Object(string val) {
        data.stringValue = val;
        attr.size = val.size();
        attr.isLive = true;
        type = STRING;
    }
    Object(float val) {
        data.stringValue = to_string(val);
        data.realValue = val;
        data.intValue = (int) val;
        attr.isLive = true;
        type = REAL;
    }
    Object(double val) {
        data.stringValue = to_string(val);
        data.realValue = val;
        data.intValue = (int) val;
        attr.isLive = true;
        type = REAL;
    }
    Object() {
        data.stringValue = "-1";
        data.realValue = -1.0f;
        data.intValue = -1;
        attr.size = 1;
        attr.isLive = false;
        type = EMPTY;
    }
    Object& operator=(const Object& o) {
        type = o.type;
        data.intValue = o.data.intValue;
        data.realValue = o.data.realValue;
        data.stringValue = o.data.stringValue;
        attr.size = o.attr.size;
        attr.isLive = o.attr.isLive;
        return *this;
    }
    string toString() {
        if (type == INTEGER)
            return to_string(data.intValue);
        if (type == REAL)
            return to_string(data.realValue);
        return data.stringValue;
    }
};

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
};

MemStore::MemStore() {
    free_list_count = 0;
    nextFreeAddress = 0;
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
    if (addr >= 0 && addr < MAX_MEM_STORE)
        return objmem[addr];
    cout<<"[INVALID ADDRESS: "<<addr<<"]"<<endl;
    return objmem[0];
}

void MemStore::store(int addr, Object o) {
    objmem[addr] = o;
}

int MemStore::storeAtNextFree(Object o) {
    if (nextFreeAddress+1 >= MAX_MEM_STORE) {
        cout<<"ERROR: OUT OF MEMORY"<<endl;
        return -1;
    }
    int addr = allocate(1);
    store(addr, o);
    return nextFreeAddress;
}

void MemStore::free(int cell) {
    objmem[cell].type = EMPTY;
    objmem[cell].data.intValue = -1;
    freedList[free_list_count++] = cell;
    liveCellCount--;
}

int MemStore::allocate(int cells) {
    if (nextFreeAddress+cells >= MAX_MEM_STORE) {
        cout<<"ERROR: OUT OF MEMORY"<<endl;
        return -1;
    }
    int m, baseAddr;
    if (cells == 1) {
        if (free_list_count > 0) {
            baseAddr = freedList[free_list_count-1];
            free_list_count--;
        } else {
            baseAddr = ++nextFreeAddress;
        }
        objmem[baseAddr].type = INTEGER;
        objmem[baseAddr].data.intValue = 0;
        liveCellCount++;
    } else {
        baseAddr = ++nextFreeAddress;
        for (; nextFreeAddress < baseAddr + cells; nextFreeAddress++) {
            objmem[nextFreeAddress].type = INTEGER;
            objmem[nextFreeAddress].data.intValue = 0;
            liveCellCount++;
        }
    }
    return baseAddr;
}

#endif