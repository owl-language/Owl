#ifndef tracer_hpp
#define tracer_hpp
#include <iostream>
#include <fstream>
#include <unordered_set>
using namespace std;
const string OWL_VERSION = "0.1a";
fstream logfile;
enum TracerStates {
    INTERP, PARSE, PARSE_AND_INTERP, OFF
};

unordered_set<TracerStates> shouldTrace;

void initTracer(char *lstr) {
    logfile.open(".runlog.tree", ios::out);
    if (!logfile.is_open()) {
        cout<<"Couldn't open logfile."<<endl;
        exit(0);
    }
    logfile<<"[Owl Interpreter "<<OWL_VERSION<<" started.]"<<endl;
    if (lstr[0] == '-' && lstr[1] == 'v') {
        if (lstr[2] == 'i') {
            shouldTrace.insert(INTERP);
        } else if (lstr[2] == 'p') {
                shouldTrace.insert(PARSE);
        } else if (lstr[2] == 'a') {
            shouldTrace.insert(INTERP);
            shouldTrace.insert(PARSE);
        }
    }
}

void printToLog(string s) {
    if (logfile.is_open()) {
        logfile<<s<<endl;
    }
}

void logError(string s) {
    cout<<s<<endl;
    if (logfile.is_open()) {
        logfile<<s<<endl;
    }
}

void endTrace() {
    if (logfile.is_open()) {
        logfile<<endl<<"Trace Complete."<<endl;
        logfile.close();
    }
}

TracerStates TRACE_STATE;
int depth;

void setTraceState(TracerStates ts) {
    TRACE_STATE = ts;
}

TracerStates getTracerState() {
    return TRACE_STATE;
}

void say(string s) {
    if (shouldTrace.find(TRACE_STATE) != shouldTrace.end()) {
        for (int i = 0; i < depth; i++) {
            cout<<" ";
            logfile<<"  ";
        }
        string msg = "(" + to_string(depth) + ") " + s;
        printToLog(msg);
        cout<<msg<<endl;
    }
}

void onEnter(string s) {
    ++depth;
    say(s);
}

void onExit(string s) {
    say(s);
    --depth;
}

void onExit() {
    --depth;
}

void traceNode(ASTNode* node) {
    if (node->kind == EXPRNODE) {
        logfile<<"["<<ExprKindStr[node->type.expr]<<"] "<<node->attribute.name<<" ";
        if (node->type.expr == OP_EXPR) 
            logfile<<" - "<<tokenString[node->attribute.op];
        logfile<<endl;
    } else {
        logfile<<"["<<StmtKindStr[node->type.stmt]<<"] "<<node->attribute.name<<endl;
    }
}

void traceTree(ASTNode* node) {
    ++depth;
    if (node != nullptr) {
        for (int i = 0; i < depth; i++) 
            logfile<<"  ";
        traceNode(node);
        for (int i = 0; i < MAX_CHILDREN; i++)
            traceTree(node->child[i]);
        
        if (node->sibling) --depth;
        traceTree(node->sibling);
    }
    if (depth > 0)
        --depth;
}

void traceAST(ASTNode* node) {
    
    printToLog("Abstract Syntax Tree:");
    printToLog("---------------------");
    depth = 0;
    traceTree(node);
}


#endif