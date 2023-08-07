#ifndef _AST_H_
#define _AST_H_

#include <stdint.h> 
#include <stdbool.h>
#include "token.h"
#include "vector.h"


/************************************ 
 *     GENERIC EXPRESSION NODE      *
 ************************************/

typedef enum ExpressionType {
    EXPRESSION_IDENTIFIER,
    EXPRESSION_INTEGER_LITERAL,
    EXPRESSION_BOOLEAN,
    EXPRESSION_PREFIX_EXPRESSION,
    EXPRESSION_INFIX_EXPRESSION,
    EXPRESSION_INVALID
} ExpressionType_t;

typedef struct Expression {
    ExpressionType_t type;
    // Pointer to specific expression type, must be typecast to explicit type
    void* value; 
} Expression_t;

Expression_t* createExpression(ExpressionType_t type, void* value);
void cleanupExpression(Expression_t** expr);

char* expressionToString(Expression_t* expr);
const char* expressionTokenLiteral(Expression_t* expr);

// function pointers for cleanup / expresion to string 
typedef void (*ExpressionCleanupFn_t) (void **);
typedef char* (*ExpressionToStringFn_t) (void*);

/************************************ 
 *           IDENTIFIER             *
 ************************************/

typedef struct Identifier {
    Token_t* token;
    const char* value;
} Identifier_t;

Identifier_t* createIdentifier(const Token_t* tok, const char* val);
void cleanupIdentifier(Identifier_t** ident);

char* identifierToString(Identifier_t* ident);


/************************************ 
 *      INTEGER LITERAL             *
 ************************************/

typedef struct IntegerLiteral {
    Token_t* token; 
    int64_t value;
}IntegerLiteral_t;

IntegerLiteral_t* createIntegerLiteral(const Token_t* tok);
void cleanupIntegerLiteral(IntegerLiteral_t** il);

char* integerLiteralToString(IntegerLiteral_t* il);

/************************************ 
 *          BOOLEAN                 *
 ************************************/

typedef struct Boolean {
    Token_t* token;
    bool value;
} Boolean_t;

Boolean_t* createBoolean(const Token_t* tok);
void cleanupBoolean(Boolean_t** bl);

char* booleanToString(Boolean_t* bl);



/************************************ 
 *      PREFIX EXPRESSION           *
 ************************************/

typedef struct PrefixExpression {
    Token_t* token;
    char* operator;
    Expression_t* right;
} PrefixExpression_t;

PrefixExpression_t* createPrefixExpresion(const Token_t* tok);
void cleanupPrefixExpression(PrefixExpression_t** exp);

char* prefixExpressionToString(PrefixExpression_t* exp);

/************************************ 
 *      INFIX EXPRESSION           *
 ************************************/

typedef struct InfixExpression {
    Token_t* token;
    Expression_t* left;
    char* operator;
    Expression_t* right;
} InfixExpression_t;

InfixExpression_t* createInfixExpresion(const Token_t* tok);
void cleanupInfixExpression(InfixExpression_t** exp);

char* infixExpressionToString(InfixExpression_t* exp);


/************************************ 
 *         GENERIC STATEMENT        *
 ************************************/

typedef enum StatementType {
    STATEMENT_LET,
    STATEMENT_RETURN,
    STATEMENT_EXPRESSION,
    STATEMENT_BLOCK,
    STATEMENT_INVALID
} StatementType_t;

typedef struct Statement{
    StatementType_t type;
    // Pointer to specific statement type, must typecast to explicit type :(
    void* value; 
} Statement_t;


// function pointers for cleanup / expresion to string 
typedef void (*StatementCleanupFn_t) (void **);
typedef char* (*StatementToStringFn_t) (void*);


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

LetStatement_t* createLetStatement(const Token_t* token);
void cleanupLetStatement(LetStatement_t** st);

char* letStatementToString(LetStatement_t* st);

/************************************ 
 *      RETURN STATEMENT            *
 ************************************/

typedef struct ReturnStatement {
    Token_t* token;
    Expression_t* returnValue;
} ReturnStatement_t;

ReturnStatement_t* createReturnStatement(const Token_t* token);
void cleanupReturnStatement(ReturnStatement_t** st);

char* returnStatementToString(ReturnStatement_t* st);

/************************************ 
 *     EXPRESSION STATEMENT         *
 ************************************/

typedef struct ExpressionStatement {
    Token_t* token;
    Expression_t* expression;
} ExpressionStatement_t;

ExpressionStatement_t* createExpressionStatement(const Token_t* token);
void cleanupExpressionStatement(ExpressionStatement_t** st);

char* expressionStatementToString(ExpressionStatement_t* st);


/************************************ 
 *         BLOCK STATEMENT          *
 ************************************/

typedef struct BlockStatement {
    Token_t* token;
    Vector_t* statements;
} BlockStatement_t;

BlockStatement_t* createBlockStatement(const Token_t* token);
void cleanupBlockStatement(BlockStatement_t** st);

char* blockStatementToString(BlockStatement_t* st);


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