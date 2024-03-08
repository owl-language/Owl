#ifndef runtimestack_hpp
#define runtimestack_hpp
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