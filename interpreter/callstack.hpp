#ifndef callstack_hpp
#define callstack_hpp
#include <iostream>
#include <map>
#include "../ast/ast.hpp"
#include "memstore.hpp"
using namespace std;


const int MAX_RT_STACK = 150;


struct StackFrame {
    map<string, int> symbolTable;
    ASTNode* body;
    ASTNode* params;
    StackFrame* staticLink;
    Object returnVal;
};

class CallStack {
    private:
        StackFrame* rtStack[MAX_RT_STACK];
        int rtsp;
    public:
        CallStack();
        ~CallStack();
        int size();
        bool empty();
        void push(StackFrame* sf);
        void pop();
        StackFrame* top();
        void clear();
};

CallStack::CallStack() {
    rtsp = 0;
}

CallStack::~CallStack() {
    clear();
}

int CallStack::size() {
    return rtsp;
}

bool CallStack::empty() {
    return rtsp == 0;
}

void CallStack::push(StackFrame* sf) {
    if (rtsp+1 == MAX_RT_STACK) {
        cout<<"[ERROR: STACK OVERFLOW!]"<<endl;
        return;
    }
    if (rtsp == 0) sf->staticLink = nullptr;
    else sf->staticLink = rtStack[rtsp];
    rtStack[++rtsp] = sf;
}

void CallStack::pop() {
    if (rtsp - 1 < 0) {
        cout<<"[ERROR: STACK UNDERFLOW!]"<<endl;
        return;
    }
    StackFrame* t = rtStack[rtsp];
    --rtsp;
    delete t;
}
StackFrame* CallStack::top() {
    return rtStack[rtsp];
}

void CallStack::clear() {
    while (!empty())
        pop();
}

#endif