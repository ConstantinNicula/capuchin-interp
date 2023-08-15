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
    EXPRESSION_BOOLEAN_LITERAL,
    EXPRESSION_PREFIX_EXPRESSION,
    EXPRESSION_INFIX_EXPRESSION,
    EXPRESSION_IF_EXPRESSION,
    EXPRESSION_FUNCTION_LITERAL,
    EXPRESSION_CALL_EXPRESSION,
    EXPRESSION_INVALID
} ExpressionType_t;

typedef struct Expression {
    ExpressionType_t type;
    Token_t* token;
} Expression_t;


Expression_t* copyExpression(Expression_t* expr);
void cleanupExpression(Expression_t** expr);

char* expressionToString(Expression_t* expr);
const char* expressionTokenLiteral(Expression_t* expr);

// function pointers for cleanup / expresion to string 
typedef void (*ExpressionCleanupFn_t) (void **);
typedef void* (*ExpressionCopyFn_t) (void *);
typedef char* (*ExpressionToStringFn_t) (void*);

/************************************ 
 *           IDENTIFIER             *
 ************************************/

typedef struct Identifier {
    ExpressionType_t type;
    Token_t* token;
    const char* value;
} Identifier_t;

Identifier_t* createIdentifier(Token_t* tok, const char* val);
Identifier_t* copyIdentifier(Identifier_t* ident);
void cleanupIdentifier(Identifier_t** ident);

char* identifierToString(Identifier_t* ident);


/************************************ 
 *      INTEGER LITERAL             *
 ************************************/

typedef struct IntegerLiteral {
    ExpressionType_t type;
    Token_t* token;
    int64_t value;
}IntegerLiteral_t;

IntegerLiteral_t* createIntegerLiteral(Token_t* tok);
IntegerLiteral_t* copyIntegerLiteral(IntegerLiteral_t* il);
void cleanupIntegerLiteral(IntegerLiteral_t** il);

char* integerLiteralToString(IntegerLiteral_t* il);

/************************************ 
 *          BOOLEAN                 *
 ************************************/

typedef struct BooleanLiteral {
    ExpressionType_t type;
    Token_t* token;
    bool value;
} BooleanLiteral_t;

BooleanLiteral_t* createBooleanLiteral(Token_t* tok);
BooleanLiteral_t* copyBooleanLiteral(BooleanLiteral_t* bl);
void cleanupBooleanLiteral(BooleanLiteral_t** bl);

char* booleanLiteralToString(BooleanLiteral_t* bl);

/************************************ 
 *      PREFIX EXPRESSION           *
 ************************************/

typedef struct PrefixExpression {
    ExpressionType_t type;
    Token_t* token;
    char* operator;
    Expression_t* right;
} PrefixExpression_t;

PrefixExpression_t* createPrefixExpresion(Token_t* tok);
PrefixExpression_t* copyPrefixExpression(PrefixExpression_t* exp);
void cleanupPrefixExpression(PrefixExpression_t** exp);

char* prefixExpressionToString(PrefixExpression_t* exp);

/************************************ 
 *      INFIX EXPRESSION           *
 ************************************/

typedef struct InfixExpression {
    ExpressionType_t type;
    Token_t* token;
    Expression_t* left;
    char* operator;
    Expression_t* right;
} InfixExpression_t;

InfixExpression_t* createInfixExpresion(Token_t* tok);
InfixExpression_t* copyInfixExpression(InfixExpression_t* exp);
void cleanupInfixExpression(InfixExpression_t** exp);

char* infixExpressionToString(InfixExpression_t* exp);


/************************************ 
 *          IF EXPRESSION           *
 ************************************/

typedef struct BlockStatement BlockStatement_t;

typedef struct IfExpression {
    ExpressionType_t type;
    Token_t* token;
    Expression_t* condition;
    BlockStatement_t* consequence;
    BlockStatement_t* alternative;
} IfExpression_t;

IfExpression_t* createIfExpresion(Token_t* tok);
IfExpression_t* copyIfExpression(IfExpression_t* exp);
void cleanupIfExpression(IfExpression_t** exp);

char* ifExpressionToString(IfExpression_t* exp);


/************************************ 
 *    FUNCTION EXPRESSION           *
 ************************************/

typedef struct FunctionLiteral {
    ExpressionType_t type;
    Token_t* token;
    Vector_t* parameters;
    BlockStatement_t* body;
} FunctionLiteral_t;

