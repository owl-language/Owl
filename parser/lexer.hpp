#pragma once
#ifndef lexer_hpp
#define lexer_hpp
#include <iostream>
#include <vector>
#include <unordered_map>
#include "../tokens/tokens.hpp"
#include "../tools/filebuffer.hpp"
#include "../tools/tracer.hpp"
using namespace std;

class OwlLexer {
    private:
        SourceBuffer sb;
        TokenType handleKeywordOrId(string word);
        Token handleSpecials();
        Token extractFullNumber();
        string extractFullWord();
        vector<Token> tokenize();
        bool inComment;
        bool inString;
        void initReserved();
        void processString(vector<Token>& tokenList);
        unordered_map<string, TokenType> reserved;
    public:
        OwlLexer() {
            inComment = false;
            initReserved();
        }
        vector<Token> tokenize(string filename) {
            sb.loadFile(filename);
            return tokenize();
        }
        vector<Token> tokenizeStatement(string statement) {
            sb.loadString(statement);
            return tokenize();
        }
};


#endif