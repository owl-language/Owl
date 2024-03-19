#ifndef ast_builder_hpp
#define ast_builder_hpp
#include <iostream>
#include <vector>
#include "../ast/ast.hpp"
#include "../lexer/lexer.hpp"
#include "../parser/parser.hpp"
using std::string;

// This was originally implemented to ease parsing individual statements
// entered into the repl. I do believe with some modifications that
// something similar will also work for implementing an eval() function/lambdas.
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
            //lets get meta-circular (dark-wing duck!)
            auto ts = lexer.tokenizeStatement(input);
            result = parser.replParse(ts);
            return result;
        }
};

#endif