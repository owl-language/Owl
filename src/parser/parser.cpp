#include "parser.hpp"

OwlParser::OwlParser() {

}

ASTNode* OwlParser::replParse(vector<Token>& tokens) {
    depth = 0;
    initStream(tokens);
    ASTNode* singleLine = statement();
    return singleLine;
}

ASTNode* OwlParser::start(vector<Token>& tokens) {
    if (tokens.empty()) {
        logError("Error: token stream is empty.");
        return nullptr;
    }
    depth = 0;
    initStream(tokens);
    if (lookahead().tokenval == LIB) {
        return library();
    }  else {
        return program();
    }
}

ASTNode* OwlParser::library() {
    ASTNode* mainBlock;
    match(LIB);
    match(SQUOTE);
    match(ID);
    match(SQUOTE);
    match(SEMI);
    mainBlock = block();
    depth = 0;
    return mainBlock;
}


ASTNode* OwlParser::program() {
    ASTNode* mainBlock, *imports;
    match(PROG);
    match(SQUOTE);
    match(ID);
    match(SQUOTE);
    match(SEMI);
    if (lookahead().tokenval == IMPORT) {
        match(IMPORT);
        imports = makeInterpreterDirective(IMPORT_DIRECTIVE, Attributes(lookahead().stringval, lookahead().numval, lookahead().realval, lookahead().tokenval));
        match(ID);
        match(SEMI);
    }
    mainBlock = block();
    if (imports) {
        imports->sibling = mainBlock;
        return imports;
    }
    depth = 0;
    return mainBlock;
}

ASTNode* OwlParser::block() {
    onEnter("block");
    match(BEGIN);
    ASTNode* node = statementList();
    match(END);
    onExit("block");
    return node;
}

