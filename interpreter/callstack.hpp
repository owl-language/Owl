#pragma once
#ifndef callstack_hpp
#define callstack_hpp
#include <iostream>
#include <map>
#include "../tools/tracer.hpp"
#include "../ast/ast.hpp"
#include "memstore.hpp"
using namespace std;


const int MAX_RT_STACK = 150;


struct StackFrame {
    map<string, pair<int,bool>> symbolTable;
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

#endif