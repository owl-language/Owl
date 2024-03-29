#include "interpreter.hpp"


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
        
    if (leftChild.type == STRING || rightChild.type == STRING) {
        retObj = evaluator.stringOp(x->attribute.op, leftChild, rightChild);
    } else {
        if (x->attribute.op == PLUS || x->attribute.op == MINUS || 
            x->attribute.op == MULT || x->attribute.op == DIVD) {
            retObj = evaluator.mathOp(x->attribute.op, leftChild, rightChild);    
        }
        if (x->attribute.op == EQUAL || x->attribute.op == NOTEQUAL || x->attribute.op == LESS || 
            x->attribute.op == GREATER || x->attribute.op == LESSEQ || x->attribute.op == GREATEREQ) {
            retObj = evaluator.relOp(x->attribute.op, leftOperand, rightOperand);
        }
    }
    say("eval result: " + retObj.toString());
    onExit();
    return retObj;
}

int Interpreter::resolveNameToAddress(string varname) {
    int addr = 0;
    if (callStack.size() && callStack.top()->symbolTable.find(varname) != callStack.top()->symbolTable.end()) {
        return callStack.top()->symbolTable[varname].addr;
    }
    if (callStack.size() > 1 ) {
        if (callStack.top()->staticLink->symbolTable.find(varname) != callStack.top()->staticLink->symbolTable.end()) {
            return callStack.top()->staticLink->symbolTable[varname].addr;
        }
    }
    if (variables.find(varname) != variables.end()) {
        return variables[varname];
    } 
    logError("Hoot! couldnt find: " + varname);
    return addr;
}

int Interpreter::calculateArrayIndex(ASTNode* x) {
    int offset = 0;
    say("Calculating Offset.");
    offset = interpretExpression(x).data.intValue();
    if (offset < 0) {
        logError("Hoot! Invalid Index: " + to_string(offset));
        return 0;
    }
    say("Array Reference, offset: " + to_string(offset));
    return offset;
}

Object Interpreter::resolveRecordFieldName(ASTNode* x, Record* rec) {
    Object retVal;
    if (x->child[2] && checkExprNodeType(x->child[2], ID_EXPR)) {
        string fieldName = x->child[2]->attribute.name;
        say("field: " + fieldName);
        if (rec->fieldAddrs.find(fieldName) != rec->fieldAddrs.end()) {
            int nextAddr = rec->fieldAddrs[fieldName];
            retVal = memStore.get(nextAddr);
            say("address: " + to_string(nextAddr) + ", value: " + retVal.data._value);
        } else {
            logError("Invalid field name specified");
        }
    } else {
        say("Returning instance.");
    }
    return retVal;
}


Object Interpreter::retrieveFromMemoryByName(ASTNode* x) {
    onEnter("retrieveFromMemoryByName");
    Object retVal;
    int offset = 0, addr = resolveNameToAddress(x->attribute.name);
    if (x->kind == EXPRNODE && checkExprNodeType(x, SUBSCRIPT_EXPR)) {
        offset = calculateArrayIndex(x->child[0]);
        if (offset > 0 && offset > memStore.get(addr).attr.size) {
            logError("Hoot! Index " + to_string(offset) + " out of range for array " + x->attribute.name + " of size " + to_string(memStore.get(addr).attr.size) + + " at address " + to_string(addr));
            onExit();
            return retVal;
        }
    }
    retVal = memStore.get(addr + offset);
    say("ID: " + x->attribute.name + ", Address: " + to_string(addr) + ", offset: " +to_string(offset) + ", value: " + retVal.toString() + ", type: " + rtTypeAsStr[retVal.type]);
    if (retVal.type == HASH && x->child[2]) {
        retVal = resolveRecordFieldName(x, retVal.data.recordValue());
    }
    onExit();
    return retVal;
}

void Interpreter::storeToMemoryByName(ASTNode* x) {
    onEnter("storeToMemoryByName ");
    string varname = x->child[0]->attribute.name; //variable name
    string fieldName;
    say("Assign to " + varname + ": ");
    Object valToAssign = interpretExpression(x->child[1]);   //value to assign
    int offset = 0, addr = 0;
    addr = resolveNameToAddress(varname);
    if (x->child[0]->child[2] && memStore.get(addr).type == HASH) {
        fieldName = x->child[0]->child[2]->attribute.name;
        Record* ht = memStore.get(addr).data.recordValue();
        say("field: " + fieldName);
        if (ht->fieldAddrs.find(fieldName) != ht->fieldAddrs.end())
            addr = ht->fieldAddrs[fieldName];
        else {
            logError("Invalid field for record: " + fieldName);
        }
    }
    if (x->child[0]->kind == EXPRNODE && checkExprNodeType(x->child[0], SUBSCRIPT_EXPR)) {
        offset = calculateArrayIndex(x->child[0]->child[0]);
        if (offset > 0 && offset > memStore.get(addr).attr.size) {
            logError("Hoot! Index " + to_string(offset) + " out of range for array " + varname + " of size " + to_string(memStore.get(addr).attr.size) + + " at address " + to_string(addr));
            onExit();
            return;
        }
    }
    memStore.store(addr + offset, valToAssign);
    onExit("storeToMemoryByName ");
}

