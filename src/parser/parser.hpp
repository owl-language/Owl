#pragma once
#ifndef parser_hpp
#define parser_hpp
#include <iostream>
#include <vector>
#include "../ast/ast.hpp"
#include "../tokens/tokens.hpp"
#include "../tools/tracer.hpp"
using std::cout;
using std::endl;
using std::string;
using std::to_string;
using std::stof;
using std::vector;

class OwlParser {
    private:
        using TokenStream = vector<Token>::iterator;
        vector<Token> tokenvector;
        TokenStream ts;
        Token currentToken;
        int stumbleCount;
        Token& lookahead();
        void nexttoken();
        void reSync();
        bool match(TokenType token);
        void initStream(vector<Token>& tokens);
    public:
        OwlParser();
        ASTNode* start(vector<Token>& ts);
        ASTNode* replParse(vector<Token>& ts);
    private:
        ASTNode* block();
        ASTNode* statementList();
        ASTNode* statement();
        ASTNode* declareVarStatement();
        ASTNode* declareProcedureStatement();
        ASTNode* declareArrayStatement();
        ASTNode* declareRecordType();
        ASTNode* initRecord();
        ASTNode* returnStatement();
        ASTNode* whileStatement();
        ASTNode* ifStatement();
        ASTNode* assignStatement();
        ASTNode* printStatement();
        ASTNode* readStatement();
        ASTNode* exprStatement();
        ASTNode* procedureCall();
        ASTNode* argumentList();
        ASTNode* parameterList();
        ASTNode* expression();
        ASTNode* simpleExpression();
        ASTNode* randExpression();
        ASTNode* typeName();
        ASTNode* term();
        ASTNode* factor();
        ASTNode* var();
        ASTNode* strValue();
        ASTNode* program();
        ASTNode* library();
};

#endif