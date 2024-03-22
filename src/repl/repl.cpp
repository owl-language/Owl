#include "repl.hpp"
#include <iostream>
using namespace std;

OwlREPL::OwlREPL() {
    
}

void OwlREPL::clear_screen() {
    for (int i = 0; i < 40; i++) {
        cout<<"\n";
    }
    cout<<endl;
}

void OwlREPL::repl() {
    cout<<"[Owl REPL v0.1 (\\^(OvO)^/)]"<<endl;
    string input;
    while (input != "exit") {
        cout<<"Owl> ";
        getline(cin, input);
        if (input == "exit" || input == "quit") {
            break;
        } else if (input == "clear") {
            clear_screen();
        } else if (input == ".ast") {
            printTree(ast);
        } else if (input == ".memory") {
            interpreter.memoryUsage();
        } else {
            parseInput(input);
        }
    }
}

void OwlREPL::parseInput(string input) {
     if (!input.empty()) {
        cout<<"-> "<<input<<endl;
        ast = astBuilder.translate(input);
        bool newline = false;
        printTree(ast);
        if (ast->type.stmt == PRINTSTM) {
            newline = true;
        }
        if (newline) {
            cout<<endl;
        }
        interpreter.Execute(ast);
    }
}