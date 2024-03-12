#include "shell.hpp"
#include <iostream>
#include <vector>
using namespace std;

OwlShell::OwlShell() {

}

void OwlShell::start() {
    vector<string> input;
    for (;;) {
        input = promptInput();
        parseCommand(input);
    }
}

vector<string> OwlShell::promptInput() {
    string input;
    cout<<"Owl> ";
    getline(cin, input);
    return tokenizeInput(input);
}

void OwlShell::resetEnv() {
    if (!ts.empty())
        ts.clear();
    if (ast != nullptr)
        freeTree(ast);
    interpreter.reset();
}

void OwlShell::showHelp() {
    cout<<"OwlShell Interactive Interpreter"<<endl;
    cout<<"-----------------------------"<<endl;
    cout<<"  Command       Description"<<endl;
    cout<<"  load <path>    load a .owl script into Owlsh."<<endl;
    cout<<"  run            execute the current Owl script."<<endl;
    cout<<"  list tokens    display all tokens from current Owl script."<<endl;
    cout<<"  list ast       display the AST of current Owl script."<<endl;
    cout<<"  list libfuncs  display any loaded library procedure's ASTs"<<endl;
    cout<<"  reset          Places OwlShell into its initial state, "<<endl;
    cout<<"                 Unloads any scripts and clears OwlVM memory."<<endl;
    cout<<"  spy memory     display memory usage in OwlVM."<<endl;
    cout<<"  help, ?        display this menu."<<endl;
    cout<<"  quit, exit     Close OwlShell."<<endl;
}

void OwlShell::parseCommand(vector<string>& tokens) {
    if (tokens[0] == "quit" || tokens[0] == "exit") {
        exit(0);
    } else if (tokens[0] == "load") {
        ts = lexer.tokenize(tokens[1]);
        ast = parser.start(ts);
    } else if (tokens[0] == "list") {
        if (tokens[1] == "tokens") {
            printTokens(ts);
        } else if (tokens[1] == "ast") {
            if (ast != nullptr) printTree(ast);
        }
    } else if (tokens[0] == "run") {
        interpreter.Execute(ast);
    } else if (tokens[0] == "reset") {
        resetEnv();
    } else if (tokens[0] == "spy") {
        if (tokens[1] == "memory") {
            interpreter.memoryUsage();
        }
    } else if (tokens[0] == "?" || tokens[0] == "help") {
        showHelp();
    } else {
        cout<<"? "<<tokens[0]<<endl;
    }
}

vector<string> OwlShell::tokenizeInput(string input) {
    vector<string> words;
    string word;
    for (int i = 0; i < input.length(); i++) {
        if (input[i] == ' ') {
            words.push_back(word);
            word.clear();
        } else {
            word.push_back(input[i]);
        }
    }
    words.push_back(word);
    return words;
}