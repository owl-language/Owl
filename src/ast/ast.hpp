#pragma once
#ifndef miniast_hpp
#define miniast_hpp
#include <iostream>
#include <vector>
#include "../tokens/tokens.hpp"
using std::cout;
using std::endl;
using std::string;
using std::vector;

enum storedType {
    as_int, as_real, as_string, as_ref
};

inline string storedTypeStr[] = { "as_int", "as_real", "as_string", "as_ref"};

struct Attributes {
    string name;
    int intValue;
    double realValue;
    TokenType op;
    storedType type;
    Attributes(string n = "", int i = 0, double r = 0.0, TokenType tt = NONE) {
        name = n;
        intValue = i;
        realValue = r;
        op = tt;
        type = as_int;
    }
};

enum NodeKind {
    EXPRNODE, STMTNODE, DIRECTIVE
};

enum ExprKind {
    ID_EXPR, OP_EXPR, CONST_EXPR, PARAM_EXPR, PROCDCALL, SUBSCRIPT_EXPR, RAND_EXPR, CONST_STR
};

inline vector<string> ExprKindStr = {
    "ID_EXPR", "OP_EXPR", "CONST_EXPR", "PARAM_EXPR", "PROCDCALL", "SUBSCRIPT_EXPR", "RAND_EXPR", "CONST_STR"
};

enum StmtKind {
    VARDECL, FUNCDECL, READSTM, PRINTSTM, ASSIGNSTM, EXPRSTM, WHILESTM, IFSTM, RETURNSTM
};

inline vector<string> StmtKindStr = {
    "VARDECL", "FUNCDECL", "READSTM", "PRINTSTMT", "ASSIGNSTM", "EXPRSTM", "WHILESTM", "IFSTM", "RETURNSTM"
};

enum DirectiveKind {
    IMPORT_DIRECTIVE
};

inline vector<string> DirectiveKindStr = { "IMPORT_DIRECTIVE" };

typedef union NodeType {
    StmtKind stmt;
    ExprKind expr;
    DirectiveKind drctv;
} NodeType;

inline const int MAX_CHILDREN = 3;

struct ASTNode {
    NodeKind kind;
    NodeType type;
    Attributes attribute;
    ASTNode* child[MAX_CHILDREN];
    ASTNode* sibling;
    ASTNode() {
        for (int i = 0; i < 3; i++)
            child[i] = nullptr;
        sibling = nullptr;
    }
};

ASTNode* makeExpressionNode(ExprKind expr, Attributes attr);

ASTNode* makeStatementNode(StmtKind stmt, Attributes attr);

ASTNode* makeInterpreterDirective(DirectiveKind directive, Attributes attr);

void printNode(ASTNode* node);

inline int rd = 0;
void printTree(ASTNode* node);

void freeTree(ASTNode* h);

#endif