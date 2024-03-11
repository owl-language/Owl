#include <iostream>
#include "../ast/ast.hpp"
#include "../interpreter/ast_builder.hpp"
#include "../interpreter/interpreter.hpp"
using namespace std;

int main() {
    cout<<"[Owl REPL v0.1 (\\^(OvO)^/)]"<<endl;
    Interpreter interpreter;
    ASTBuilder astbuiler;
    string input;
    while (input != "exit") {
        cout<<"Owl> ";
        getline(cin, input);
        if (!input.empty() && input != "exit") {
            cout<<"-> "<<input<<endl;
            auto ast = astbuiler.translate(input);
            printTree(ast);
            interpreter.Execute(ast);
        }
    }
    cout<<"\nHoot!"<<endl;
    return 0;
}