#ifndef interpreter_hpp
#define interpreter_hpp
#include <iostream>
#include <map>
#include "../ast/ast.hpp"
#include "../tokens/tokens.hpp"
#include "../tools/tracer.hpp"
#include "runtime.hpp"
using namespace std;

class Interpreter {
    private:
        map<string, int> variables;
        map<string, StackFrame*> procedures;
        RuntimeStack rtStack;
        MemStore memStore;
        int rtsp;
        int eval(ASTNode* x);
        int interpretExpression(ASTNode* x);
        int handleIDEXPR(ASTNode* x);
        void interpretReturnStatement(ASTNode* x);
        void interpretVarDeclaration(ASTNode* x);
        void interpretFuncDeclaration(ASTNode* x);
        void interpretPrintStatement(ASTNode* x);
        void interpretReadStatement(ASTNode* x);
        void interpretIfStatement(ASTNode* x);
        void interpretStatement(ASTNode* x);
        void interpretWhileStatement(ASTNode* x);
        void interpretExprStatement(ASTNode* x);
        void interpretAssignment(ASTNode* x);
        StackFrame* prepStackFrame(StackFrame* x);
        int Dispatch(ASTNode* x);
    public:
        void Execute(ASTNode* x);

};

int divide(int a, int b) {
    if (b == 0) {
        cout<<"Don't do that."<<endl;
        return 0;
    }
    float fa = (float) a;
    float fb = (float) b;
    float t = fa/fb;
    return (int)t;
}

int Interpreter::eval(ASTNode* x) {
    onEnter("eval: " + tokenString[x->attribute.intValue]);
    int leftOperand, rightOperand, result = 0;
    leftOperand = interpretExpression(x->child[0]);
    rightOperand = interpretExpression(x->child[1]);
    switch (x->attribute.op) {
        case PLUS:
            result = leftOperand + rightOperand;
            break;
        case MINUS:
            result = leftOperand - rightOperand;
            break;
        case MULT:
            result = leftOperand * rightOperand;
            break;
        case DIVD:
            result = divide(leftOperand, rightOperand);
            break;
        case LESS:
            result = (leftOperand < rightOperand);
            break;
        case GREATER:
            result = (leftOperand > rightOperand);
            break;
        case EQUAL:
            result = (leftOperand == rightOperand);
            break;
        case NOTEQUAL:
            result = (leftOperand != rightOperand);
            break;
        case LESSEQ:
            result = (leftOperand <= rightOperand);
            break;
        case GREATEREQ:
            result = (leftOperand >= rightOperand);
            break;
    }
    onExit("eval" + x->attribute.name);
    return result;
}

int Interpreter::handleIDEXPR(ASTNode* x) {
    onEnter("handleIDExpr");
    int retVal = 0;
    int offset = 0;
    if (procedures.find(x->attribute.name) != procedures.end()) {
        say("Procedure Found, Dispatching.");
        retVal = Dispatch(x);
        say("return from " + x->attribute.name + " value: " + to_string(retVal));
        onExit(" ");
        return retVal;
    }
    if (x->kind == EXPRNODE && x->type.expr == SUBSCRIPT_EXPR) {
        say("Calculating Offset.");
        offset = interpretExpression(x->child[0]);
        if (offset < 0) {
            cout<<"Error: Invalid Index: "<<offset<<endl;
            return -1;
        }
        say("Array Reference, offset: " + to_string(offset));
    }
    if (rtStack.size()) {
        if (rtStack.top()->symbolTable.find(x->attribute.name) != rtStack.top()->symbolTable.end()) {
            int addr = rtStack.top()->symbolTable[x->attribute.name];
            if (offset > 0) {
                if (offset > memStore.get(addr).attr.size) {
                    cout<<"Error: Index "<<offset<<" out of range for array "<<x->attribute.name<<endl;
                    return -1;
                }
            }
            retVal = memStore.get(addr + offset).data.intValue;
            say("ID: " + x->attribute.name + ", Address: " + to_string(addr) + ", offset: " +to_string(offset) + ", value: " + to_string(retVal));
            onExit(" ");
            return retVal;
        }
    }
    if (variables.find(x->attribute.name) != variables.end()) {
        int addr = variables[x->attribute.name];
        if (offset > 0) {
            if (offset > memStore.get(addr).attr.size) {
                cout<<"Error: Index "<<offset<<" out of range for array "<<x->attribute.name<<endl;
                return -1;
            }
        }
        retVal = memStore.get(addr + offset).data.intValue;
        say("ID: " + x->attribute.name + ", Address: " + to_string(addr) + ", offset: " +to_string(offset) + ", value: " + to_string(retVal));
        onExit(" ");
        return retVal;
    }
    cout<<"Uh oh, couldnt find: "<<x->attribute.name<<endl;
    onExit(" ");
    return retVal;
}

