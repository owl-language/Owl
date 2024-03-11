#pragma once
#ifndef parser_hpp
#define parser_hpp
#include <iostream>
#include <vector>
#include "../ast/ast.hpp"
#include "../tokens/tokens.hpp"
#include "../tools/tracer.hpp"
using namespace std;

class OwlParser {
    private:
        using TokenStream = vector<Token>::iterator;
        vector<Token> tokenvector;
        TokenStream ts;
        Token currentToken;
        int stumbleCount;
        Token& lookahead() {
            return *ts;
        }
        void nexttoken() {
            std::advance(ts,1);
            if (ts != tokenvector.end()) {
                currentToken = *ts;
            } else {
                say("Token Stream Consumed.");
                std::advance(ts, -1);
            }
        }
        void reSync() {
            if (stumbleCount > 2) {
                logError("Too many errors during parsing, bailing out.");
                exit(0);
            }
            stumbleCount++;
        }
        bool match(TokenType token) {
            if (lookahead().tokenval == token) {
                say("Match: " + currentToken.stringval);
                nexttoken();
                return true;
            }
            cout<<"Mismatched Token on line "<<lookahead().lineno<<": "<<lookahead().stringval<<endl;
            cout<<"Expected: "<<tokenString[token]<<" but found "<<tokenString[lookahead().tokenval]<<endl;
            stumbleCount++;
            return false;
        }
        void initStream(vector<Token>& tokens) {
            tokenvector = tokens;
            ts = tokenvector.begin();
            currentToken = *ts;
        }
    public:
        OwlParser() {

        }
        ASTNode* start(vector<Token>& ts);
        ASTNode* replParse(vector<Token>& ts);
    private:
        ASTNode* block();
        ASTNode* statementList();
        ASTNode* statement();
        ASTNode* declareVarStatement();
        ASTNode* declareProcedureStatement();
        ASTNode* declareArrayStatement();
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