#pragma once
#ifndef lexer_hpp
#define lexer_hpp
#include <iostream>
#include <vector>
#include <unordered_map>
#include "../tokens/tokens.hpp"
#include "../tools/filebuffer.hpp"
#include "../tools/tracer.hpp"
using std::string;
using std::cout;
using std::endl;
using std::to_string;
using std::stof;
using std::vector;
using std::unordered_map;

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
        OwlLexer();
        vector<Token> tokenize(string filename);
        vector<Token> tokenizeStatement(string statement);
};


#endif