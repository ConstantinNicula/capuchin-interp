#include <malloc.h>

#include "ast.h"
#include "token.h"
#include "utils.h"
#include "sbuf.h"

/************************************ 
 *         IDENTIFIER NODE          *
 ************************************/

Identifier_t* createIdentifier(Token_t* tok, const char* val) {
    Identifier_t* ident = (Identifier_t*)malloc(sizeof(Identifier_t));
    if (ident == NULL)
        return NULL; 
    ident->token = tok;
    ident->value = cloneString(val);
    return ident;
}

void cleanupIdentifier(Identifier_t** ident) {
    if (*ident == NULL)
        return;

    cleanupToken(&(*ident)->token);
    free((void*)(*ident)->value);
    
    free(*ident);
    *ident = NULL;
}

char* identifierToString(Identifier_t* ident) {
    // Return a copy to avoid free errors
    return cloneString(ident->value);
}

/************************************ 
 *         EXPRESSION NODE          *
 ************************************/

Identifier_t* createExpression() { 
    return NULL; 
}

void cleanupExpression(Expression_t** expr) { 
    return;
}

char* expressionToString(Expression_t* expr) {
    // Return a copy to avoid free errors 
    return cloneString("");
}

/************************************ 
 *         GENERIC STATEMENT        *
 ************************************/

Statement_t* createStatement(StatementType_t type, void* value) {
    Statement_t* st = (Statement_t*)malloc(sizeof(Statement_t));
    if( st == NULL)
        return NULL;
    st->type = type;
    st->value = value;
    return st;
}

void cleanupStatement(Statement_t** st) {
    if (*st == NULL)
        return;

    switch((*st)->type) {
        case STATEMENT_LET: 
            cleanupLetStatement((LetStatement_t**) &((*st)->value));
            break;
        case STATEMENT_RETURN:
            cleanupReturnStatement((ReturnStatement_t**)&((*st)->value));
            break;
        case STATEMENT_EXPRESSION:
            cleanupExpressionStatement((ExpressionStatement_t**)&((*st)->value));
            break;
        case STATEMENT_NULL:
            // TO DO: add handling 
            break;
        
    }
    (*st)->type = STATEMENT_NULL;
    free(*st);
    *st = NULL;
}


const char* statementTokenLiteral(Statement_t* st) {
    // Switch statement could be avoided with type punning, left like this for clarity :)
    switch (st->type)
    {
        case STATEMENT_LET:
            return ((LetStatement_t*)st->value)->token->literal;
        case STATEMENT_RETURN:
            return ((ReturnStatement_t*)st->value)->token->literal;
        case STATEMENT_EXPRESSION:
            return ((ExpressionStatement_t*)st->value)->token->literal;
        default:
            return "";
    }
}

char* statementToString(Statement_t* st) {
    switch (st->type)
    {
        case STATEMENT_LET:
            return letStatementToString((LetStatement_t*)st->value);
        case STATEMENT_RETURN:
            return returnStatementToString((ReturnStatement_t*)st->value);
        case STATEMENT_EXPRESSION:
            return expressionStatementToString((ExpressionStatement_t*)st->value);
        default:
            return cloneString("");
    }
}




/************************************ 
 *         LET STATEMENT            *
 ************************************/

LetStatement_t* createLetStatement(Token_t* token) {
    LetStatement_t* st = (LetStatement_t*)malloc(sizeof(LetStatement_t));
    if (st == NULL)
        return NULL;
    st->token = token;
    st->name = NULL;
    st->value = NULL;
    return st;
}


void cleanupLetStatement(LetStatement_t** st) {
    if (*st == NULL)
        return;

    cleanupToken(&(*st)->token);
    cleanupIdentifier(&(*st)->name);
    cleanupExpression(&(*st)->value);
    
    free(*st);
    *st = NULL;
}

char* letStatementToString(LetStatement_t* st) {
    Strbuf_t* sbuf = createStrbuf();
    char *ret = NULL, *tmp = NULL;
    
    strbufWrite(sbuf, st->token->literal);
    strbufWrite(sbuf, " ");

    tmp = identifierToString(st->name);
    strbufWrite(sbuf, tmp);
    free(tmp);

    strbufWrite(sbuf, " = ");

    if (st->value != NULL) {
        tmp = expressionToString(st->value);
        strbufWrite(sbuf, tmp);
        free(tmp);
    }

    strbufWrite(sbuf, ";");

    ret = strbufDetach(sbuf);
    cleanupStrbuf(&sbuf);
    return ret;
}


