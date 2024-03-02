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
        int Dispatch(ASTNode* x);
    public:
        void Execute(ASTNode* x);

};

int Interpreter::eval(ASTNode* x) {
    onEnter("eval: " + tokenString[x->attribute.intValue]);

    int leftOperand, rightOperand, result = 0;
    switch (x->attribute.op) {
        case PLUS:
            leftOperand = interpretExpression(x->child[0]);
            rightOperand = interpretExpression(x->child[1]); 
            result = leftOperand + rightOperand;
            break;
        case MINUS:
            leftOperand = interpretExpression(x->child[0]);
            rightOperand = interpretExpression(x->child[1]); 
            result = leftOperand - rightOperand;
            break;
        case MULT:
            leftOperand = interpretExpression(x->child[0]);
            rightOperand = interpretExpression(x->child[1]); 
            result = leftOperand * rightOperand;
            break;
        case LESS:
            leftOperand = interpretExpression(x->child[0]);
            rightOperand = interpretExpression(x->child[1]); 
            result = (leftOperand < rightOperand);
            break;
        case GREATER:
            leftOperand = interpretExpression(x->child[0]);
            rightOperand = interpretExpression(x->child[1]); 
            result = (leftOperand > rightOperand);
            break;
        case EQUAL:
            leftOperand = interpretExpression(x->child[0]);
            rightOperand = interpretExpression(x->child[1]); 
            result = (leftOperand == rightOperand);
            break;
        case NOTEQUAL:
            leftOperand = interpretExpression(x->child[0]);
            rightOperand = interpretExpression(x->child[1]); 
            result = (leftOperand != rightOperand);
            break;
        case LESSEQ:
            leftOperand = interpretExpression(x->child[0]);
            rightOperand = interpretExpression(x->child[1]); 
            result = (leftOperand <= rightOperand);
            break;
        case GREATEREQ:
            leftOperand = interpretExpression(x->child[0]);
            rightOperand = interpretExpression(x->child[1]); 
            result = (leftOperand >= rightOperand);
            break;
    }
    onExit("eval" + x->attribute.name);
    return result;
}

int Interpreter::handleIDEXPR(ASTNode* x) {
    int retVal = 0;
    if (rtStack.size()) {
        if (rtStack.top()->symbolTable.find(x->attribute.name) != rtStack.top()->symbolTable.end()) {
            retVal = memStore.get(rtStack.top()->symbolTable[x->attribute.name]).data.intValue;
            say("ID: " + x->attribute.name + " value: " + to_string(retVal));
            onExit(" ");
            return retVal;
        }
    }
    if (variables.find(x->attribute.name) != variables.end())
        retVal = memStore.get(variables[x->attribute.name]).data.intValue;
        say("ID: " + x->attribute.name + " value: " + to_string(retVal));
        onExit(" ");
        return retVal;
    if (procedures.find(x->attribute.name) != procedures.end()) {
        retVal = Dispatch(x);
        say("return from " + x->attribute.name + " value: " + to_string(retVal));
        onExit(" ");
        return retVal;
    }
}

int Interpreter::interpretExpression(ASTNode* x) {
    int retVal = 0;
    string msg, result;
    onEnter("Expression " + ExprKindStr[x->type.expr]);
    switch (x->type.expr) {
        case CONST_EXPR:
            retVal = x->attribute.intValue;
            say(ExprKindStr[x->type.expr] + " value: " + to_string(retVal));
            onExit("");
            return retVal;
        case ID_EXPR:
            return handleIDEXPR(x);
            break; 
        case PROCDCALL:
            if (procedures.find(x->attribute.name) != procedures.end()) {
                retVal = Dispatch(x);
                say("return from " + x->attribute.name + " value: " + to_string(retVal));
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
    return 0;
}

void Interpreter::interpretVarDeclaration(ASTNode* x) {
    if (x->child[0]->type.expr == SUBSCRIPT_EXPR) {
        say("Arrays not fully supported yet chief.");
    } else {
        say("Declaring Variable: " + x->child[0]->attribute.name + " with value " + to_string(x->child[1]->attribute.intValue));
        Object obj;
        obj.type = INTEGER;
        obj.data.intValue = x->child[1]->attribute.intValue;
        int addr = memStore.storeAtNextFree(obj);
        variables[x->child[0]->attribute.name] = addr;
    }
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
        cout<<"passed test"<<endl;
        interpretStatement(x->child[1]);
    } else {
        cout<<"else clause"<<endl;
        interpretStatement(x->child[2]);
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
}


StackFrame* cloneSF(StackFrame* x) {
    StackFrame* y = new StackFrame;
    y->body = x->body;
    y->params = x->params;
    y->returnVal = x->returnVal;
    for (auto m : x->symbolTable) {
        y->symbolTable[m.first] = m.second;
    }
    return y;
}

int Interpreter::Dispatch(ASTNode* x) {
    say("Dispatch: " + x->attribute.name);
    int retVal = 0;
    if (procedures.find(x->attribute.name) != procedures.end()) {
        StackFrame* nsf = cloneSF(procedures[x->attribute.name]);
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
        rtStack.pop();
        if (rtStack.size() > 0) rtStack.top()->returnVal = retVal; 
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

void Interpreter::interpretAssignment(ASTNode* x) {
    onEnter("AssignStatement ");
    int ret = interpretExpression(x->child[1]);
    memStore.store(variables[x->child[0]->attribute.name], ret);
    onExit("AssignStatement ");
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
            interpretAssignment(x);
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
    Execute(x->sibling);
}

#endif