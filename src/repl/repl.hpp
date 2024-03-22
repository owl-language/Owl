#ifndef repl_hpp
#define repl_hpp
#include "../ast/ast.hpp"
#include "../interpreter/ast_builder.hpp"
#include "../interpreter/interpreter.hpp"
#include <iostream>
using std::cout;
using std::endl;
using std::string;


class OwlREPL {
    private:
        void clear_screen();
        ASTNode* ast;
        ASTBuilder astBuilder;
        Interpreter interpreter;
        void parseInput(string input);
    public:
        OwlREPL();
        void repl();
};

#endif