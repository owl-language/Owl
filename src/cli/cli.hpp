#ifndef cli_hpp
#define cli_hpp
#include "../vm/ast_builder.hpp"
#include "../vm/interpreter.hpp"
#include "../ast/ast.hpp"
#include "../tools/tracer.hpp"
#include "../lexer/lexer.hpp"
#include "../parser/parser.hpp"


class OwlCLI {
    private:
        ASTBuilder astBuilder;
        ASTNode* ast;
        Interpreter interpreter;
        void execute();
    public:
        void runVerbose(string args, string fileName);
        void runQuiet(string filename);
};

#endif