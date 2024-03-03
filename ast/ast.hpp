#ifndef miniast_hpp
#define miniast_hpp
#include <iostream>
#include <vector>
#include "../tokens/tokens.hpp"
using namespace std;


struct Attributes {
    string name;
    int intValue;
    TokenType op;
};

enum NodeKind {
    EXPRNODE, STMTNODE
};

enum ExprKind {
    ID_EXPR, OP_EXPR, CONST_EXPR, PARAM_EXPR, PROCDCALL, SUBSCRIPT_EXPR
};

vector<string> ExprKindStr = {
    "ID_EXPR", "OP_EXPR", "CONST_EXPR", "PARAM_EXPR", "PROCDCALL", "SUBSCRIPT_EXPR"
};

enum StmtKind {
    VARDECL, FUNCDECL, READSTM, PRINTSTM, ASSIGNSTM, EXPRSTM, WHILESTM, IFSTM, RETURNSTM
};

vector<string> StmtKindStr = {
    "VARDECL", "FUNCDECL", "READSTM", "PRINTSTMT", "ASSIGNSTM", "EXPRSTM", "WHILESTM", "IFSTM", "RETURNSTM"
};

typedef union NodeType {
    StmtKind stmt;
    ExprKind expr;
} NodeType;

const int MAX_CHILDREN = 3;

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

ASTNode* makeExpressionNode(ExprKind expr, Attributes attr) {
    ASTNode* node = new ASTNode();
    node->kind = EXPRNODE;
    node->type.expr = expr;
    node->attribute = attr;
    return node;
}

ASTNode* makeStatementNode(StmtKind stmt, Attributes attr) {
    ASTNode* node = new ASTNode();
    node->kind = STMTNODE;
    node->type.stmt = stmt;
    node->attribute = attr;
    return node;
}

void printNode(ASTNode* node) {
    if (node->kind == EXPRNODE) {
        cout<<"["<<ExprKindStr[node->type.expr]<<"] "<<node->attribute.name<<" - ";
        if (node->type.expr == OP_EXPR) 
            cout<<tokenString[node->attribute.op];
        cout<<endl;
    } else {
        cout<<"["<<StmtKindStr[node->type.stmt]<<"] "<<node->attribute.name<<endl;
    }
}

int rd = 0;
void printTree(ASTNode* node) {
    ++rd;
    if (node != nullptr) {
        for (int i = 0; i < rd; i++) cout<<"  ";
        printNode(node);
        for (int i = 0; i < MAX_CHILDREN; i++)
            printTree(node->child[i]);
        
        if (node->sibling) --rd;
        printTree(node->sibling);
    }
    --rd;
}

void freeTree(ASTNode* h) {
    if (h == nullptr) return;
    for (int i = 0; i < MAX_CHILDREN; i++)
        freeTree(h->child[i]);
    freeTree(h->sibling);
    delete h;
}

#endif