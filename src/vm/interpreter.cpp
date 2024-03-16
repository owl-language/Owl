#include "interpreter.hpp"

Object Interpreter::stringOp(TokenType op, Object left, Object right) {
    onEnter("String Operation");
    Object retObj(" ");
    Object convObj;
    if (op == PLUS) {
        if (left.type == STRING) {
            retObj.data._value = left.data.stringValue();
            convObj = right;
            if (convObj.type == INTEGER) convObj.data._value = to_string(convObj.data.intValue());
                else if (convObj.type == REAL) convObj.data._value = to_string(convObj.data.realValue());
                retObj.data._value += convObj.data.stringValue();
            } else {
                retObj.data._value = right.data.stringValue();
                convObj = left;
                if (convObj.type == INTEGER) convObj.data._value = to_string(convObj.data.intValue());
                else if (convObj.type == REAL) convObj.data._value = to_string(convObj.data.realValue());
                retObj.data._value = convObj.data.stringValue() + retObj.data.stringValue();
            }
    } else if (op == EQUAL) {
        retObj = Object(left.data.stringValue() == right.data.stringValue());
    } else if (op == NOTEQUAL) {
        retObj = Object(left.data.stringValue() != right.data.stringValue());
    } else {
        logError("Hoot! Operation: " + tokenString[op] + " not supported on strings.");
        retObj = Object(0);
    }
    onExit();
    return retObj;
}

float divide(float a, float b) {
    if (b == 0) {
        logError("Hoot! Hoot!: Divide By Zero. (Don't do that.)");
        return 0;
    }
    return a/b;
}

Object Interpreter::mathOp(TokenType op, Object leftChild, Object rightChild) {
    onEnter("math Op");
    Object retObj;
    float leftOperand, rightOperand, result = 0;
    leftOperand = leftChild.data.realValue();
    rightOperand = rightChild.data.realValue();
    say(to_string(leftOperand) + " " + tokenString[op] + " " + to_string(rightOperand));
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
    } else {
        retObj.type = REAL;
    }
    retObj.data._value = to_string(result);
    onExit();
    return retObj;
}

Object Interpreter::relOp(TokenType op, float leftOperand, float rightOperand) {
    onEnter("rel Op");
    Object retVal;
    say(to_string(leftOperand) + " " + tokenString[op] + " " + to_string(rightOperand));
    bool result;
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
    retVal = Object(result);
    retVal.type = INTEGER;
    return retVal;
}

Object Interpreter::eval(ASTNode* x) {
    onEnter("eval: " + tokenString[x->attribute.op]);
    Object leftChild, rightChild, retObj;
    float leftOperand, rightOperand, result = 0;
    leftChild = interpretExpression(x->child[0]);
    onExit();
    rightChild = interpretExpression(x->child[1]);
    if (leftChild.type != STRING)
        leftOperand = leftChild.data.realValue();
    if (rightChild.type != STRING)
        rightOperand = rightChild.data.realValue();
        
    if (leftChild.type == STRING || rightChild.type == STRING)
        return stringOp(x->attribute.op, leftChild, rightChild);
    
    if (x->attribute.op == PLUS || x->attribute.op == MINUS || 
        x->attribute.op == MULT || x->attribute.op == DIVD) {
        retObj = mathOp(x->attribute.op, leftChild, rightChild);    
    }
    if (x->attribute.op == EQUAL || x->attribute.op == NOTEQUAL || x->attribute.op == LESS || 
        x->attribute.op == GREATER || x->attribute.op == LESSEQ || x->attribute.op == GREATEREQ) {
        retObj = relOp(x->attribute.op, leftOperand, rightOperand);
    }
    say("eval result: " + retObj.toString());
    onExit();
    return retObj;
}

