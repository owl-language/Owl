#include "ast.hpp"

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

ASTNode* makeInterpreterDirective(DirectiveKind directive, Attributes attr) {
    ASTNode* node = new ASTNode();
    node->kind = DIRECTIVE;
    node->type.drctv = directive;
    node->attribute = attr;
    return node;
}

void printNode(ASTNode* node) {
    if (node->kind == EXPRNODE) {
        cout<<"["<<ExprKindStr[node->type.expr]<<"] "<<node->attribute.name<<" - ";
        if (node->type.expr == OP_EXPR) 
            cout<<tokenString[node->attribute.op];
        cout<<endl;
    } else if (node->kind == STMTNODE) {
        cout<<"["<<StmtKindStr[node->type.stmt]<<"] "<<node->attribute.name<<endl;
    } else if (node->kind == DIRECTIVE) {
        cout<<"["<<DirectiveKindStr[node->type.drctv]<<"] "<<node->attribute.name<<endl;
    }
}

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
