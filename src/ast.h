#ifndef _AST_H_
#define _AST_H_

#include <stdint.h> 
#include "token.h"
#include "vector.h"

/************************************ 
 *         IDENTIFIER NODE          *
 ************************************/

typedef struct Identifier {
    Token_t* token;
    const char* value;
} Identifier_t;

Identifier_t* createIdentifier(Token_t* tok, const char* val);
void cleanupIdentifier(Identifier_t** ident);

char* identifierToString(Identifier_t* ident);

/************************************ 
 *         EXPRESSION NODE          *
 ************************************/

typedef struct Expression {

} Expression_t;

Identifier_t* createExpression();
void cleanupExpression(Expression_t** expr);

char* expressionToString(Expression_t* expr);


/************************************ 
 *         GENERIC STATEMENT        *
 ************************************/

typedef enum StatementType {
    STATEMENT_LET,
    STATEMENT_RETURN,
    STATEMENT_EXPRESSION,
    STATEMENT_NULL
} StatementType_t;

typedef struct Statement{
    StatementType_t type;
    void* value; // Pointer to specific statement type, must typecast to explicit type :(
} Statement_t;

Statement_t* createStatement(StatementType_t type, void* value);
void cleanupStatement(Statement_t** st);

const char* statementTokenLiteral(Statement_t* st);
char* statementToString(Statement_t* st);

/************************************ 
 *         LET STATEMENT            *
 ************************************/

typedef struct LetStatement {
    Token_t* token;
    Identifier_t* name;
    Expression_t* value; 
} LetStatement_t;

LetStatement_t* createLetStatement(Token_t* token);
void cleanupLetStatement(LetStatement_t** st);

char* letStatementToString(LetStatement_t* st);

/************************************ 
 *      RETURN STATEMENT            *
 ************************************/

typedef struct ReturnStatement {
    Token_t* token;
    Expression_t* returnValue;
} ReturnStatement_t;

ReturnStatement_t* createReturnStatement(Token_t* token);
void cleanupReturnStatement(ReturnStatement_t** st);

char* returnStatementToString(ReturnStatement_t* st);

/************************************ 
 *     EXPRESSION STATEMENT         *
 ************************************/

typedef struct ExpressionStatement {
    Token_t* token;
    Expression_t* expression;
} ExpressionStatement_t;

ExpressionStatement_t* createExpressionStatement(Token_t* token);
void cleanupExpressionStatement(ExpressionStatement_t** st);

char* expressionStatementToString(ExpressionStatement_t* st);

/************************************ 
 *      PROGRAM NODE                *
 ************************************/

typedef struct Program {
    Vector_t* statements;
} Program_t;

Program_t* createProgram();
void cleanupProgram(Program_t** prog);

Statement_t** programGetStatements(Program_t* prog);
uint32_t programGetStatementCount(Program_t* prog);
void programAppendStatement(Program_t* prog, Statement_t* st);
const char* programTokenLiteral(Program_t* prog);
char* programToString(Program_t* prog);


#endif 