void Interpreter::interpretAssignment(ASTNode* x) {
    onEnter("AssignStatement ");
    string varname = x->child[0]->attribute.name; //variable name
    say("Assign to " + varname + ": ");
    int ret = interpretExpression(x->child[1]);   //value to assign
    int offset = 0;
    if (x->child[0]->kind == EXPRNODE && x->child[0]->type.expr == SUBSCRIPT_EXPR) {
        say("Retrieving subscript " + x->child[0]->attribute.name);
        offset = interpretExpression(x->child[0]->child[0]);
        if (offset < 0) {
            cout<<"Error: Invalid Index: "<<offset<<endl;
        }
        say("Array Reference, offset: " + offset);
    }
    if (rtStack.size()) { //check procedure symbol table first if were in a subroutine.
        if (rtStack.top()->symbolTable.find(varname) != rtStack.top()->symbolTable.end()) {
            int addr = rtStack.top()->symbolTable[varname];
            if (offset > 0) {
                if (offset > memStore.get(addr).attr.size) {
                    cout<<"Error: Index "<<offset<<" out of range for array "<<varname<<endl;
                    return;
                }
            }
            memStore.store(addr + offset, Object(ret));
            say("stored local variable " + varname  + " " + to_string(ret) + " at " + to_string(addr) + " offset: " + to_string(offset));
            onExit(" ");
            return;
        }
    }
    //not a local var, better check globals.
    if (variables.find(varname) != variables.end()) {
        int addr = variables[varname];
        if (offset > 0) {
            if (offset > memStore.get(addr).attr.size) {
                cout<<"Error: Index "<<offset<<" out of range for array "<<varname<<endl;
                return;
            }
        }
        memStore.store(addr + offset, Object(ret));
        say( "stored global variable " + varname + " value: " + to_string(ret) + " at " + to_string(addr) + " offset: " + to_string(offset));
    }
    onExit("AssignStatement ");
}

int Interpreter::interpretExpression(ASTNode* x) {
    int retVal = 0; int rbound = 1;
    string msg, result;
    onEnter("Expression " + ExprKindStr[x->type.expr]);
    switch (x->type.expr) {
        case CONST_EXPR:
            retVal = x->attribute.intValue;
            say(ExprKindStr[x->type.expr] + " value: " + to_string(retVal));
            onExit("");
            return retVal;
        case RAND_EXPR:
            rbound = x->child[0]->attribute.intValue;
            return rand() % (rbound - 1) + 1;
        case SUBSCRIPT_EXPR:
        case ID_EXPR:
            return handleIDEXPR(x);
            break; 
        case PROCDCALL:
            if (procedures.find(x->attribute.name) != procedures.end()) {
                retVal = Dispatch(x);
                onExit(" ");
                return retVal;
            }
            break;
        case OP_EXPR:
            retVal = eval(x);
            if (x->attribute.op == LESS || x->attribute.op == EQUAL || x->attribute.op == NOTEQUAL) {
                result = (retVal == 0) ? "false":"true";
                msg = "value: " + result;
                say(msg);
            } else
                say("value: " + to_string(retVal));
            onExit("");
            return retVal;
    }
    onExit("Expression");
    return retVal;
}

void Interpreter::interpretVarDeclaration(ASTNode* x) {
    string name;
    int addr;
    if (x->child[0]->type.expr == SUBSCRIPT_EXPR) {
        ASTNode* t = x->child[0];
        name = t->attribute.name;
        int size = t->child[0]->attribute.intValue;
        addr = memStore.allocate(size);
        memStore.get(addr).attr.size = size;
        say("Declaring Array: " + name + " of size " + to_string(size) + " at address " + to_string(addr));
    } else {
        name = x->child[0]->attribute.name;
        Object obj;
        obj.type = INTEGER;
        obj.data.intValue = x->child[1]->attribute.intValue;
        addr = memStore.storeAtNextFree(obj);
        say("Declaring Variable: " + name + " with value " + to_string(x->child[1]->attribute.intValue));
    }
    if (rtStack.empty())  variables[name] = addr;
    else rtStack.top()->symbolTable[name] = addr;
}


void Interpreter::interpretFuncDeclaration(ASTNode* x) {
    onEnter("Procedure Declaration: " + x->attribute.name);
    if (procedures.find(x->attribute.name) == procedures.end()) {
        StackFrame *procRec = new StackFrame;
        procRec->body = x->child[0];
        procRec->params = x->child[1]; 
        for (auto t = x->child[1]; t != nullptr; t = t->sibling) {
            say("Parameter: " + t->attribute.name + " added to procedures symbol table.");
            Object obj(0);
            procRec->symbolTable[t->attribute.name] = memStore.storeAtNextFree(obj);
        } 
        procedures[x->attribute.name] = procRec;
    }
    onExit("Procedure Declaration: " + x->attribute.name);
}