Object Interpreter::retrieveFromMemoryByName(ASTNode* x) {
    onEnter("retrieveFromMemoryByName");
    Object retVal;
    int offset = 0, addr = 0;
    if (x->kind == EXPRNODE && x->type.expr == SUBSCRIPT_EXPR) {
        say("Calculating Offset.");
        offset = interpretExpression(x->child[0]).data.intValue();
        if (offset < 0) {
            logError("Hoot! Invalid Index: " + to_string(offset));
            return Object(-1);
        }
        say("Array Reference, offset: " + to_string(offset));
    }
    if (callStack.size() && callStack.top()->symbolTable.find(x->attribute.name) != callStack.top()->symbolTable.end()) {
        addr = callStack.top()->symbolTable[x->attribute.name].addr;
    } else if (callStack.size() > 1 && callStack.top()->staticLink->symbolTable.find(x->attribute.name) != callStack.top()->staticLink->symbolTable.end()) {
        addr = callStack.top()->staticLink->symbolTable[x->attribute.name].addr;
    } else  if (variables.find(x->attribute.name) != variables.end()) {
        addr = variables[x->attribute.name];
    } else {
        logError("Hoot! couldnt find: " + x->attribute.name);
        onExit();
        return retVal;
    }
    
    if (offset > memStore.get(addr).attr.size) {
        logError("Hoot! Index " + to_string(offset) + " out of range for array " + x->attribute.name);
        return -1;
    }
    retVal = memStore.get(addr + offset);
    say("ID: " + x->attribute.name + ", Address: " + to_string(addr) + ", offset: " +to_string(offset) + ", value: " + retVal.toString() + ", type: " + rtTypeAsStr[retVal.type]);
    onExit();
    return retVal;
}

void Interpreter::storeToMemoryByName(ASTNode* x) {
    onEnter("storeToMemoryByName ");
    string varname = x->child[0]->attribute.name; //variable name
    say("Assign to " + varname + ": ");
    Object valToAssign = interpretExpression(x->child[1]);   //value to assign
    int offset = 0, addr = 0;
    if (x->child[0]->kind == EXPRNODE && x->child[0]->type.expr == SUBSCRIPT_EXPR) {
        say("Retrieving subscript " + x->child[0]->attribute.name);
        offset = interpretExpression(x->child[0]->child[0]).data.intValue();
        if (offset <= 0) {
            logError("Hoot! Invalid Index: " + to_string(offset));
        }
        say("Array Reference, offset: " + offset);
    }
    if (callStack.size() && callStack.top()->symbolTable.find(varname) != callStack.top()->symbolTable.end()) {
        addr = callStack.top()->symbolTable[varname].addr;
        say("stored local variable " + varname  + " with value: " + valToAssign.toString() + " at " + to_string(addr) + " offset: " + to_string(offset) + " as: " + rtTypeAsStr[valToAssign.type]);
    } else if (variables.find(varname) != variables.end()) {
        addr = variables[varname];
        say("stored global variable " + varname + " value: " + valToAssign.toString() + " at " + to_string(addr) + " offset: " + to_string(offset) + " as: " + rtTypeAsStr[valToAssign.type]);
    } else {
        logError("Error: unknown identifier: " + varname);
        onExit();
        return;
    }
    if (offset > 0 && offset > memStore.get(addr).attr.size) {
        logError("Hoot! Index " + to_string(offset) + " out of range for array " + varname);
        onExit();
        return;
    }
    memStore.store(addr + offset, valToAssign);
    onExit("storeToMemoryByName ");
}

Object Interpreter::doProcedureCall(ASTNode* x) {
    if (!callStack.empty() && callStack.top()->nestedProcedures.find(x->attribute.name) != callStack.top()->nestedProcedures.end()) {
        say("nested procedure.");
        return Dispatch(x, prepStackFrame(callStack.top()->nestedProcedures[x->attribute.name]));
    } else if (procedures.find(x->attribute.name) != procedures.end()) {
        say("global procedure");
        return Dispatch(x, prepStackFrame(procedures[x->attribute.name]));
    } else {
        logError("No such procedure: " + x->attribute.name);
    }
    return Object(0);
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
            return retVal;
        case CONST_STR:
            retVal = Object(x->attribute.name);
            retVal.type = STRING;
            return retVal;
        case RAND_EXPR:
            rbound = x->child[0]->attribute.intValue;
            return Object(rand() % (rbound - 1) + 1);
        case SUBSCRIPT_EXPR:
        case ID_EXPR:
            return retrieveFromMemoryByName(x);
            break; 
        case PROCDCALL:
            return doProcedureCall(x);
            break;
        case OP_EXPR:
            retVal = eval(x);
            if (x->attribute.op == LESS || x->attribute.op == EQUAL || x->attribute.op == NOTEQUAL) {
                result = (retVal.data.intValue() == 0) ? "false":"true";
                msg = "value: " + result;
                say(msg);
            } else
                say("value: " + retVal.toString());
            onExit();
            return retVal;
    }
    onExit();
    return retVal;
}

