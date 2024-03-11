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
    public:
        OwlREPL();
        void repl();
};

#endif