FunctionLiteral_t* createFunctionLiteral(Token_t* tok);
FunctionLiteral_t* copyFunctionLiteral(FunctionLiteral_t* exp);
void cleanupFunctionLiteral(FunctionLiteral_t** exp);

char* functionLiteralToString(FunctionLiteral_t* exp);
void functionLiteralAppendParameter(FunctionLiteral_t* exp, Identifier_t* param);
uint32_t functionLiteralGetParameterCount(FunctionLiteral_t* exp);
Identifier_t** functionLiteralGetParameters(FunctionLiteral_t* exp);

/************************************ 
 *        CALL EXPRESSION           *
 ************************************/

typedef struct CallExpression {
    ExpressionType_t type;
    Token_t* token;
    Expression_t* function;
    Vector_t* arguments;
} CallExpression_t;


CallExpression_t* createCallExpression(Token_t* tok);
CallExpression_t* copyCallExpression(CallExpression_t* exp);
void cleanupCallExpression(CallExpression_t** exp);

char* callExpressionToString(CallExpression_t* exp);
void callExpressionAppendArgument(CallExpression_t* exp, Expression_t* arg);
uint32_t callExpresionGetArgumentCount(CallExpression_t* exp);
Expression_t** callExpressionGetArguments(CallExpression_t* exp);


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
    Token_t* token;
} Statement_t;


// function pointers for cleanup / expresion to string 
typedef void (*StatementCleanupFn_t) (void **);
typedef void* (*StatementCopyFn_t) (void *);
typedef char* (*StatementToStringFn_t) (void*);

Statement_t* copyStatement(Statement_t* st);
void cleanupStatement(Statement_t** st);

const char* statementTokenLiteral(Statement_t* st);
char* statementToString(Statement_t* st);

/************************************ 
 *         LET STATEMENT            *
 ************************************/

typedef struct LetStatement {
    StatementType_t type;
    Token_t* token;
    Identifier_t* name;
    Expression_t* value; 
} LetStatement_t;

LetStatement_t* createLetStatement(Token_t* token);
LetStatement_t* copyLetStatement(LetStatement_t* st);
void cleanupLetStatement(LetStatement_t** st);

char* letStatementToString(LetStatement_t* st);

/************************************ 
 *      RETURN STATEMENT            *
 ************************************/

typedef struct ReturnStatement {
    StatementType_t type;
    Token_t* token;
    Expression_t* returnValue;
} ReturnStatement_t;

ReturnStatement_t* createReturnStatement(Token_t* token);
ReturnStatement_t* copyReturnStatement(ReturnStatement_t* st);
void cleanupReturnStatement(ReturnStatement_t** st);

char* returnStatementToString(ReturnStatement_t* st);

/************************************ 
 *     EXPRESSION STATEMENT         *
 ************************************/

typedef struct ExpressionStatement {
    StatementType_t type;
    Token_t* token;
    Expression_t* expression;
} ExpressionStatement_t;

ExpressionStatement_t* createExpressionStatement(Token_t* token);
ExpressionStatement_t* copyExpressionStatement(ExpressionStatement_t* st);
void cleanupExpressionStatement(ExpressionStatement_t** st);

char* expressionStatementToString(ExpressionStatement_t* st);


/************************************ 
 *         BLOCK STATEMENT          *
 ************************************/

typedef struct BlockStatement {
    StatementType_t type;
    Token_t* token;
    Vector_t* statements;
} BlockStatement_t;

BlockStatement_t* createBlockStatement(Token_t* token);
BlockStatement_t* copyBlockStatement(BlockStatement_t* st);
void cleanupBlockStatement(BlockStatement_t** st);

char* blockStatementToString(BlockStatement_t* st);
uint32_t blockStatementGetStatementCount(BlockStatement_t* st);
Statement_t** blockStatementGetStatements(BlockStatement_t* st);
void blockStatementAppendStatement(BlockStatement_t* block, Statement_t* st);



/************************************ 
 *      PROGRAM NODE                *
 ************************************/

typedef struct Program {
    Vector_t* statements;
} Program_t;

Program_t* createProgram();
Program_t* copyProgram(Program_t* prog);
void cleanupProgram(Program_t** prog);

Statement_t** programGetStatements(Program_t* prog);
uint32_t programGetStatementCount(Program_t* prog);
void programAppendStatement(Program_t* prog, Statement_t* st);
const char* programTokenLiteral(Program_t* prog);
char* programToString(Program_t* prog);


#endif 