void Interpreter::declareVariable(ASTNode* x) {
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
        Object obj = interpretExpression(x->child[1]);
        addr = memStore.storeAtNextFree(obj);
        say("Declaring Variable: " + name + " with value " + obj.toString() + " type: " + rtTypeAsStr[obj.type]);
    }
    if (callStack.empty())  variables[name] = addr;
    else callStack.top()->symbolTable[name].addr = addr;
}

void Interpreter::declareStruct(ASTNode* x) {

};

void Interpreter::addProcedureToSymbolTable(map<string, StackFrame*>& procdefs, ASTNode* node) {
    StackFrame *procRec = new StackFrame;
    procRec->body = node->child[0];
    procRec->params = node->child[1]; 
    for (ASTNode* param = node->child[1]; param != nullptr; param = param->sibling) {
        say("Parameter: " + param->attribute.name + " added to procedures symbol table.");
        Object obj(0);
        procRec->symbolTable[param->attribute.name].addr = memStore.storeAtNextFree(obj);
    } 
    procdefs[node->attribute.name] = procRec;
}

void Interpreter::declareFunction(ASTNode* node) {
    onEnter("Procedure Declaration: " + node->attribute.name);
    if (!callStack.empty() && callStack.top()->nestedProcedures.find(node->attribute.name) ==  callStack.top()->nestedProcedures.end()) {
        addProcedureToSymbolTable(callStack.top()->nestedProcedures, node);
        say("nested procedure " + node->attribute.name + " added to stackframe");
    } else if (procedures.find(node->attribute.name) == procedures.end()) {
        addProcedureToSymbolTable(procedures, node);
        say("procedure " + node->attribute.name + " added to global namespace");
    } else {
        logError("A procedure with name " + node->attribute.name + " has already been defined.");
    }
    onExit();
}


void Interpreter::doPrintStatement(ASTNode* node) {
    onEnter("[PRINT]");
    if (node->type.expr == CONST_STR) //I mean, thats WHY its a const string, right?
        cout<<node->attribute.name;
    else
        cout<<interpretExpression(node).toString();
    onExit();
}

void Interpreter::doReadStatement(ASTNode* node) {
    onEnter("[READ]");
    string input;
    getline(cin, input);
    int addr = variables[node->attribute.name];
    Object val = memStore.get(addr);
    val.data._value = input;
    memStore.store(addr, val);
    onExit();
}

void Interpreter::handleIfStatement(ASTNode* node) {
    int res = interpretExpression(node->child[0]).data.intValue();
    if (res) {
        say("passed test");
        auto t = node->child[1];
        while (t) {
            interpretStatement(t);
            t = t->sibling;
        }
    } else {
        if (node->child[2]) {
            say("else clause");
            auto t = node->child[2];
            while (t) {
                interpretStatement(t);
                t = t->sibling;
            }
        }
    }
}

void Interpreter::handleWhileStatement(ASTNode* node) {
    onEnter("While Loop");
    say("Check loop condition");
    Object ret = Object(interpretExpression(node->child[0]).data.intValue());
    while (ret.data.intValue()) {
        say("Execute Body: ");
        Execute(node->child[1]);
        say("check loop condition: ");
        ret = eval(node->child[0]);
        say("Result: " + ret.toString());
    }
    onExit("Leaving While Statement");
}

