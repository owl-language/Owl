#ifndef runtimestack_hpp
#define runtimestack_hpp
#include <iostream>
#include <map>
#include "../ast/ast.hpp"
using namespace std;


const int MAX_MEM_STORE = 1500; //these values are completely arbitrary
const int MAX_RT_STACK = 150;

//Run time Type of data stored in Object
enum RTType {
    INTEGER, CHARACTER, STRING, POINTER, EMPTY, REAL
};

string rtTypeAsStr[] = { "INTEGER", "CHARACTER", "STRING", "POINTER", "EMPTY", "REAL"};

//All data, no matter the type is held as an "Object" in memory
//Objects are simple tagged structs, allowing for easy run time type checking
//and all values are passed by reference.
struct Object {
    RTType type;
    struct _data {
        int intValue;
        char charValue;
        double realValue;
        string stringValue;
    } data;
    struct _attr {
        int size;
    } attr;
    Object(const Object& o) {
        type = o.type;
        data.intValue = o.data.intValue;
        data.charValue = o.data.charValue;
        data.realValue = o.data.realValue;
        data.stringValue = o.data.stringValue;
        attr.size = o.attr.size;
    }
    Object(char c) {
        data.charValue = c;
        data.intValue = -1;
        attr.size = 1;
        type = CHARACTER;
    }
    Object(int val) {
        data.charValue = -1;
        data.intValue = val;
        data.realValue = (float) val;
        attr.size = 1;
        type = INTEGER;
    }
    Object(string val) {
        data.stringValue = val;
        attr.size = val.size();
        type = STRING;
    }
    Object(float val) {
        data.realValue = val;
        data.charValue = -1;
        data.intValue = (int) val;
        type = REAL;
    }
    Object(double val) {
        data.realValue = val;
        data.charValue = -1;
        data.intValue = (int) val;
        type = REAL;
    }
    Object() {
        data.charValue = -1;
        data.intValue = -1;
        attr.size = 1;
        type = EMPTY;
    }
    Object& operator=(const Object& o) {
        type = o.type;
        data.intValue = o.data.intValue;
        data.charValue = o.data.charValue;
        data.realValue = o.data.realValue;
        data.stringValue = o.data.stringValue;
        attr.size = o.attr.size;
        return *this;
    }
    string toString() {
        if (type == STRING)
            return data.stringValue;
        if (type == REAL)
            return to_string(data.realValue);
        return to_string(data.intValue);
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
        int usage();
        bool isFull();
};

MemStore::MemStore() {
    free_list_count = 0;
    nextFreeAddress = 0;
}

int MemStore::usage() {
    float pct = (float)liveCellCount/(float)MAX_MEM_STORE;
    return pct * 100;
}

bool MemStore::isFull() {
    return nextFreeAddress == MAX_MEM_STORE && free_list_count == 0;
}

void MemStore::display() {
   /* for (int i = 0; i <= nextFreeAddress; i++) {
        cout<<"["<<i<<": "<<objmem[i].toString()<<"] ";
    }
    cout<<endl;*/
}

Object& MemStore::get(int addr) {
    if (addr >= 0 && addr < MAX_MEM_STORE)
        return objmem[addr];
    cout<<"[INVALID ADDRESS: "<<addr<<"]"<<endl;
    return objmem[MAX_MEM_STORE - 1];
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
    } else {
        baseAddr = ++nextFreeAddress;
        for (; nextFreeAddress < baseAddr + cells; nextFreeAddress++) {
            objmem[nextFreeAddress].type = INTEGER;
            objmem[nextFreeAddress].data.intValue = 0;
        }
    }
    return baseAddr;
}

struct StackFrame {
    map<string, int> symbolTable;
    ASTNode* body;
    ASTNode* params;
    StackFrame* staticLink;
    Object returnVal;
};

class RuntimeStack {
    private:
        StackFrame* rtStack[MAX_RT_STACK];
        int rtsp;
    public:
        RuntimeStack();
        int size();
        bool empty();
        void push(StackFrame* sf);
        void pop();
        StackFrame* top();
};

RuntimeStack::RuntimeStack() {
    rtsp = 0;
}
int RuntimeStack::size() {
    return rtsp;
}

bool RuntimeStack::empty() {
    return rtsp == 0;
}

void RuntimeStack::push(StackFrame* sf) {
    if (rtsp+1 == MAX_RT_STACK) {
        cout<<"[ERROR: STACK OVERFLOW!]"<<endl;
        return;
    }
    if (rtsp == 0) sf->staticLink = nullptr;
    else sf->staticLink = rtStack[rtsp];
    rtStack[++rtsp] = sf;
}

void RuntimeStack::pop() {
    if (rtsp - 1 < 0) {
        cout<<"[ERROR: STACK UNDERFLOW!]"<<endl;
        return;
    }
    StackFrame* t = rtStack[rtsp];
    --rtsp;
    delete t;
}
StackFrame* RuntimeStack::top() {
    return rtStack[rtsp];
}

#endif