ASTNode* OwlParser::statementList() {
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

ASTNode* OwlParser::statement() {
    onEnter("statement");
    ASTNode* node = nullptr;
    switch (lookahead().tokenval) {
        case LET:
            node = declareVarStatement();
            break;
        case FUNC:
            node = declareProcedureStatement();
            break;
        case RECORD:
            node = declareRecordType();
            break;
        case PRINT:
            node = printStatement();
            break;
        case READ:
            node = readStatement();
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
            break;
        default:
            cout<<"Hoot!, invalid statement on line "<<lookahead().lineno<<": "<<tokenString[lookahead().tokenval]<<endl;
            reSync();
            return node;
    }
    if (lookahead().tokenval == SEMI)
        match(SEMI);
    onExit("statement");
    return node;
}

ASTNode* OwlParser::assignStatement() {
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

ASTNode* OwlParser::declareVarStatement() {
    onEnter("declareVarStatement");
    ASTNode* node = makeStatementNode(VARDECL, Attributes(lookahead().stringval, lookahead().numval, lookahead().realval, lookahead().tokenval));
    match(LET);
    node->child[0] = term();
    if (lookahead().tokenval == SEMI) {
        match(SEMI);
        return node;
    }
    match(ASSIGN);
    node->child[1] = expression();
    if (node->child[1]->type.expr == INITREC_EXPR) {
        node->child[1]->attribute.name += " - " + node->child[0]->attribute.name;
        ASTNode* t = node->child[0];
        node->child[0] = node->child[1];
        node->child[1] = nullptr;
        delete t;
    }
    match(SEMI);
    onExit("declareVarStatement");
    return node;
}

ASTNode* OwlParser::declareRecordType() {
    onEnter("declareRecordType");
    ASTNode* node = makeStatementNode(RECDECL, Attributes(lookahead().stringval, lookahead().numval, lookahead().realval, lookahead().tokenval));
    match(RECORD);
    node->child[0] = term();
    match(BEGIN);
    ASTNode *p = node->child[0];
    while (lookahead().tokenval != END) {
        ASTNode* t = declareVarStatement();
        p->sibling = t;
        p = t;
    }
    match(END);
    onExit("declareRecordType");
    return node;
}

ASTNode* OwlParser::declareProcedureStatement() {
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

ASTNode* OwlParser::parameterList() {
    ASTNode* t = nullptr;
    onEnter("paramList");
    if (lookahead().tokenval == RPAREN) {
        return t;
    } else {
        t = factor();
        t->type.expr = PARAM_EXPR;
        match(COLON);
        if (lookahead().tokenval == REF) {
            match(REF);
            t->attribute.type = as_ref;
        }
        if (lookahead().tokenval == INT || lookahead().tokenval == REALT || lookahead().tokenval == STR) {
            match(lookahead().tokenval);
        }
        ASTNode* m = t;
        while (lookahead().tokenval == COMA) {
            match(COMA);
            ASTNode* p = factor();
            match(COLON);
            if (lookahead().tokenval == REF) {
                match(REF);
                p->attribute.type = as_ref;
            }
            if (lookahead().tokenval == INT || lookahead().tokenval == REALT || lookahead().tokenval == STR) {
                match(lookahead().tokenval);
            }
            p->type.expr = PARAM_EXPR;
            m->sibling = p;
            m = p;
        }
    }
    onExit("paramList");
    return t;
}

ASTNode* OwlParser::procedureCall() {
    onEnter("procedureCall");
    ASTNode* node = makeExpressionNode(PROCDCALL, Attributes(lookahead().stringval, lookahead().numval, lookahead().realval, lookahead().tokenval));
    match(LPAREN);
    if (lookahead().tokenval != RPAREN)
        node->child[1] = argumentList();
    match(RPAREN);
    onExit("procedureCall");
    return node;
}

ASTNode* OwlParser::argumentList() {
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

ASTNode* OwlParser::exprStatement() {
    onEnter("exprStatement");
    ASTNode* t = makeStatementNode(EXPRSTM, Attributes(lookahead().stringval, lookahead().numval, lookahead().realval, lookahead().tokenval));
    if (lookahead().tokenval == ID) {
        t->child[0] = var();
        if (lookahead().tokenval == PERIOD) {
            match(PERIOD);
            t->child[0]->sibling = var();
        } 
        if (lookahead().tokenval == ASSIGN) {
            ASTNode* p = makeStatementNode(ASSIGNSTM, Attributes(lookahead().stringval, lookahead().numval, lookahead().realval, lookahead().tokenval));
            p->attribute.op = ASSIGN;
            match(ASSIGN);
            p->child[0] = t->child[0];
            p->child[1] = expression();
            t->child[0] = p;
            t = t->child[0];
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

ASTNode* OwlParser::whileStatement() {
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

ASTNode* OwlParser::ifStatement() {
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

ASTNode* OwlParser::printStatement() {
    ASTNode* node = makeStatementNode(PRINTSTM, Attributes(lookahead().stringval, lookahead().numval, lookahead().realval, lookahead().tokenval));
    match(PRINT);
    if (lookahead().tokenval == QUOTE) 
        node->child[0] = strValue();
    else node->child[0] = term();
    match(SEMI);
    return node;
}

ASTNode* OwlParser::readStatement() {
    ASTNode* node = makeStatementNode(READSTM, Attributes(lookahead().stringval, lookahead().numval, lookahead().realval, lookahead().tokenval));
    match(READ);
    node->child[0] = var();
    match(SEMI);
    return node;
}

ASTNode* OwlParser::randExpression() {
    ASTNode* node = makeExpressionNode(RAND_EXPR, Attributes(lookahead().stringval, lookahead().numval, lookahead().realval, lookahead().tokenval));
    match(RAND);
    match(LPAREN);
    node->child[0] = expression();
    match(RPAREN);
    return node;
}

ASTNode* OwlParser::returnStatement() {
    ASTNode* t = makeStatementNode(RETURNSTM, Attributes(lookahead().stringval, lookahead().numval, lookahead().realval, lookahead().tokenval));
    onEnter("return statement");
    match(RETURN);
    t->child[0] = expression();
    match(SEMI);
    onExit("return statement");
    return t;
}

ASTNode* OwlParser::expression() {
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

ASTNode* OwlParser::simpleExpression() {
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

ASTNode* OwlParser::term() {
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

ASTNode* OwlParser::factor() {
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
    } else if (lookahead().tokenval == MAKE) {
        match(MAKE);
        node = initRecord();
    } else {
        cout<<"Parse error in factor: I have no idea what a '"<<lookahead().stringval<<"' is"<<endl;
        reSync();
    }
    onExit("factor");
    return node;
}

ASTNode* OwlParser::var() {
    onEnter("var");
    ASTNode* node = makeExpressionNode(ID_EXPR, Attributes(lookahead().stringval, lookahead().numval, lookahead().realval, lookahead().tokenval));
    match(ID);
    if (lookahead().tokenval == LSQBRACKET) {
        match(LSQBRACKET);
        node->child[0] = expression();
        match(RSQBRACKET);
        node->type.expr = SUBSCRIPT_EXPR;
    } else if (lookahead().tokenval == LPAREN) {
        ASTNode* replace = procedureCall();
        replace->attribute.name = node->attribute.name;
        delete node;
        node = replace;
    } else if (lookahead().tokenval == PERIOD) {
        match(PERIOD);
        node->child[2] = expression();
    }
    onExit("var");
    return node;
}

ASTNode* OwlParser::strValue() {
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

ASTNode* OwlParser::initRecord() {
    onEnter("initRecord");
    ASTNode* node = makeExpressionNode(INITREC_EXPR, Attributes(lookahead().stringval, lookahead().numval, lookahead().realval, lookahead().tokenval));
    match(ID);
    onExit("initRecord");
    return node;
}

Token& OwlParser::lookahead() {
    return *ts;
}

void OwlParser::nexttoken() {
    std::advance(ts,1);
    if (ts != tokenvector.end()) {
        currentToken = *ts;
    } else {
        say("Token Stream Consumed.");
        std::advance(ts, -1);
    }
}

void OwlParser::reSync() {
    //if you can't tell, im not a fan of trying to recover from a mistep in parseing
    //better to have the programmer fix whats wrong that mangle weird AST
    if (stumbleCount > 2) {
        logError("Too many errors during parsing, bailing out.");
        exit(0);
    }
    stumbleCount++;
}

bool OwlParser::match(TokenType token) {
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

void OwlParser::initStream(vector<Token>& tokens) {
    tokenvector = tokens;
    ts = tokenvector.begin();
    currentToken = *ts;
}