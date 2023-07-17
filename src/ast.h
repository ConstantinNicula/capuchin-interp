#ifndef _AST_H_
#define _AST_H_

#include <stdint.h> 
#include "token.h"

/* This is straight up memory abuse, but all other structs start with the
* same attributes as node, so each other struct can be cast to node in order to 
* access the token literal. This definition should never be explicitly used.
*/
typedef struct Node {
    Token_t* token;
} Node_t;
const char* nodeTokenLiteral(Node_t* node);


/* Definition for Identifier node */
typedef struct Identifier {
    Token_t* token;
    const char* value;
} Identifier_t;

Identifier_t* createIdentifier(Token_t* tok, const char* val);
void cleanupIdentifier(Identifier_t** ident);

/* Definition for Expression */
typedef struct Expression {

} Expression_t;

Identifier_t* createExpression();
void cleanupExpression(Expression_t** expr);


/* Definition for generic Statement struct */
typedef enum StatementType {
    STATEMENT_LET,
    STATEMENT_RETURN,
    STATEMENT_NULL
} StatementType_t;

typedef struct Statement{
    StatementType_t type;
    void* value; // Pointer to specific statement type, must typecast to explicit type :(
} Statement_t;

Statement_t* createStatement(StatementType_t type, void* value);
void cleanupStatement(Statement_t** st);


/* Definition for specific statements */
typedef struct LetStatement {
    Token_t* token;
    Identifier_t* name;
    Expression_t* value; 
} LetStatement_t;


LetStatement_t* createLetStatement(Token_t* token);
void cleanupLetStatement(LetStatement_t** st);

/* Program struct: handles toplevel ast nodes */
typedef struct Program {
    uint32_t statement_cap;
    uint32_t statement_cnt;
    Statement_t** statements;

} Program_t;


Program_t* createProgram();
void cleanupProgram(Program_t** prog);
void programAppendStatement(Program_t* prog, Statement_t* st);
const char* programTokenLiteral(Program_t* prog);

#endif 