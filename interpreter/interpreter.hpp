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
        Object eval(ASTNode* x);
        Object interpretExpression(ASTNode* x);
        Object handleIDEXPR(ASTNode* x);
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
        Object Dispatch(ASTNode* x);
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

Object Interpreter::eval(ASTNode* x) {
    onEnter("eval: " + tokenString[x->attribute.op]);
    Object leftChild, rightChild, retObj;
    float leftOperand, rightOperand, result = 0;
    leftChild = interpretExpression(x->child[0]);
    rightChild = interpretExpression(x->child[1]);
    if (leftChild.type == INTEGER) {
        leftOperand = (float) leftChild.data.intValue;
        rightOperand = (float) rightChild.data.intValue;
    } else {
        leftOperand = leftChild.data.realValue;
        rightOperand = rightChild.data.realValue;
    }
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
    if (leftChild.type == INTEGER || rightChild.type == INTEGER) {
        retObj.type = INTEGER;
        retObj.data.intValue = (int)result;
    } else {
        retObj.type = REAL;
        retObj.data.realValue = result;
    }
    onExit("eval result: " + retObj.toString());
    return retObj;
}

Object Interpreter::handleIDEXPR(ASTNode* x) {
    onEnter("handleIDExpr");
    Object retVal;
    int offset = 0, addr = 0;
    if (procedures.find(x->attribute.name) != procedures.end()) {
        say("Procedure Found, Dispatching.");
        retVal = Dispatch(x);
        say("return from " + x->attribute.name + " value: " + retVal.toString());
        onExit(" ");
        return retVal;
    }
    if (x->kind == EXPRNODE && x->type.expr == SUBSCRIPT_EXPR) {
        say("Calculating Offset.");
        offset = interpretExpression(x->child[0]).data.intValue;
        if (offset < 0) {
            cout<<"Error: Invalid Index: "<<offset<<endl;
            return Object(-1);
        }
        say("Array Reference, offset: " + to_string(offset));
    }
    if (rtStack.size() && (rtStack.top()->symbolTable.find(x->attribute.name) != rtStack.top()->symbolTable.end())) {
        addr = rtStack.top()->symbolTable[x->attribute.name];
    } else  if (variables.find(x->attribute.name) != variables.end()) {
        addr = variables[x->attribute.name];
    } else {
        cout<<"Uh oh, couldnt find: "<<x->attribute.name<<endl;
        onExit(" ");
        return retVal;
    }
    
    if (offset > memStore.get(addr).attr.size) {
        cout<<"Error: Index "<<offset<<" out of range for array "<<x->attribute.name<<endl;
        return -1;
    }
    retVal = memStore.get(addr + offset);
    say("ID: " + x->attribute.name + ", Address: " + to_string(addr) + ", offset: " +to_string(offset) + ", value: " + retVal.toString() + "type: " + rtTypeAsStr[retVal.type]);
    onExit(" ");
    return retVal;
}

void Interpreter::interpretAssignment(ASTNode* x) {
    onEnter("AssignStatement ");
    string varname = x->child[0]->attribute.name; //variable name
    say("Assign to " + varname + ": ");
    Object valToAssign = interpretExpression(x->child[1]);   //value to assign
    int offset = 0, addr = 0;
    if (x->child[0]->kind == EXPRNODE && x->child[0]->type.expr == SUBSCRIPT_EXPR) {
        say("Retrieving subscript " + x->child[0]->attribute.name);
        offset = interpretExpression(x->child[0]->child[0]).data.intValue;
        if (offset < 0) {
            cout<<"Error: Invalid Index: "<<offset<<endl;
        }
        say("Array Reference, offset: " + offset);
    }
    if (rtStack.size() && rtStack.top()->symbolTable.find(varname) != rtStack.top()->symbolTable.end()) {
        addr = rtStack.top()->symbolTable[varname];
        say("stored local variable " + varname  + " with value: " + valToAssign.toString() + " at " + to_string(addr) + " offset: " + to_string(offset) + " as: " + rtTypeAsStr[valToAssign.type]);
    } else if (variables.find(varname) != variables.end()) {
        addr = variables[varname];
        say("stored global variable " + varname + " value: " + valToAssign.toString() + " at " + to_string(addr) + " offset: " + to_string(offset) + " as: " + rtTypeAsStr[valToAssign.type]);
    } else {
        cout<<"Error: unknown identifier: "<<varname<<endl;
        return;
    }
    if (offset > memStore.get(addr).attr.size) {
        cout<<"Error: Index "<<offset<<" out of range for array "<<varname<<endl;
        return;
    }
    memStore.store(addr + offset, valToAssign);
    onExit("AssignStatement ");
}

