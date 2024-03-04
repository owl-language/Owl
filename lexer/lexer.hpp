#ifndef lexer_hpp
#define lexer_hpp
#include <iostream>
#include <vector>
#include <unordered_map>
#include "../tokens/tokens.hpp"
#include "filebuffer.hpp"
using namespace std;

class OwlLexer {
    private:
        SourceBuffer sb;
        TokenType handleKeywordOrId(string word);
        Token handleSpecials();
        int extractFullNumber();
        string extractFullWord();
        vector<Token> tokenize();
        bool inComment;
        bool inString;
        void initReserved();
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
        
};

void OwlLexer::initReserved() {
    reserved["begin"] = BEGIN;
    reserved["end"]  = END;
    reserved["read"] = READ;
    reserved["print"] = PRINT;
    reserved["input"] = READ;
    reserved["if"] = IF;
    reserved["then"] = THEN;
    reserved["else"] = ELSE;
    reserved["func"] = FUNC;
    reserved["while"] = WHILE;
    reserved["let"] = LET;
    reserved["int"] = INT;
    reserved["char"] = CHAR;
    reserved["program"] = PROG;
    reserved["return"] = RETURN;
    reserved["array"] = ARRAY;
    reserved["string"] = STR;
    reserved["rand"] = RAND;
}

TokenType OwlLexer::handleKeywordOrId(string word) {
    if (reserved.find(word) != reserved.end()) {
        return reserved.at(word);
    } else {
        return ID;
    }
    return ERROR;
}

Token OwlLexer::handleSpecials() {
    Token nextToken;
    if (sb.Char() == ':') {
        sb.GetChar();
        if (sb.Char() == '=') {
            return Token(ASSIGN, ":=", -1, sb.lineNumber());
        }
        sb.UnGetChar();
        return Token(COLON, ":", -1, sb.lineNumber());
    } else if (sb.Char() == '!') {
        sb.GetChar();
        if (sb.Char() == '=') {
            return Token(NOTEQUAL, "!=", -1, sb.lineNumber());
        }
        sb.UnGetChar();
        return Token(NOT, "!", -1, sb.lineNumber());
    } else if (sb.Char() == '<') {
        sb.GetChar();
        if (sb.Char() == '=') {
            return Token(LESSEQ, "<=", -1, sb.lineNumber());
        }
        sb.UnGetChar();
        return Token(LESS, "<", -1, sb.lineNumber());
    } else if (sb.Char() == '>') {
        sb.GetChar();
        if (sb.Char() == '=') {
            return Token(GREATEREQ, ">=", -1, sb.lineNumber());
        }
        sb.UnGetChar();
        return Token(GREATER, ">=", -1, sb.lineNumber());
    } else if (sb.Char() == '{') {
        sb.GetChar();
        if (sb.Char() == '*') {
            return Token(OPENCOMMENT, "{*", -1, sb.lineNumber());
        }
        sb.UnGetChar();        
    } else if (sb.Char() == '(') {
        return Token(LPAREN, "(", -1, sb.lineNumber());
    } else if (sb.Char() == ')') {
        return Token(RPAREN, ")", -1, sb.lineNumber());
    } else if (sb.Char() == '[') {
        return Token(LSQBRACKET, "[", -1, sb.lineNumber());
    } else if (sb.Char() == ']') {
        return Token(RSQBRACKET, "]", -1, sb.lineNumber());
    } else if (sb.Char() == '+') {
        return Token(PLUS, "+", -1, sb.lineNumber());
    } else if (sb.Char() == '-') {
        return Token(MINUS, "-", -1, sb.lineNumber());
    } else if (sb.Char() == '*') {
        sb.GetChar();
        if (sb.Char() == '}') {
            return Token(CLOSECOMMENT, "*}", -1, sb.lineNumber());
        } 
        sb.UnGetChar();
        return Token(MULT, "*", -1, sb.lineNumber());
    } else if (sb.Char() == '/') {
        return Token(DIVD, "/", -1, sb.lineNumber());
    } else if (sb.Char() == '=') {
        return Token(EQUAL, "=", -1, sb.lineNumber());      
    } else if (sb.Char() == '\"') {
        nextToken.tokenval = QUOTE;
        nextToken.stringval = "\"";
        nextToken.numval = -1;    
        nextToken.lineno = sb.lineNumber();
        return nextToken;
    } else if (sb.Char() == ';') {
        return Token(SEMI, ";", -1, sb.lineNumber());
    } else if (sb.Char() == ',') {
        return Token(COMA, ",", -1, sb.lineNumber());
    } else if (sb.Char() == '\'') {
        nextToken.tokenval = SQUOTE;
        nextToken.stringval = "\'";
        nextToken.numval = -1;    
        nextToken.lineno = sb.lineNumber();
        return nextToken;
    } else {
        cout<<"ERROR uknown token: "<<sb.Char()<<"\n";
        nextToken.tokenval = ERROR;
        nextToken.stringval = "ERROR";
        nextToken.numval = -1;   
        nextToken.lineno = sb.lineNumber(); 
        sb.GetChar();
        return nextToken;
    }
    return nextToken;
}

int OwlLexer::extractFullNumber() {
    string asStr;
    while (sb.Char() != sb.EOFMarker() && isdigit(sb.Char())) {
        asStr.push_back(sb.Char());
        sb.GetChar();
    }
    return atoi(asStr.c_str());
}

string OwlLexer::extractFullWord() {
    string word;
    while (sb.Char() != sb.EOFMarker() && isalpha(sb.Char())) {
        word.push_back(sb.Char());
        sb.GetChar();
    }
    return word;
}

vector<Token> OwlLexer::tokenize() {
    vector<Token> tokenList;
        Token nextToken;
        while (sb.Char() != sb.EOFMarker()) {
        if (isalpha(sb.Char()) && inString == false) {
            string word = extractFullWord();
            nextToken.tokenval = handleKeywordOrId(word);
            nextToken.stringval = word;
            nextToken.lineno = sb.lineNumber();
            if (!inComment)
                tokenList.push_back(nextToken);
            continue;
        } else if (isdigit(sb.Char()) && inString == false) {
                int num = extractFullNumber();
                nextToken.tokenval = NUM;
                nextToken.numval = num;
                nextToken.stringval = to_string(num);
                nextToken.lineno = sb.lineNumber();
                if (!inComment)
                    tokenList.push_back(nextToken);
                continue;
        } else 
            if (sb.Char() != ' ') {
                nextToken = handleSpecials();
                if (inComment == false && nextToken.tokenval != OPENCOMMENT)
                    tokenList.push_back(nextToken);
                if (nextToken.tokenval == OPENCOMMENT) {
                    inComment = true;
                    sb.GetChar();
                }
                if (nextToken.tokenval == CLOSECOMMENT)
                    inComment = false;
            
        }
        sb.GetChar();
    }
    return tokenList;
}

#endif