Object Interpreter::doProcedureCall(ASTNode* x) {
    Object retVal(0);
    if (!callStack.empty() && callStack.top()->nestedProcedures.find(x->attribute.name) != callStack.top()->nestedProcedures.end()) {
        say("nested procedure.");
        return Dispatch(x, prepStackFrame(callStack.top()->nestedProcedures[x->attribute.name]));
    } else if (procedures.find(x->attribute.name) != procedures.end()) {
        say("global procedure");
        return Dispatch(x, prepStackFrame(procedures[x->attribute.name]));
    } else {
        logError("No such procedure: " + x->attribute.name);
    }
    return retVal;
}

Object Interpreter::initializeRecord(ASTNode* x) {
    onEnter("Initializing Record");
    string anotatedRecName = x->attribute.name;
    string recName, instanceName;
    int i;
    Object retVal;
    for (i = 0; anotatedRecName[i] != ' '; i++)
        recName.push_back(anotatedRecName[i]);
    while (i < anotatedRecName.size()) {
        if (isalpha(anotatedRecName[i]))
            instanceName.push_back(anotatedRecName[i]);
        i++;
    }
    if (recordDefinitions.find(recName) == recordDefinitions.end()) {
        logError("No Record definition found with name: " + recName);
        return retVal;
    } 
    Record* recDef = recordDefinitions[recName];
    Record* nRec = new Record(instanceName);
    for (auto entries : recDef->fieldAddrs) {
        nRec->fieldAddrs[entries.first] = memStore.allocate(1);
    }
    say("Record " + instanceName + " of type " + recName + " instantiated");
    recordInstances.push_back(nRec);
    retVal = Object(recordInstances.back());
    int addr = memStore.storeAtNextFree(retVal);
    variables[instanceName] = addr;
    say("Stored at address: " + to_string(variables[instanceName]));
    onExit();
    return retVal;
}

Object Interpreter::interpretExpression(ASTNode* x) {
    Object retVal; int rbound = 1, addr = 0;
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
            retVal =  Object(rand() % (rbound - 1) + 1);
            return retVal;
        case SUBSCRIPT_EXPR:
        case ID_EXPR:
            return retrieveFromMemoryByName(x);
        case PROCDCALL:
            onExit();
            return doProcedureCall(x);
        case OP_EXPR:
            retVal = eval(x);
            if (x->attribute.op == LESS || x->attribute.op == EQUAL || x->attribute.op == NOTEQUAL) {
                result = (retVal.data.intValue() == 0) ? "false":"true";
                msg = "value: " + result;
                say(msg);
            } else
                say("value: " + retVal.toString());
            return retVal;
        case INITREC_EXPR:
            retVal = initializeRecord(x->child[0]);
            msg = retVal.data.recordValue()->name;
            addr = variables[msg];
            say("record: " + msg + " initialized at address" + to_string(addr));
            break;
        default:
            break;
    }
    onExit();
    return retVal;
}

void Interpreter::declareVariable(ASTNode* x) {
    string name;
    int addr;
    Object obj;
    if (checkExprNodeType(x->child[0], SUBSCRIPT_EXPR)) {
        ASTNode* child = x->child[0];
        name = child->attribute.name;
        int size = child->child[0]->attribute.intValue;
        addr = memStore.allocate(size);
        say("Declaring Array: " + name + " of size " + to_string(size) + " at address " + to_string(addr));
    } else if (checkExprNodeType(x->child[0], INITREC_EXPR)) {
        obj = initializeRecord(x->child[0]);
        if (obj.type == HASH) {
            name = obj.data.recordValue()->name;
            addr = variables[name];
            say("record: " + name + " initialized.");
        } else {
            logError("Record Initialization failed.");
        }
    } else {
        name = x->child[0]->attribute.name;
        obj = interpretExpression(x->child[1]);
        addr = memStore.storeAtNextFree(obj);
        say("Declaring Variable: " + name + " with value " + obj.toString() + " type: " + rtTypeAsStr[obj.type]);
    }
    if (callStack.empty())  variables[name] = addr;
    else callStack.top()->symbolTable[name].addr = addr;
}

void Interpreter::declareStruct(ASTNode* x) {
    string recName;
    if (x->child[0]) {
        recName = x->child[0]->attribute.name;
    }
    Record* recordDef = new Record(recName);
    for (ASTNode* it = x->child[0]->sibling; it != nullptr; it = it->sibling) {
        string fieldname = it->child[0]->attribute.name;
        recordDef->fieldAddrs[fieldname] = 0;
    } 
    recordDefinitions[recName] = recordDef;
    say("Declared Record " + recName + " with " + to_string(recordDef->fieldAddrs.size()) + " fields");
}

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
    if (checkExprNodeType(node, CONST_STR)) {
        cout<<node->attribute.name;
    } else {
        cout<<interpretExpression(node).toString();
    }
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
        case RECDECL:
            declareStruct(node);
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
            storeToMemoryByName(node);
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
            break;
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
    libAst = libBuilder.build("./" + libName + ".owl");
    Execute(libAst);
}

bool Interpreter::checkExprNodeType(ASTNode* x, ExprKind nodeType) {
    return x->type.expr == nodeType;
}

bool Interpreter::checkStmtNodeType(ASTNode* x, StmtKind nodeType) {
    return x->type.stmt == nodeType;
}