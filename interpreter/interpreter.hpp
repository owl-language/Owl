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
        Object stringOp(TokenType op, Object left, Object right);
        Object mathOp(TokenType op, Object left, Object right);
        Object relOp(TokenType op, Object left, Object right);
        Object interpretExpression(ASTNode* x);
        Object retrieveFromMemoryByName(ASTNode* x);
        void storeToMemoryByName(ASTNode* x);
        void interpretReturnStatement(ASTNode* x);
        void interpretVarDeclaration(ASTNode* x);
        void interpretFuncDeclaration(ASTNode* x);
        void interpretPrintStatement(ASTNode* x);
        void interpretReadStatement(ASTNode* x);
        void interpretIfStatement(ASTNode* x);
        void interpretStatement(ASTNode* x);
        void interpretWhileStatement(ASTNode* x);
        void interpretExprStatement(ASTNode* x);
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

Object Interpreter::stringOp(TokenType op, Object left, Object right) {
    onEnter("String Operation");
    Object retObj(" ");
    Object convObj;
    if (left.type == STRING) {
        retObj.data.stringValue = left.data.stringValue;
        convObj = right;
    } else {
        retObj.data.stringValue = right.data.stringValue;
        convObj = left;
    }
    if (convObj.type == INTEGER)
        retObj.data.stringValue += to_string(convObj.data.intValue);
    else if (convObj.type == REAL)
        retObj.data.stringValue += to_string(convObj.data.realValue);
    onExit();
    return retObj;
}

Object Interpreter::mathOp(TokenType op, Object leftChild, Object rightChild) {
    onEnter("mathOp");
    float leftOperand, rightOperand, result = 0;
    Object retObj;
    if (leftChild.type == INTEGER) {
        leftOperand = (float) leftChild.data.intValue;
    }  else if (leftChild.type == REAL) {
        leftOperand = leftChild.data.realValue;
    }
    if (rightChild.type == INTEGER) {
        rightOperand = (float) rightChild.data.intValue;
    } else if (rightChild.type == REAL) {
        rightOperand = rightChild.data.realValue;
    }
    say(to_string(leftOperand) + "op" + to_string(rightOperand));
    switch (op) {
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
        default:
            break;
    }
    if (leftChild.type == INTEGER && rightChild.type == INTEGER) {
        retObj.type = INTEGER;
        retObj.data.intValue = (int)result;
    } else {
        retObj.type = REAL;
        retObj.data.realValue = result;
    }
    onExit();
    return retObj;
}

