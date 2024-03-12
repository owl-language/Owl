#ifndef shell_hpp
#define shell_hpp
#include <iostream>
#include "../ast/ast.hpp"
#include "../lexer/lexer.hpp"
#include "../parser/parser.hpp"
#include "../tokens/tokens.hpp"
#include "../vm/interpreter.hpp"
#include "../tools/tracer.hpp"
using namespace std;

class OwlShell {
    private:
        OwlLexer lexer;  //Lexer Returns ts
        vector<Token> ts;
        OwlParser parser; //Parser returns ast
        ASTNode* ast;
        Interpreter interpreter; //take a wild guess
        vector<string> tokenizeInput(string input);
        void showHelp();
        void resetEnv();
    public:
        OwlShell();
        void start();
        vector<string> promptInput();
        void parseCommand(vector<string>& tokens);
};

#endif