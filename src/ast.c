#include <malloc.h>

#include "ast.h"
#include "token.h"
#include "utils.h"
#include "sbuf.h"


/************************************ 
 *         EXPRESSION NODE          *
 ************************************/

Expression_t* createExpression(ExpressionType_t type, void* value) { 
    Expression_t* exp = (Expression_t*)malloc(sizeof(Expression_t));
    if (exp == NULL)
        return NULL;
    exp->type = type;
    exp->value = value;
    return exp;
}

void cleanupExpression(Expression_t** expr) { 
    if (*expr == NULL)
        return;
    
    switch((*expr)->type) {
        case EXPRESSION_IDENTIFIER:
            cleanupIdentifier((Identifier_t**)&(*expr)->value);
            break;
        case EXPRESSION_INTEGER_LITERAL:
            cleanupIntegerLiteral((IntegerLiteral_t**)&(*expr)->value);
            break;
        case EXPRESSION_INVALID:
            // TO DO: add handling 
            break;
    }
    (*expr)->type = EXPRESSION_INVALID;
    free(*expr);
    *expr=NULL;
}

char* expressionToString(Expression_t* expr) {
    switch (expr->type)
    {
        case EXPRESSION_IDENTIFIER:
            return identifierToString((Identifier_t*)expr->value);
        case EXPRESSION_INTEGER_LITERAL: 
            return integerLiteralToString((IntegerLiteral_t*)expr->value);
        default:
            return cloneString("");
    }
}

const char* expressionTokenLiteral(Expression_t* expr) {
    // Switch statement could be avoided with type punning, left like this for clarity :)
    switch (expr->type)
    {
        case EXPRESSION_IDENTIFIER:
            return ((Identifier_t*)expr->value)->token->literal;
        case EXPRESSION_INTEGER_LITERAL:
            return ((IntegerLiteral_t*)expr->value)->token->literal;
        default:
            return "";
    }
}


/************************************ 
 *           IDENTIFIER             *
 ************************************/

Identifier_t* createIdentifier(const Token_t* tok, const char* val) {
    Identifier_t* ident = (Identifier_t*)malloc(sizeof(Identifier_t));
    if (ident == NULL)
        return NULL; 
    ident->token = cloneToken(tok);
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
 *      INTEGER LITERAL             *
 ************************************/

IntegerLiteral_t* createIntegerLiteral(const Token_t* tok) {
    IntegerLiteral_t* il = (IntegerLiteral_t*)malloc(sizeof(IntegerLiteral_t));
    if (il == NULL)
        return NULL;
    il->token = cloneToken(tok);
    il->value = 0;
    return il;
}

void cleanupIntegerLiteral(IntegerLiteral_t** il) {
    if (*il == NULL)
        return;
    
    cleanupToken(&(*il)->token);
    
    free(*il);
    *il = NULL;
}

char* integerLiteralToString(IntegerLiteral_t* il) {
    return cloneString(il->token->literal);
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
        case STATEMENT_INVALID:
            // TO DO: add handling 
            break;
        
    }
    (*st)->type = STATEMENT_INVALID;
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

LetStatement_t* createLetStatement(const Token_t* token) {
    LetStatement_t* st = (LetStatement_t*)malloc(sizeof(LetStatement_t));
    if (st == NULL)
        return NULL;
    st->token = cloneToken(token);
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

ReturnStatement_t* createReturnStatement(const Token_t* token) {
    ReturnStatement_t* st = (ReturnStatement_t*)malloc(sizeof(ReturnStatement_t));
    if (st == NULL)
        return NULL;
    st->token = cloneToken(token);
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

ExpressionStatement_t* createExpressionStatement(const Token_t* token) {
    ExpressionStatement_t* st = (ExpressionStatement_t*) malloc(sizeof(ExpressionStatement_t));
    if (st == NULL)
        return NULL;
    st->token = cloneToken(token);
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
