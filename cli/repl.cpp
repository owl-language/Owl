#include "repl.hpp"
#include <iostream>
using namespace std;

void OwlREPL::clear_screen() {
    for (int i = 0; i < 40; i++) {
        cout<<"\n";
    }
    cout<<endl;
}

void OwlREPL::repl() {
    cout<<"[Owl REPL v0.1 (\\^(OvO)^/)]"<<endl;
    Interpreter interpreter;
    ASTBuilder astbuiler;
    string input;
    while (input != "exit") {
        cout<<"Owl> ";
        getline(cin, input);
        if (input == "exit" || input == "quit") {
            break;
        } else if (input == "clear") {
            clear_screen();
        }
        if (!input.empty()) {
            cout<<"-> "<<input<<endl;
            auto ast = astbuiler.translate(input);
            printTree(ast);
            interpreter.Execute(ast);
        }
    }
}