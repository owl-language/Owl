#ifndef interpreter_hpp
#define interpreter_hpp
#include <iostream>
#include <map>
#include "ast_builder.hpp"
#include "../ast/ast.hpp"
#include "../tokens/tokens.hpp"
#include "../tools/tracer.hpp"
#include "runtime/callstack.hpp"
#include "runtime/memstore.hpp"
#include "runtime/memobject.hpp"
using std::cout;
using std::endl;
using std::map;


class Interpreter {
    private:
        map<string, int> variables;
        map<string, StackFrame*> procedures;
        CallStack callStack;
        MemStore memStore;
        Object eval(ASTNode* x);
        Object stringOp(TokenType op, Object left, Object right);
        Object mathOp(TokenType op, Object left, Object right);
        Object relOp(TokenType op, float left, float right);
        void addProcedureToSymbolTable(map<string, StackFrame*>& procdefs, ASTNode* node);
        void declareVariable(ASTNode* x);
        void declareFunction(ASTNode* x);
        void declareStruct(ASTNode* x);
        Object retrieveFromMemoryByName(ASTNode* x);
        void storeToMemoryByName(ASTNode* x);
        Object interpretExpression(ASTNode* x);
        void interpretStatement(ASTNode* x);
        void interpretExprStatement(ASTNode* x);
        Object doProcedureCall(ASTNode* x);
        void doReturnStatement(ASTNode* x);
        void doPrintStatement(ASTNode* x);
        void doReadStatement(ASTNode* x);
        void handleIfStatement(ASTNode* x);
        void handleWhileStatement(ASTNode* x);
        void compilerDirective(ASTNode* x);
        void assignLocalSymbols(ASTNode* node, StackFrame* nsf);
        StackFrame* prepStackFrame(StackFrame* x);
        Object Dispatch(ASTNode* x, StackFrame* nsf);
        void importLibrary(string libName);
    public:
        void Execute(ASTNode* x);
        void memoryUsage() {
            cout<<"[ Memory Usage: "<<memStore.usage()<<"% ]"<<endl;
            memStore.display();
        }
        void reset() {
            procedures.clear();
            variables.clear();
            callStack.clear();
            memStore.reset();
        }

};

#endif