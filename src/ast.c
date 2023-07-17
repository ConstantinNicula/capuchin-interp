#include "ast.h"
#include "token.h"
#include <malloc.h>
/* Annoying allocation and cleanup code */

Identifier_t* createIdentifier(Token_t* tok, char* val) {
    Identifier_t* ident = malloc(sizeof(Identifier_t));
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
    free((*ident)->value);
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
            // TO DO: add handling
            break;
        case STATEMENT_NULL:
            // TO DO: add handling 
            break;
        
    }
    (*st)->type = STATEMENT_NULL;
    free(*st);
    *st = NULL;
}



/* Let statement */
LetStatement_t* createLetStatement(Token_t* token) {
    LetStatement_t* st = (LetStatement_t*)malloc(sizeof(LetStatement_t*));
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
}


/* Program */
Program_t* createProgram() {
    Program_t* prog = (Program_t*) malloc(sizeof(Program_t));
    if (prog == NULL)
        return NULL;
    prog->statement_cap = 0u;
    prog->statement_cnt = 0u;
    prog->statements = NULL; // no entries initially 
    return prog;
}

void cleanupProg(Program_t** prog) {
    if (*prog == NULL) 
        return;
    for (uint32_t i = 0; i < (*prog)->statement_cnt; i++)
        cleanupStatement(&(*prog)->statements[i]);
    free((*prog)->statements);
    (*prog)->statement_cap = 0u;
    (*prog)->statement_cnt = 0u;
    free(*prog);
    *prog = NULL;
}

void programAppendStatement(Program_t* prog, Statement_t* st) {
    if (prog->statement_cnt >= prog->statement_cap)
    {
        if (prog->statements != NULL) {
            // no more space, reallocate
            prog->statement_cap = 2*prog->statement_cap;
            prog->statements = (Statement_t**)realloc(prog->statements, sizeof(Statement_t*)*prog->statement_cap);
            if (prog->statements == NULL) {
                return; // TO DO: OOM error 
            }
        }
        else 
        {
            // initial allocation
            prog->statement_cap = 2;
            prog->statements = (Statement_t**)malloc(sizeof(Statement_t) * prog->statement_cap);
        }
    }
    prog->statements[prog->statement_cap++] = st;
}

const char* programTokenLiteral(Program_t* prog) 
{
    if (prog->statement_cnt > 0) 
    {
        return nodeTokenLiteral((Node_t*) prog->statements[0]);
    } else  {
        return "";
    }
}

const char* nodeTokenLiteral(Node_t* node)
{
    return node->token->literal;
}