Object Interpreter::interpretExpression(ASTNode* x) {
    Object retVal; int rbound = 1;
    string msg, result;
    onEnter("Expression " + ExprKindStr[x->type.expr]);
    switch (x->type.expr) {
        case CONST_EXPR:
            if (x->attribute.type == as_int)
                retVal = Object(x->attribute.intValue);
            else if (x->attribute.type == as_real)
                retVal = Object(stof(x->attribute.name));
            say(ExprKindStr[x->type.expr] + " value: " + retVal.toString());
            onExit("");
            return retVal;
        case RAND_EXPR:
            rbound = x->child[0]->attribute.intValue;
            return Object(rand() % (rbound - 1) + 1);
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
                result = (retVal.data.intValue == 0) ? "false":"true";
                msg = "value: " + result;
                say(msg);
            } else
                say("value: " + retVal.toString());
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
        if (x->child[1]->attribute.type == as_string) {
            obj.type = STRING;
            obj.data.stringValue = x->child[1]->attribute.name;
        } else if (x->child[1]->attribute.type == as_real) {
            obj.type = REAL;
            obj.data.realValue = stof(x->child[1]->attribute.name);
        } else {
            obj.type = INTEGER;
            obj.data.intValue = x->child[1]->attribute.intValue;
        }
        addr = memStore.storeAtNextFree(obj);
        say("Declaring Variable: " + name + " with value " + obj.toString() + " type: " + rtTypeAsStr[obj.type]);
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
    if (x->type.expr == CONST_STR) //I mean, thats WHY its a const string, right?
        cout<<x->attribute.name<<endl;
    else
        cout<<interpretExpression(x).toString()<<endl;
    onExit("[PRINT]");
}

void Interpreter::interpretReadStatement(ASTNode* x) {
    int input;
    cin>>input;
    int addr = variables[x->attribute.name];
    memStore.store(addr, Object(input));
}

void Interpreter::interpretIfStatement(ASTNode* x) {
    int res = interpretExpression(x->child[0]).data.intValue;
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
    Object ret = interpretExpression(x->child[0]).data.intValue;
    while (ret.data.intValue) {
        say("Execute Body: ");
        Execute(x->child[1]);
        say("check loop condition: ");
        ret = eval(x->child[0]);
        say("Result: " + ret.toString());
    }
    onExit("Leaving While Statement");
}

void Interpreter::interpretReturnStatement(ASTNode* x) {
    onEnter("Return Statement.");
    Object retVal =  interpretExpression(x->child[0]);
    rtStack.top()->returnVal = retVal;
    say(retVal.toString() + " saved as return value.");
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

Object Interpreter::Dispatch(ASTNode* x) {
    say("Dispatch: " + x->attribute.name);
    int retVal = 0;
    if (procedures.find(x->attribute.name) != procedures.end()) {
        StackFrame* nsf = prepStackFrame(procedures[x->attribute.name]);
        auto paramIt = nsf->params;
        auto argIt = x->child[1];
        //now we want to assign the parameters to their correct symbol tables.
        while (paramIt && argIt) {
            Object obj = interpretExpression(argIt);
            memStore.store(nsf->symbolTable[paramIt->attribute.name], obj);
            paramIt = paramIt->sibling;
            argIt = argIt->sibling;
        }
        rtStack.push(nsf);
        Execute(rtStack.top()->body);
        Object retVal = rtStack.top()->returnVal;
        for (auto addr : rtStack.top()->symbolTable) {
            memStore.free(addr.second);
        }
        rtStack.pop();
        say("value returned: " + retVal.toString());
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