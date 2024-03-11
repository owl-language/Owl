#ifndef ast_builder_hpp
#define ast_builder_hpp
#include <iostream>
#include <vector>
#include "../ast/ast.hpp"
#include "../lexer/lexer.hpp"
#include "../parser/parser.hpp"
using namespace std;

class ASTBuilder {
    private:
        OwlLexer lexer;
        OwlParser parser;
        ASTNode* result;
    public:
        ASTBuilder() {
            
        }
        ASTNode* build(string fileName) {
             auto ts = lexer.tokenize(fileName);
             result = parser.start(ts);
             return result;
        }
        ASTNode* translate(string input) {
            auto ts = lexer.tokenizeStatement(input);
            result = parser.replParse(ts);
            return result;
        }
};

#endif