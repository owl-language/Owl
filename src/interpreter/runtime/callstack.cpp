#include "callstack.hpp"

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
        logError("[ERROR: STACK OVERFLOW!]");
        return;
    }
    if (rtsp == 0) sf->staticLink = nullptr;
    else sf->staticLink = rtStack[rtsp];
    rtStack[++rtsp] = sf;
}

void CallStack::pop() {
    if (rtsp - 1 < 0) {
        logError("[ERROR: STACK UNDERFLOW!]");
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