Object Interpreter::relOp(TokenType op, Object leftChild, Object rightChild) {
    onEnter("relop");
    bool result;
    float leftOperand, rightOperand;
    if (leftChild.type == INTEGER) {
        leftOperand = (float) leftChild.data.intValue;
    }  else if (leftChild.type == REAL) {
        leftOperand = leftChild.data.realValue;
    }
    if (rightChild.type == INTEGER) {
        rightOperand = (float) rightChild.data.intValue;
    } else if (rightChild.type == REAL) {
        rightOperand = rightChild.data.realValue;
    }
    say(to_string(leftOperand) + "op" + to_string(rightOperand));
    switch (op) {
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
    Object retVal = Object(result);
    retVal.type = INTEGER;
    return retVal;
}

Object Interpreter::eval(ASTNode* x) {
    onEnter("eval: " + tokenString[x->attribute.op]);
    Object leftChild, rightChild, retObj;
    float leftOperand, rightOperand, result = 0;
    leftChild = interpretExpression(x->child[0]);
    rightChild = interpretExpression(x->child[1]);
    if (leftChild.type == STRING || rightChild.type == STRING)
        return stringOp(x->attribute.op, leftChild, rightChild);
    if (x->attribute.op == PLUS || x->attribute.op == MINUS || x->attribute.op == MULT || x->attribute.op == DIVD)
        return mathOp(x->attribute.op, leftChild, rightChild);    
    if (x->attribute.op == EQUAL || x->attribute.op == NOTEQUAL || x->attribute.op == LESS || x->attribute.op == GREATER || x->attribute.op == LESSEQ || x->attribute.op == GREATEREQ)
        return relOp(x->attribute.op, leftChild, rightChild);
    onExit("eval result: " + retObj.toString());
    return retObj;
}

Object Interpreter::retrieveFromMemoryByName(ASTNode* x) {
    onEnter("retrieveFromMemoryByName");
    Object retVal;
    memStore.display();
    int offset = 0, addr = 0;
    if (procedures.find(x->attribute.name) != procedures.end()) {
        say("Procedure Found, Dispatching.");
        retVal = Dispatch(x);
        say("return from " + x->attribute.name + " value: " + retVal.toString());
        onExit();
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
        onExit();
        return retVal;
    }
    
    if (offset > memStore.get(addr).attr.size) {
        cout<<"Error: Index "<<offset<<" out of range for array "<<x->attribute.name<<endl;
        return -1;
    }
    retVal = memStore.get(addr + offset);
    say("ID: " + x->attribute.name + ", Address: " + to_string(addr) + ", offset: " +to_string(offset) + ", value: " + retVal.toString() + ", type: " + rtTypeAsStr[retVal.type]);
    onExit();
    return retVal;
}

void Interpreter::storeToMemoryByName(ASTNode* x) {
    memStore.display();
    onEnter("storeToMemoryByName ");
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
        onExit();
        return;
    }
    if (offset > 0 && offset > memStore.get(addr).attr.size) {
        cout<<"Error: Index "<<offset<<" out of range for array "<<varname<<endl;
        onExit();
        return;
    }
    memStore.store(addr + offset, valToAssign);
    onExit("storeToMemoryByName ");
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
            return retrieveFromMemoryByName(x);
            break; 
        case PROCDCALL:
            if (procedures.find(x->attribute.name) != procedures.end()) {
                retVal = Dispatch(x);
                onExit();
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
        ASTNode* child = x->child[0];
        name = child->attribute.name;
        int size = child->child[0]->attribute.intValue;
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


void Interpreter::interpretFuncDeclaration(ASTNode* node) {
    onEnter("Procedure Declaration: " + node->attribute.name);
    if (procedures.find(node->attribute.name) == procedures.end()) {
        StackFrame *procRec = new StackFrame;
        procRec->body = node->child[0];
        procRec->params = node->child[1]; 
        for (ASTNode* param = node->child[1]; param != nullptr; param = param->sibling) {
            say("Parameter: " + param->attribute.name + " added to procedures symbol table.");
            Object obj(0);
            procRec->symbolTable[param->attribute.name] = memStore.storeAtNextFree(obj);
        } 
        procedures[node->attribute.name] = procRec;
    }
    onExit("Procedure Declaration: " + node->attribute.name);
}


void Interpreter::interpretPrintStatement(ASTNode* node) {
    onEnter("[PRINT]");
    if (node->type.expr == CONST_STR) //I mean, thats WHY its a const string, right?
        cout<<node->attribute.name<<endl;
    else
        cout<<interpretExpression(node).toString()<<endl;
    onExit("[PRINT]");
}

void Interpreter::interpretReadStatement(ASTNode* node) {
    int input;
    cin>>input;
    int addr = variables[node->attribute.name];
    memStore.store(addr, Object(input));
}

void Interpreter::interpretIfStatement(ASTNode* node) {
    int res = interpretExpression(node->child[0]).data.intValue;
    if (res) {
        say("passed test");
        interpretStatement(node->child[1]);
    } else {
        if (node->child[2]) {
            say("else clause");
            interpretStatement(node->child[2]);
        }
    }
}

void Interpreter::interpretWhileStatement(ASTNode* node) {
    onEnter("While Loop");
    Object ret = interpretExpression(node->child[0]).data.intValue;
    while (ret.data.intValue) {
        say("Execute Body: ");
        Execute(node->child[1]);
        say("check loop condition: ");
        ret = eval(node->child[0]);
        say("Result: " + ret.toString());
    }
    onExit("Leaving While Statement");
}

void Interpreter::interpretReturnStatement(ASTNode* node) {
    onEnter("Return Statement.");
    Object retVal =  interpretExpression(node->child[0]);
    rtStack.top()->returnVal = retVal;
    say(retVal.toString() + " saved as return value.");
    onExit();
}


StackFrame* Interpreter::prepStackFrame(StackFrame* curentFrame) {
    StackFrame* nextFrame = new StackFrame;
    nextFrame->body = curentFrame->body;
    nextFrame->params = curentFrame->params;
    nextFrame->returnVal = curentFrame->returnVal;
    for (auto m : curentFrame->symbolTable) {
        nextFrame->symbolTable[m.first] = memStore.allocate(1);
    }
    return nextFrame;
}

Object Interpreter::Dispatch(ASTNode* node) {
    say("Dispatch: " + node->attribute.name);
    Object retVal;
    if (procedures.find(node->attribute.name) != procedures.end()) {
        StackFrame* nsf = prepStackFrame(procedures[node->attribute.name]);
        auto paramIt = nsf->params;
        auto argIt = node->child[1];
        //now we want to assign the parameters to their correct symbol tables.
        while (paramIt && argIt) {
            Object obj = interpretExpression(argIt);
            memStore.store(nsf->symbolTable[paramIt->attribute.name], obj);
            paramIt = paramIt->sibling;
            argIt = argIt->sibling;
        }
        rtStack.push(nsf);
        Execute(rtStack.top()->body);
        retVal = rtStack.top()->returnVal;
        for (auto addr : rtStack.top()->symbolTable) {
            memStore.free(addr.second);
        }
        rtStack.pop();
        say("value returned: " + retVal.toString());
        return retVal;
    } else {
        say("No such procedure: " + node->attribute.name);
    }
    onExit("Dispatch");
    return retVal;
}

void Interpreter::interpretExprStatement(ASTNode* node) {
    onEnter("ExprStatement ");
    
    switch (node->child[0]->kind) {
        case STMTNODE:
            storeToMemoryByName(node->child[0]);
            break;
        case EXPRNODE:
            interpretExpression(node->child[0]);
            break;
    }
    onExit("ExprStatement ");
}

void Interpreter::interpretStatement(ASTNode* node) {
    switch (node->type.stmt) {
        case VARDECL:
            interpretVarDeclaration(node);
            break;
        case FUNCDECL:
            interpretFuncDeclaration(node);
            break;
        case PRINTSTM:
            interpretPrintStatement(node->child[0]);
            break;
        case READSTM:
            interpretReadStatement(node->child[0]);
            break;
        case IFSTM:
            interpretIfStatement(node); 
            break;
        case WHILESTM:
            interpretWhileStatement(node);
            break;
        case EXPRSTM:
            interpretExprStatement(node);
            break;
        case ASSIGNSTM:
            storeToMemoryByName(node->child[0]);
            break;
        case RETURNSTM:
            interpretReturnStatement(node);
            break;
    }
}


void Interpreter::Execute(ASTNode* node) {
    if (node == nullptr)
        return;
    switch (node->kind) {
        case EXPRNODE:
            interpretExpression(node);
            break;
        case STMTNODE:
            interpretStatement(node);
            break;
    }
    if (node->kind == STMTNODE && node->type.stmt == RETURNSTM)
        return;
    Execute(node->sibling);
}

#endif