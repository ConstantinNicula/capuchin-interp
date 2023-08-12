#include "evaluator.h"


static Object_t* evalStatment(Statement_t* stmt);
static Object_t* evalExpression(Expression_t* expr);

static Object_t* evalPrefixExpression(TokenType_t operator, Object_t* right);
static Object_t* evalBangOperatorExpression(Object_t* right);



Object_t* evalProgram(Program_t* prog) {
    uint32_t stmtsCount = programGetStatementCount(prog);
    Statement_t** stmts = programGetStatements(prog);
    
    Object_t* result = NULL;
    for (uint32_t i = 0; i < stmtsCount; i++) {
        result = evalStatment(stmts[i]);
    }
    return result;
}


static Object_t* evalStatment(Statement_t* stmt) {
    switch (stmt->type)
    {
        case STATEMENT_EXPRESSION: 
            return evalExpression(((ExpressionStatement_t*)stmt)->expression);
        default:
            return NULL;
    }
}


static Object_t* evalExpression(Expression_t* expr) {
    switch(expr->type) 
    {
        case EXPRESSION_INTEGER_LITERAL:
            return (Object_t*)createInteger(((IntegerLiteral_t*)expr)->value);
        case EXPRESSION_BOOLEAN_LITERAL: 
            return (Object_t*)createBoolean(((BooleanLiteral_t*)expr)->value);
        case EXPRESSION_PREFIX_EXPRESSION: 
            TokenType_t op =  ((PrefixExpression_t*) expr)->token->type;
            Object_t* evalRight = evalExpression(((PrefixExpression_t*) expr)->right);
            Object_t* evalRes = evalPrefixExpression(op, evalRight);
            
            cleanupObject(&evalRight);
            return evalRes;
        default: 
            return NULL;
    }
}


static Object_t* evalPrefixExpression(TokenType_t operator, Object_t* right) {
    // TO DO use proper enum, easier to switch on token type instead of "operator" field    
    switch(operator) {
        case TOKEN_BANG: 
            return evalBangOperatorExpression(right);
        default: 
            return NULL;
    }
}

static Object_t* evalBangOperatorExpression(Object_t* right) {
    switch (right->type) {
        case OBJECT_BOOLEAN:
            return ((Boolean_t*)right)->value? (Object_t*)createBoolean(false): (Object_t*)createBoolean(true);
        case OBJECT_NULL:
            return (Object_t*)createBoolean(true);
        default:    
            return (Object_t*)createBoolean(false);
    }
}
