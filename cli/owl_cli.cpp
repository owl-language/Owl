#include <iostream>
#include <map>
#include <cstring>
#include "../interpreter/interpreter.hpp"
#include "../ast/ast.hpp"
#include "../tools/tracer.hpp"
#include "../lexer/lexer.hpp"
#include "../parser/parser.hpp"
using namespace std;

void showUsage() {
    cout<<"[ Owl v"<<OWL_VERSION<<" "<<FLACO<<" ]"<<endl;
    cout<<"Useage: "<<endl;
    cout<<"      owlcli <options> <filename> "<<endl;
    cout<<"Optional Flags: "<<endl;
    cout<<"       -v   display tokens and generated AST"<<endl;
    cout<<"       -vp  include trace of parse step"<<endl;
    cout<<"       -vi  show trace of interpreter"<<endl;
    cout<<"       -vip show all of the above"<<endl;
}

int main(int argc, char* argv[]) {
    srand(time(0));
    if (argc < 2) {
        showUsage();
        return 0;
    }
    ASTBuilder astBuilder;
    ASTNode* ast;
    Interpreter interpreter;
    if (argv[1][0] == '-' && argv[1][1] == 'v') {
        initTracer(argv[1]);
        setTraceState(PARSE);
        string file = argv[2];
        ast = astBuilder.build(file);
        printTree(ast);
    } else {
        setTraceState(PARSE);
        ast = astBuilder.build(argv[1]);
    }
    printToLog("[Parsing Complete.]");
    traceAST(ast);
    printToLog("[Running Program]");
    setTraceState(INTERP);
    interpreter.Execute(ast);
    freeTree(ast);
    endTrace();
    return 0;
}