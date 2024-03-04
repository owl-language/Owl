#ifndef tokens_hpp
#define tokens_hpp
#include <iostream>
using namespace std;

enum TokenType {
    NONE,ERROR,ENDFILE,OPENCOMMENT,CLOSECOMMENT,
    ARRAY,INT,CHAR, STR,
    RETURN,IF,THEN,ELSE,ENDIF,WHILE,PROG,FUNC,READ,PRINT,BEGIN,END,LET,RAND,
    ID,NUM,ASSIGN,
    NOTEQUAL,EQUAL,LESS,GREATER,LESSEQ,GREATEREQ,
    NOT,PLUS,MINUS,MULT,DIVD,
    LPAREN,RPAREN,QUOTE,SEMI,COMA,LSQBRACKET,RSQBRACKET, SQUOTE, COLON
};

string tokenString[] = {
    "NONE","ERROR","ENDFILE","OPENCOMMENT","CLOSECOMMENT",
    "ARRAY","INT","CHAR", "STRING",
    "RETURN", "IF","THEN","ELSE","ENDIF","WHILE", "PROG", "FUNC", "READ", "PRINT","BEGIN","END","LET", "RAND",
    "ID", "NUM","ASSIGN", 
    "NOTEQUAL", "EQUAL", "LESS", "GREATER", "LESSEQ", "GREATEREQ",
    "NOT", "PLUS", "MINUS", "MULT", "DIVD", 
    "LPAREN", "RPAREN", "QUOTE", "SEMI", "COMA", "LSQBRACKET","RSQBRACKET", "SQUOTE", "COLON"
};

struct Token {
    TokenType tokenval;
    string stringval;
    int numval;
    int lineno;
    Token(TokenType tt = NONE, string sv = "", int nv = -1, int ln = 0) {
        tokenval = tt; stringval = sv; numval = nv; lineno = ln;
    }
    void print() {
        cout<<lineno<<": "<<" <[ "<<tokenString[tokenval]<<" , "<<stringval<<" ]>"<<endl;
    }
};

void printTokens(vector<Token>& tokens) {
    for (Token token : tokens)
        token.print();
}

#endif