/************************************ 
 *      RETURN STATEMENT            *
 ************************************/

ReturnStatement_t* createReturnStatement(Token_t* token) {
    ReturnStatement_t* st = (ReturnStatement_t*)malloc(sizeof(ReturnStatement_t));
    if (st == NULL)
        return NULL;
    st->token = token;
    st->returnValue = NULL;
    return st;
}

void cleanupReturnStatement(ReturnStatement_t** st) {
    if (*st == NULL)
        return;
    
    cleanupToken(&(*st)->token);
    cleanupExpression(&(*st)->returnValue);
    
    free(*st);
    *st = NULL;
}

char* returnStatementToString(ReturnStatement_t* st) {
    Strbuf_t* sbuf = createStrbuf();
    char *ret  = NULL, *tmp = NULL;

    strbufWrite(sbuf, st->token->literal);
    strbufWrite(sbuf, " ");

    if (st->returnValue != NULL) {
        tmp = expressionToString(st->returnValue);
        strbufWrite(sbuf, tmp);
        free(tmp);
    }

    strbufWrite(sbuf, ";");
    
    ret = strbufDetach(sbuf);
    cleanupStrbuf(&sbuf);
    return ret;
}

/************************************ 
 *      EXPRESSION STATEMENT        *
 ************************************/

ExpressionStatement_t* createExpressionStatement(Token_t* token) {
    ExpressionStatement_t* st = (ExpressionStatement_t*) malloc(sizeof(ExpressionStatement_t));
    if (st == NULL)
        return NULL;
    st->token = token;
    st->expression = NULL;
    return st;
}

void cleanupExpressionStatement(ExpressionStatement_t** st) {
    if (*st == NULL)
        return;

    cleanupToken(&(*st)->token);
    cleanupExpression(&(*st)->expression);

    free(*st);
    *st = NULL;
}

char* expressionStatementToString(ExpressionStatement_t* st) {
    if (st->expression != NULL) {
        return expressionToString(st->expression);
    }
    return cloneString("");
}



/************************************ 
 *      PROGRAM NODE                *
 ************************************/

Program_t* createProgram() {
    Program_t* prog = (Program_t*) malloc(sizeof(Program_t));
    if (prog == NULL)
        return NULL;
    prog->statements = createVector(sizeof(Statement_t*));
    return prog;
}

Statement_t** programGetStatements(Program_t* prog) {
    return (Statement_t**)vectorGetBuffer(prog->statements);
}

uint32_t programGetStatementCount(Program_t* prog) {
    return vectorGetCount(prog->statements);
}

void cleanupProgram(Program_t** prog) {
    if (*prog == NULL) 
        return;

    uint32_t cnt = programGetStatementCount(*prog);
    Statement_t** pst = programGetStatements(*prog);

    for (uint32_t i = 0; i < cnt; i++)
    {
        cleanupStatement(&(pst[i]));
    }    

    cleanupVector(&(*prog)->statements);
    
    free(*prog);
    *prog = NULL;
}

void programAppendStatement(Program_t* prog, Statement_t* st) {
    vectorAppend(prog->statements, (void*)&st);
}

const char* programTokenLiteral(Program_t* prog) {
    if (programGetStatementCount(prog) > 0u) 
    {   
        Statement_t** stmts = programGetStatements(prog);
        return statementTokenLiteral(stmts[0]);
    } else  {
        return "";
    }
}


char* programToString(Program_t* prog) {
    Strbuf_t* sbuf = createStrbuf();
    char *ret = NULL, *tmp = NULL;

    uint32_t cnt = programGetStatementCount(prog);
    Statement_t** stmts = programGetStatements(prog);

    for (uint32_t i = 0; i < cnt; i++) {
        tmp = statementToString(stmts[i]);
        strbufWrite(sbuf, tmp);
        free(tmp);
    }

    ret = strbufDetach(sbuf);
    cleanupStrbuf(&sbuf);
    return ret;
}
