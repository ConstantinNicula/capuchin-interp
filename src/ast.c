#include "ast.h"
#include "token.h"
#include <malloc.h>

/* Annoying allocation and cleanup code */
Identifier_t* createIdentifier(Token_t* tok, const char* val) {
    Identifier_t* ident = (Identifier_t*)malloc(sizeof(Identifier_t));
    if (ident == NULL)
        return NULL; 
    ident->token = tok;
    ident->value = val;
    return ident;
}
void cleanupIdentifier(Identifier_t** ident) {
    if (*ident == NULL)
        return;

    free((*ident)->token);
    free((void*)(*ident)->value);
    
    free(*ident);
    *ident = NULL;
}

/* Expression node (TO DO)*/
Identifier_t* createExpression() { return NULL; }
void cleanupExpression(Expression_t** expr) { return;}


/* Generic statement */
Statement_t* createStatement(StatementType_t type, void* value)
{
    Statement_t* st = (Statement_t*)malloc(sizeof(Statement_t));
    if( st == NULL)
        return NULL;
    st->type = type;
    st->value = value;
    return st;
}

void cleanupStatement(Statement_t** st)
{
    if (*st == NULL)
        return;
    switch((*st)->type) {
        case STATEMENT_LET: 
            cleanupLetStatement((LetStatement_t**) &((*st)->value));
            break;
        case STATEMENT_RETURN:
            cleanupReturnStatement((ReturnStatement_t**)&((*st)->value));
            break;
        case STATEMENT_NULL:
            // TO DO: add handling 
            break;
        
    }
    (*st)->type = STATEMENT_NULL;
    free(*st);
    *st = NULL;
}



/* LET STATEMENT functions*/
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


/* RETURN STATEMENT functions*/
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
    cleanupExpression(&(*st)->returnValue);
    cleanupToken(&(*st)->token);
    free(*st);
    *st = NULL;
}

/* PROGRAM top level AST node functions */
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

void cleanupProg(Program_t** prog) {
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

const char* programTokenLiteral(Program_t* prog) 
{
    if (programGetStatementCount(prog) > 0u) 
    {   
        Statement_t** stmts = programGetStatements(prog);
        return statementTokenLiteral(stmts[0]);
    } else  {
        return "";
    }
}


const char* statementTokenLiteral(Statement_t* st) {
    switch (st->type)
    {
        case STATEMENT_LET:
            return tokenCopyLiteral(((LetStatement_t*)st->value)->token);
        case STATEMENT_RETURN:
            return tokenCopyLiteral(((ReturnStatement_t*)st->value)->token);
        default:
            return NULL;
    }
}
