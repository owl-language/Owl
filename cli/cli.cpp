#include "cli.hpp"

void OwlCLI::runVerbose(string argString, string fileName) {
    initTracer(argString);
    setTraceState(PARSE);
    ast = astBuilder.build(fileName);
    printTree(ast);
    execute();
}

void OwlCLI::runQuiet(string fileName) {
    setTraceState(PARSE);
    ast = astBuilder.build(fileName);
    execute();
}

void OwlCLI::execute() {
    printToLog("[Parsing Complete.]");
    traceAST(ast);
    printToLog("[Running Program]");
    setTraceState(INTERP);
    interpreter.Execute(ast);
    freeTree(ast);
    endTrace();
}