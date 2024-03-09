#include <iostream>
#include <map>
#include <cstring>
#include "../interpreter/interpreter.hpp"
#include "../ast/ast.hpp"
#include "../parser/parser.hpp"
#include "../lexer/lexer.hpp"
using namespace std;

void showUsage() {
    cout<<"[ Owl v"<<OWL_VERSION<<" "<<FLACO<<"]"<<endl;
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
    vector<Token> ts;
    ASTNode* ast;
    OwlLexer lexer;
    Parser mp;
    Interpreter interpreter;
    if (argv[1][0] == '-' && argv[1][1] == 'v') {
        initTracer(argv[1]);
        setTraceState(PARSE);
        ts = lexer.tokenize(argv[2]);
        for (Token tok : ts) {
            tok.print();
        }
        ast = mp.start(ts);
        printTree(ast);
    } else {
        setTraceState(PARSE);
        ts = lexer.tokenize(argv[1]);
        ast = mp.start(ts);
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