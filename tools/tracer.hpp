#pragma once
#ifndef tracer_hpp
#define tracer_hpp
#include <iostream>
#include <fstream>
#include <unordered_set>
#include "../ast/ast.hpp"
using namespace std;
inline const string OWL_VERSION = "0.1a";
inline const string FLACO = "(\\^(OvO)^/)";
inline fstream logfile;
enum TracerStates {
    INTERP, PARSE, PARSE_AND_INTERP, OFF
};
inline TracerStates TRACE_STATE;
inline int depth;
inline unordered_set<TracerStates> shouldTrace;

void initTracer(string lstr);

void printToLog(string s);

void logError(string s);

void endTrace();

void setTraceState(TracerStates ts);

TracerStates getTracerState();

void say(string s);

void onEnter(string s);

void onExit(string s);

void onExit();

void traceNode(ASTNode* node);

void traceTree(ASTNode* node);

void traceAST(ASTNode* node);

#endif