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
    INTEGER, CHARACTER, STRING, POINTER, EMPTY
};

//All data, no matter the type is held as an "Object" in memory
//Objects are simple tagged structs, allowing for easy run time type checking
//and all values are passed by reference.
struct Object {
    RTType type;
    struct _data {
        int intValue;
        char charValue;
    } data;
    Object(char c) {
        data.charValue = c;
        data.intValue = -1;
        type = CHARACTER;
    }
    Object(int val) {
        data.charValue = -1;
        data.intValue = val;
        type = INTEGER;
    }
    Object() {
        data.charValue = -1;
        data.intValue = -1;
        type = EMPTY;
    }
};

class MemStore {
    private:
        int free_list_count;
        int freedList[MAX_MEM_STORE/2];
        Object objmem[MAX_MEM_STORE];
        int nextFreeAddress;
    public:
        MemStore();
        Object& get(int addr);
        void store(int addr, Object o);
        int storeAtNextFree(Object o);
        int allocate(int cells);
        void free(int cell);
};

MemStore::MemStore() {
    free_list_count = 0;
    nextFreeAddress = 0;
}

Object& MemStore::get(int addr) {
    return objmem[addr];
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
    int baseAddr = ++nextFreeAddress;
    for (int m = baseAddr; m < baseAddr + cells; m++) {
        objmem[m] = Object(0);
    }
    nextFreeAddress = baseAddr + cells; 
    return nextFreeAddress;
}

struct StackFrame {
    map<string, int> symbolTable;
    ASTNode* body;
    ASTNode* params;
    StackFrame* staticLink;
    int returnVal;
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