#ifndef parser_hpp
#define parser_hpp
#include <iostream>
#include <vector>
#include "../ast/ast.hpp"
#include "../tokens/tokens.hpp"
#include "../tools/tracer.hpp"
using namespace std;

class Parser {
    private:
        using TokenStream = vector<Token>::iterator;
        vector<Token> tokenvector;
        TokenStream ts;
        Token currentToken;
        Token& lookahead() {
            return *ts;
        }
        void nexttoken() {
            ts++;
            if (ts != tokenvector.end()) {
                currentToken = *ts;
            } else {
                say("Token Stream Consumed.");
            }
        }
        void reSync() {
            while (lookahead().tokenval != END) {
                if (lookahead().tokenval == SEMI || lookahead().tokenval == ELSE)
                    break;
                nexttoken();
            }
        }
        bool match(TokenType token) {
            if (lookahead().tokenval == token) {
                say("Match: " + currentToken.stringval);
                nexttoken();
                return true;
            }
            cout<<"Mismatched Token on line "<<lookahead().lineno<<": "<<lookahead().stringval<<endl;
            cout<<"Expected: "<<tokenString[lookahead().tokenval]<<" but found "<<tokenString[token]<<endl;
            return false;
        }
        void initStream(vector<Token>& tokens) {
            tokenvector = tokens;
            ts = tokenvector.begin();
            currentToken = *ts;
        }
    public:
        Parser() {

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
};

ASTNode* Parser::replParse(vector<Token>& tokens) {
    depth = 0;
    initStream(tokens);
    ASTNode* singleLine = statement();
    return singleLine;
}

ASTNode* Parser::start(vector<Token>& tokens) {
    depth = 0;
    initStream(tokens);
    match(PROG);
    match(SQUOTE);
    match(ID);
    match(SQUOTE);
    match(SEMI);
    ASTNode* mainBlock = block();
    depth = 0;
    return mainBlock;
}

ASTNode* Parser::block() {
    onEnter("block");
    match(BEGIN);
    ASTNode* node = statementList();
    match(END);
    onExit("block");
    return node;
}

ASTNode* Parser::statementList() {
    onEnter("statementList");
    ASTNode* node = statement();
    ASTNode* next = node;
    while (lookahead().tokenval != END && lookahead().tokenval != ELSE) {
        ASTNode* t = statement();
        if (t != nullptr) {
            if (node == nullptr) {
                node = next = t;
            } else {
                next->sibling = t;
                next = t;
            }
        }
    } 
    onExit("statementList");
    return node;
}

ASTNode* Parser::statement() {
    onEnter("statement");
    ASTNode* node = nullptr;
    switch (lookahead().tokenval) {
        case LET:
            node = declareVarStatement();
            break;
        case PRINT:
            node = printStatement();
            break;
        case READ:
            node = readStatement();
            break;
        case FUNC:
            node = declareProcedureStatement();
            break;
        case WHILE:
            node = whileStatement();
            break;
        case IF:
            node = ifStatement();
            break;
        case ID:
            node = exprStatement();
            break;
        case RETURN: 
            node = returnStatement();
        default:
            cout<<"Error, invalid statement on line "<<lookahead().lineno<<": "<<lookahead().stringval<<endl;
            reSync();
            return node;
    }
    if (lookahead().tokenval == SEMI)
        match(SEMI);
    onExit("statement");
    return node;
}

ASTNode* Parser::assignStatement() {
    onEnter("assignmentStatement");
    ASTNode* node = makeStatementNode(ASSIGNSTM, Attributes(lookahead().stringval, lookahead().numval, lookahead().realval, lookahead().tokenval));
    if (node != nullptr && lookahead().tokenval == ID) {
        match(ID);
        match(ASSIGN);
        node->child[0] = expression();
    }
    match(SEMI);
    onExit("assignmentStatement");
    return node;
}

ASTNode* Parser::declareVarStatement() {
    onEnter("declareVarStatement");
    ASTNode* node = makeStatementNode(VARDECL, Attributes(lookahead().stringval, lookahead().numval, lookahead().realval, lookahead().tokenval));
    match(LET);
    node->child[0] = term();
    match(COLON);
    if (lookahead().tokenval == INT) {
        match(INT);
        if (lookahead().tokenval == SEMI) {
            match(SEMI);
            return node;
        }
        match(ASSIGN);
        node->child[1] = term();
    } else if (lookahead().tokenval == REALT) {
        match(REALT);
        if (lookahead().tokenval == SEMI) {
            match(SEMI);
            return node;
        }
        match(ASSIGN);
        node->child[1] = term();
    } else if (lookahead().tokenval == STR) {
        match(STR);
        match(ASSIGN);
        node->child[1] = strValue();
        match(SEMI);
        onExit("declareVarStatement");
    }
    return node;
}

ASTNode* Parser::declareProcedureStatement() {
    onEnter("declareProcedure");
    ASTNode* node = makeStatementNode(FUNCDECL, Attributes(lookahead().stringval, lookahead().numval, lookahead().realval, lookahead().tokenval));
    match(FUNC);
    node->attribute.name = lookahead().stringval;
    match(ID);
    match(LPAREN);
    node->child[1] = parameterList();
    match(RPAREN);
    node->child[0] = block();
    onExit("declareProcedure");
    return node;
}

ASTNode* Parser::parameterList() {
    ASTNode* t = nullptr;
    onEnter("paramList");
    if (lookahead().tokenval == RPAREN) {
        return t;
    } else {
        t = factor();
        t->type.expr = PARAM_EXPR;
        match(COLON);
        match(INT);
        ASTNode* m = t;
        while (lookahead().tokenval == COMA) {
            match(COMA);
            ASTNode* p = factor();
            match(COLON);
            match(INT);
            p->type.expr = PARAM_EXPR;
            m->sibling = p;
            m = p;
        }
    }
    onExit("paramList");
    return t;
}

ASTNode* Parser::procedureCall() {
    onEnter("procedureCall");
    ASTNode* node = makeExpressionNode(PROCDCALL, Attributes(lookahead().stringval, lookahead().numval, lookahead().realval, lookahead().tokenval));
    match(LPAREN);
    if (lookahead().tokenval != RPAREN)
        node->child[1] = argumentList();
    match(RPAREN);
    onExit("procedureCall");
    return node;
}

ASTNode* Parser::argumentList() {
    onEnter("argumentList");
    ASTNode* node = nullptr;
    if (lookahead().tokenval == RPAREN)
        match(RPAREN);
    else {
        node = expression();
        ASTNode* m = node;
        while (lookahead().tokenval == COMA) {
            match(COMA);
            ASTNode* p = expression();
            m->sibling = p;
            m = p;
        }
    }
    onExit("argumentList");
    return node;
}

ASTNode* Parser::exprStatement() {
    onEnter("exprStatement");
    ASTNode* t = makeStatementNode(EXPRSTM, Attributes(lookahead().stringval, lookahead().numval, lookahead().realval, lookahead().tokenval));
    if (lookahead().tokenval == ID) {
        t->child[0] = var();
        if (lookahead().tokenval == ASSIGN) {
            ASTNode* p = makeStatementNode(ASSIGNSTM, Attributes(lookahead().stringval, lookahead().numval, lookahead().realval, lookahead().tokenval));
            p->attribute.op = ASSIGN;
            match(ASSIGN);
            p->child[0] = t->child[0];
            p->child[1] = expression();
            t->child[0] = p;
        } else if (lookahead().tokenval == LPAREN) {
            ASTNode* p = makeExpressionNode(PROCDCALL, Attributes(lookahead().stringval, lookahead().numval, lookahead().realval, lookahead().tokenval));
            p->attribute.name = t->child[0]->attribute.name;
            match(LPAREN);
            if (lookahead().tokenval != RPAREN) {
                ASTNode* td = t->child[0];
                p->child[0] = t->child[0];
                p->child[1] = argumentList();
                p->child[0] = nullptr;
                t->child[0] = p;
                delete td;
            }
            match(RPAREN);
        }
    } else {
        t->child[0] = simpleExpression();
    }
    match(SEMI);
    onExit("exprStatement");
    return t;
}

ASTNode* Parser::whileStatement() {
    ASTNode* t = makeStatementNode(WHILESTM, Attributes(lookahead().stringval, lookahead().numval, lookahead().realval, lookahead().tokenval));
    onEnter("whileStatement");
    match(WHILE);
    t->child[0] = expression();
    match(BEGIN);
    t->child[1] = statementList();
    match(END);
    onExit("whileStatement");
    return t;
}

ASTNode* Parser::ifStatement() {
    ASTNode* t = makeStatementNode(IFSTM, Attributes(lookahead().stringval, lookahead().numval, lookahead().realval, lookahead().tokenval));
    onEnter("ifStatement");
    match(IF);
    t->child[0] = expression();
    match(THEN);
    t->child[1] = statementList();
    if (lookahead().tokenval == ELSE) {
        match(ELSE);
        t->child[2] = statementList();
    }
    match(END);
    onExit("ifStatement");
    return t;
}

ASTNode* Parser::printStatement() {
    ASTNode* node = makeStatementNode(PRINTSTM, Attributes(lookahead().stringval, lookahead().numval, lookahead().realval, lookahead().tokenval));
    match(PRINT);
    if (lookahead().tokenval == QUOTE) 
        node->child[0] = strValue();
    else node->child[0] = term();
    match(SEMI);
    return node;
}

ASTNode* Parser::readStatement() {
    ASTNode* node = makeStatementNode(READSTM, Attributes(lookahead().stringval, lookahead().numval, lookahead().realval, lookahead().tokenval));
    match(READ);
    node->child[0] = var();
    match(SEMI);
    return node;
}

ASTNode* Parser::randExpression() {
    ASTNode* node = makeExpressionNode(RAND_EXPR, Attributes(lookahead().stringval, lookahead().numval, lookahead().realval, lookahead().tokenval));
    match(RAND);
    match(LPAREN);
    node->child[0] = expression();
    match(RPAREN);
    return node;
}

ASTNode* Parser::returnStatement() {
    ASTNode* t = makeStatementNode(RETURNSTM, Attributes(lookahead().stringval, lookahead().numval, lookahead().realval, lookahead().tokenval));
    onEnter("return statement");
    match(RETURN);
    t->child[0] = expression();
    match(SEMI);
    onExit("return statement");
    return t;
}


ASTNode* Parser::expression() {
    onEnter("expression");
    ASTNode* node = simpleExpression();
    if (lookahead().tokenval == LESS || lookahead().tokenval == GREATER || 
        lookahead().tokenval == LESSEQ || lookahead().tokenval == GREATEREQ ||
        lookahead().tokenval == EQUAL || lookahead().tokenval == NOTEQUAL) {
        ASTNode* exp = makeExpressionNode(OP_EXPR, Attributes(lookahead().stringval, lookahead().numval, lookahead().realval, lookahead().tokenval));
        if (node != nullptr) {
            exp->child[0] = node;
            node = exp;
        }
        match(lookahead().tokenval);
        if ( node != nullptr)
            node->child[1] = simpleExpression();
    }
    onExit("expression");
    return node; 
}

ASTNode* Parser::simpleExpression() {
    onEnter("simpleExpression");
    ASTNode* node = term();
    while (lookahead().tokenval == PLUS || lookahead().tokenval == MINUS) {
        ASTNode* tmp = makeExpressionNode(OP_EXPR, Attributes(lookahead().stringval, lookahead().numval, lookahead().realval, lookahead().tokenval));
        if (node != nullptr) {
            tmp->child[0] = node;
            node = tmp;
            match(lookahead().tokenval);
            tmp->child[1] = term();
        } 
    }
    onExit("simpleExpression");
    return node;
}

ASTNode* Parser::term() {
    onEnter("term");
    ASTNode* node = factor();
    while (lookahead().tokenval == MULT || lookahead().tokenval == DIVD) {
        ASTNode* expNode = makeExpressionNode(OP_EXPR, Attributes(lookahead().stringval, lookahead().numval, lookahead().realval, lookahead().tokenval));
        expNode->child[0] = node;
        node = expNode;
        match(lookahead().tokenval);
        expNode->child[1] = factor();
    }
    onExit("term");
    return node;
}

ASTNode* Parser::factor() {
    ASTNode* node = nullptr;
    onEnter("factor");
    if (lookahead().tokenval == ID) {
        node = var();
    } else if (lookahead().tokenval == NUM) {
        node = makeExpressionNode(CONST_EXPR, Attributes(lookahead().stringval, lookahead().numval, lookahead().realval, lookahead().tokenval));
        node->attribute.intValue = lookahead().numval;
        match(NUM);
    } else if (lookahead().tokenval == REALNUM) {
        node = makeExpressionNode(CONST_EXPR, Attributes(lookahead().stringval, lookahead().numval, lookahead().realval, lookahead().tokenval));
        node->attribute.type = as_real;
        node->attribute.intValue = lookahead().numval;
        match(REALNUM);
    } else if (lookahead().tokenval == QUOTE) {
        match(QUOTE);
        if (lookahead().tokenval == STRING_LITERAL) {
            node = makeExpressionNode(CONST_STR, Attributes(lookahead().stringval, lookahead().numval, lookahead().realval, lookahead().tokenval));
            node->attribute.type = as_string;
            node->attribute.name = lookahead().stringval;
            match(STRING_LITERAL);
            match(QUOTE);
        } else {
            cout<<"ERROR: unexpected string"<<endl;
        }
    } else if (lookahead().tokenval == RAND) {
        node = randExpression();
    } else if (lookahead().tokenval == LPAREN) {
        match(LPAREN);
        if (lookahead().tokenval != RPAREN)
            node = expression();
        match(RPAREN);
    } else {
        cout<<"I have no idea."<<endl;
        reSync();
    }
    onExit("factor");
    return node;
}

ASTNode* Parser::var() {
    onEnter("var");
    ASTNode* node = makeExpressionNode(ID_EXPR, Attributes(lookahead().stringval, lookahead().numval, lookahead().realval, lookahead().tokenval));
    match(ID);
    if (lookahead().tokenval == LSQBRACKET) {
        match(LSQBRACKET);
        node->child[0] = expression();
        match(RSQBRACKET);
        node->type.expr = SUBSCRIPT_EXPR;
    }else if (lookahead().tokenval == LPAREN) {
        ASTNode* replace = procedureCall();
        replace->attribute.name = node->attribute.name;
        delete node;
        node = replace;
    }
    onExit("var");
    return node;
}

ASTNode* Parser::strValue() {
    onEnter("String value");
    match(QUOTE);
    ASTNode* node = makeExpressionNode(CONST_STR, Attributes(lookahead().stringval, lookahead().numval, lookahead().realval, lookahead().tokenval));
    node->attribute.type = as_string;
    say("Returning const string: " + lookahead().stringval);
    match(STRING_LITERAL);
    match(QUOTE);
    onExit("String value");
    return node;
}

#endif