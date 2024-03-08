#include <iostream>
#include <map>
#include <cstring>
#include "../interpreter/interpreter.hpp"
#include "../ast/ast.hpp"
#include "../parser/parser.hpp"
#include "../lexer/lexer.hpp"
using namespace std;

int main(int argc, char* argv[]) {
    srand(time(0));
    if (argc < 2) {
        cout<<"Useage: owlcli <filename> "<<endl;
        return 0;
    }
    vector<Token> ts;
    ASTNode* ast;
    OwlLexer lexer;
    Parser mp;
    Interpreter interpreter;
    if (argv[1][0] == '-' && argv[1][1] == 'v') {
        initTracer(argv[1]);
        ts = lexer.tokenize(argv[2]);
        for (Token tok : ts) {
            tok.print();
        }
        ast = mp.start(ts);
        printTree(ast);
    } else {
        ts = lexer.tokenize(argv[1]);
        ast = mp.start(ts);
    }
    printToLog("[Parsing Complete.]");
    traceAST(ast);
    printToLog("[Running Program]");
    interpreter.Execute(ast);
    freeTree(ast);
    endTrace();
    return 0;
}