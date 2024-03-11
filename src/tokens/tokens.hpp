#pragma once
#ifndef tokens_hpp
#define tokens_hpp
#include <iostream>
#include <vector>
using namespace std;

enum TokenType {
    NONE,ERROR,ENDFILE,OPENCOMMENT,CLOSECOMMENT,LINECOMMENT,
    ARRAY,INT,REALT, REF, STR,
    RETURN,IMPORT,IF,THEN,ELSE,ENDIF,WHILE,PROG,LIB,FUNC,READ,PRINT,BEGIN,END,LET,RAND,
    STRING_LITERAL,ID,REALNUM,NUM,ASSIGN,
    NOTEQUAL,EQUAL,LESS,GREATER,LESSEQ,GREATEREQ,
    NOT,PLUS,MINUS,MULT,DIVD,
    LPAREN,RPAREN,QUOTE,SEMI,COMA,PERIOD,LSQBRACKET,RSQBRACKET, SQUOTE, COLON
};

inline string tokenString[] = {
    "NONE","ERROR","ENDFILE","OPENCOMMENT","CLOSECOMMENT","LINECOMMENT",
    "ARRAY","INT","REALT","REF", "STRING",
    "RETURN","IMPORT", "IF","THEN","ELSE","ENDIF","WHILE", "PROG","LIBR", "FUNC", "READ", "PRINT","BEGIN","END","LET", "RAND",
    "STRING_LITERAL","ID", "REALNUM", "NUM","ASSIGN", 
    "NOTEQUAL", "EQUAL", "LESS", "GREATER", "LESSEQ", "GREATEREQ",
    "NOT", "PLUS", "MINUS", "MULT", "DIVD", 
    "LPAREN", "RPAREN", "QUOTE", "SEMI", "COMA", "PERIOD", "LSQBRACKET","RSQBRACKET", "SQUOTE", "COLON"
};

struct Token {
    TokenType tokenval;
    string stringval;
    int numval;
    int realval;
    int lineno;
    Token(TokenType tt = NONE, string sv = "", int nv = -1, int ln = 0) {
        tokenval = tt; stringval = sv; numval = nv; lineno = ln;
    }
    void print() {
        cout<<lineno<<": "<<" <[ "<<tokenString[tokenval]<<" , "<<stringval<<" ]>"<<endl;
    }
};

void printTokens(vector<Token>& tokens);

#endif