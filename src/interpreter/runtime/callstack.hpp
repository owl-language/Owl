#pragma once
#ifndef callstack_hpp
#define callstack_hpp
#include <iostream>
#include <map>
#include "../../tools/tracer.hpp"
#include "../../ast/ast.hpp"
#include "memstore.hpp"
using std::cout;
using std::endl;
using std::map;
using std::string;

inline const int MAX_RT_STACK = 150;

struct stEntry {
    int addr;
    bool isRef;
    stEntry(int ad = 0, bool ref = false) {
        addr = ad;
        isRef = ref;
    }
};

struct StackFrame {
    map<string, stEntry> symbolTable;
    map<string, StackFrame*> nestedProcedures;
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