void Interpreter::doReturnStatement(ASTNode* node) {
    onEnter("Return Statement.");
    Object retVal =  interpretExpression(node->child[0]);
    callStack.top()->returnVal = retVal;
    say(retVal.toString() + " saved as return value.");
    onExit();
}


StackFrame* Interpreter::prepStackFrame(StackFrame* masterFrame) {
    StackFrame* nextFrame = new StackFrame;
    nextFrame->body = masterFrame->body;
    nextFrame->params = masterFrame->params;
    nextFrame->returnVal = Object(0);
    for (auto m : masterFrame->symbolTable) {
        nextFrame->symbolTable[m.first].addr = memStore.allocate(1);
    }
    return nextFrame;
}

void Interpreter::assignLocalSymbols(ASTNode* node, StackFrame* nsf) {
    auto paramIt = nsf->params;
    auto argIt = node->child[1];
    //now we want to assign the parameters to their correct symbol tables.
    while (paramIt && argIt) {
        if (paramIt->attribute.type == as_ref) {
            if (!callStack.empty()) {
                if (callStack.top()->symbolTable.find(paramIt->attribute.name) != callStack.top()->symbolTable.end()) {
                    nsf->symbolTable[paramIt->attribute.name].addr = callStack.top()->symbolTable[paramIt->attribute.name].addr;
                } else if (variables.find(paramIt->attribute.name) != variables.end()) {
                    nsf->symbolTable[paramIt->attribute.name].addr = variables[paramIt->attribute.name]; 
                } else {
                    logError("No such variable " + paramIt->attribute.name);
                }
            } else {
                if (variables.find(paramIt->attribute.name) != variables.end()) {
                    nsf->symbolTable[paramIt->attribute.name].addr = variables[paramIt->attribute.name]; 
                } else {
                    logError("No such variable " + paramIt->attribute.name);
                }
            }
        }
        Object obj = interpretExpression(argIt);
        if (paramIt->attribute.type == as_ref)
            nsf->symbolTable[paramIt->attribute.name].isRef = true;
        else
            nsf->symbolTable[paramIt->attribute.name].isRef = false;
        memStore.store(nsf->symbolTable[paramIt->attribute.name].addr, obj);
        paramIt = paramIt->sibling;
        argIt = argIt->sibling;
    }
}

Object Interpreter::Dispatch(ASTNode* node, StackFrame* nsf) {
    say("Dispatch: " + node->attribute.name);
    Object retVal;
    assignLocalSymbols(node, nsf);
    callStack.push(nsf);
    Execute(callStack.top()->body);
    retVal = callStack.top()->returnVal;
    for (auto addr : callStack.top()->symbolTable) {
        if (addr.second.isRef == false)
            memStore.free(addr.second.addr);
    }
    callStack.pop();
    say("value returned: " + retVal.toString());
    return retVal;
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
            declareVariable(node);
            break;
        case FUNCDECL:
            declareFunction(node);
            break;
        case PRINTSTM:
            doPrintStatement(node->child[0]);
            break;
        case READSTM:
            doReadStatement(node->child[0]);
            break;
        case IFSTM:
            handleIfStatement(node); 
            break;
        case WHILESTM:
            handleWhileStatement(node);
            break;
        case EXPRSTM:
            interpretExprStatement(node);
            break;
        case ASSIGNSTM:
            storeToMemoryByName(node->child[0]);
            break;
        case RETURNSTM:
            doReturnStatement(node);
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
        case DIRECTIVE:
            compilerDirective(node);
    }
    if (node->kind == STMTNODE && node->type.stmt == RETURNSTM)
        return;
    Execute(node->sibling);
}

void Interpreter::compilerDirective(ASTNode* node) {
    onEnter("Compiler Directive.");
    if (node->type.drctv == IMPORT_DIRECTIVE) {
        importLibrary(node->attribute.name);
    }
    onExit();
}

void Interpreter::importLibrary(string libName) {
    onEnter("Importing " + libName);
    ASTNode* libAst;
    ASTBuilder libBuilder;
    libAst = libBuilder.build("owlcode/" + libName + ".owl");
    Execute(libAst);
}