void Interpreter::interpretPrintStatement(ASTNode* x) {
    onEnter("[PRINT]");
    cout<<interpretExpression(x)<<endl;
    onExit("[PRINT]");
}

void Interpreter::interpretReadStatement(ASTNode* x) {
    int input;
    cin>>input;
    int addr = variables[x->attribute.name];
    memStore.store(addr, Object(input));
}

void Interpreter::interpretIfStatement(ASTNode* x) {
    int res = interpretExpression(x->child[0]);
    if (res) {
        say("passed test");
        interpretStatement(x->child[1]);
    } else {
        if (x->child[2]) {
            say("else clause");
            interpretStatement(x->child[2]);
        }
    }
}

void Interpreter::interpretWhileStatement(ASTNode* x) {
    onEnter("While Loop");
    int ret = interpretExpression(x->child[0]);
    while (ret) {
        say("Execute Body: ");
        Execute(x->child[1]);
        say("check loop condition: ");
        ret = eval(x->child[0]);
        say("Result: " + to_string(ret));
    }
    onExit("Leaving While Statement");
}

void Interpreter::interpretReturnStatement(ASTNode* x) {
    onEnter("Return Statement.");
    int retVal =  interpretExpression(x->child[0]);
    rtStack.top()->returnVal = retVal;
    say(to_string(retVal) + " saved as return value.");
    onExit(" ");
}


StackFrame* Interpreter::prepStackFrame(StackFrame* x) {
    StackFrame* y = new StackFrame;
    y->body = x->body;
    y->params = x->params;
    y->returnVal = x->returnVal;
    for (auto m : x->symbolTable) {
        y->symbolTable[m.first] = memStore.allocate(1);
    }
    return y;
}

int Interpreter::Dispatch(ASTNode* x) {
    say("Dispatch: " + x->attribute.name);
    int retVal = 0;
    if (procedures.find(x->attribute.name) != procedures.end()) {
        StackFrame* nsf = prepStackFrame(procedures[x->attribute.name]);
        auto paramIt = nsf->params;
        auto argIt = x->child[1];
        //now we want to assign the parameters to their correct symbol tables.
        while (paramIt && argIt) {
            Object obj;
            obj.data.intValue = interpretExpression(argIt);
            memStore.store(nsf->symbolTable[paramIt->attribute.name], obj);
            paramIt = paramIt->sibling;
            argIt = argIt->sibling;
        }
        rtStack.push(nsf);
        Execute(rtStack.top()->body);
        int retVal = rtStack.top()->returnVal;
        for (auto addr : rtStack.top()->symbolTable) {
            memStore.free(addr.second);
        }
        rtStack.pop();
        say("value returned: " + to_string(retVal));
        return retVal;
    } else {
        say("No such procedure: " + x->attribute.name);
    }
    onExit("Dispatch");
    return retVal;
}

void Interpreter::interpretExprStatement(ASTNode* x) {
    onEnter("ExprStatement ");
    
    switch (x->child[0]->kind) {
        case STMTNODE:
            interpretAssignment(x->child[0]);
            break;
        case EXPRNODE:
            interpretExpression(x->child[0]);
            break;
    }
    onExit("ExprStatement ");
}

void Interpreter::interpretStatement(ASTNode* x) {
    switch (x->type.stmt) {
        case VARDECL:
            interpretVarDeclaration(x);
            break;
        case FUNCDECL:
            interpretFuncDeclaration(x);
            break;
        case PRINTSTM:
            interpretPrintStatement(x->child[0]);
            break;
        case READSTM:
            interpretReadStatement(x->child[0]);
            break;
        case IFSTM:
            interpretIfStatement(x); 
            break;
        case WHILESTM:
            interpretWhileStatement(x);
            break;
        case EXPRSTM:
            interpretExprStatement(x);
            break;
        case ASSIGNSTM:
            interpretAssignment(x->child[0]);
            break;
        case RETURNSTM:
            interpretReturnStatement(x);
            break;
    }
}


void Interpreter::Execute(ASTNode* x) {
    if (x == nullptr)
        return;
    switch (x->kind) {
        case EXPRNODE:
            interpretExpression(x);
            break;
        case STMTNODE:
            interpretStatement(x);
            break;
    }
    if (x->kind == STMTNODE && x->type.stmt == RETURNSTM)
        return;
    Execute(x->sibling);
}

#endif