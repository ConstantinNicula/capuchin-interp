#include "evaluator.h"


static Object_t* evalStatment(Statement_t* stmt);
static Object_t* evalExpression(Expression_t* expr);

static Object_t* evalPrefixExpression(TokenType_t operator, Object_t* right);
static Object_t* evalBangOperatorPrefixExpression(Object_t* right);
static Object_t* evalMinusOperatorPrefixExpression(Object_t* right);


static Object_t* evalInfixExpression(TokenType_t operator, Object_t* left, Object_t* right);
static Object_t* evalIntegerInfixExpression(TokenType_t operator, Integer_t* left, Integer_t* right);


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
        case EXPRESSION_INTEGER_LITERAL: {
            return (Object_t*)createInteger(((IntegerLiteral_t*)expr)->value);
        }

        case EXPRESSION_BOOLEAN_LITERAL: {
            return (Object_t*)createBoolean(((BooleanLiteral_t*)expr)->value);
        }

        case EXPRESSION_PREFIX_EXPRESSION: {
            TokenType_t op =  ((PrefixExpression_t*) expr)->token->type;
            Object_t* evalRight = evalExpression(((PrefixExpression_t*) expr)->right);
            Object_t* evalRes = evalPrefixExpression(op, evalRight);
            
            cleanupObject(&evalRight);
            return evalRes;
        }

        case EXPRESSION_INFIX_EXPRESSION: {
            TokenType_t op = ((InfixExpression_t*) expr)->token->type;
            Object_t* evalLeft = evalExpression(((InfixExpression_t*) expr)->left);
            Object_t* evalRight = evalExpression(((InfixExpression_t*) expr)->right);
        
            Object_t* evalRes = evalInfixExpression(op, evalLeft, evalRight);
            cleanupObject(&evalLeft);
            cleanupObject(&evalRight);
            return evalRes;
        }


        default: 
            return NULL;
    }
}


static Object_t* evalPrefixExpression(TokenType_t operator, Object_t* right) {
    // TO DO use proper enum, easier to switch on token type instead of "operator" field    
    switch(operator) {
        case TOKEN_BANG: 
            return evalBangOperatorPrefixExpression(right);
        case TOKEN_MINUS: 
            return evalMinusOperatorPrefixExpression(right);
        default: 
            return NULL;
    }
}

static Object_t* evalBangOperatorPrefixExpression(Object_t* right) {
    switch (right->type) {
        case OBJECT_BOOLEAN:
            return ((Boolean_t*)right)->value? (Object_t*)createBoolean(false): (Object_t*)createBoolean(true);
        case OBJECT_NULL:
            return (Object_t*)createBoolean(true);
        default:    
            return (Object_t*)createBoolean(false);
    }
}


static Object_t* evalMinusOperatorPrefixExpression(Object_t* right) {
    if (right->type != OBJECT_INTEGER) {
        return NULL;
    }

    int64_t value = ((Integer_t*)right)->value;
    return (Object_t*) createInteger(-value);
}


static Object_t* evalInfixExpression(TokenType_t operator, Object_t* left, Object_t* right) {
    
    // Integer only 
    if (left->type == OBJECT_INTEGER && right->type == OBJECT_INTEGER) {
        return evalIntegerInfixExpression(operator, (Integer_t*)left, (Integer_t*)right);
    }

    // Boolean only 
    if (left->type == OBJECT_BOOLEAN && right->type == OBJECT_BOOLEAN) {
        bool leftVal = ((Boolean_t*) left)->value;
        bool rightVal = ((Boolean_t*) right)->value;

        switch(operator) {
            case TOKEN_EQ:
                return (Object_t*) createBoolean(leftVal == rightVal);
            case TOKEN_NOT_EQ:
                return (Object_t*) createBoolean(leftVal != rightVal);
            default: 
                return NULL;
        }
    }

    return NULL;
}

static Object_t* evalIntegerInfixExpression(TokenType_t operator, Integer_t* left, Integer_t* right) {
    switch(operator) {
        case TOKEN_PLUS: 
            return (Object_t*) createInteger(left->value + right->value);
        case TOKEN_MINUS: 
            return (Object_t*) createInteger(left->value - right->value);
        case TOKEN_ASTERISK: 
            return (Object_t*) createInteger(left->value * right->value);
        case TOKEN_SLASH: 
            return (Object_t*) createInteger(left->value / right->value);

        case TOKEN_LT:
            return (Object_t*) createBoolean(left->value < right->value);
        case TOKEN_GT: 
            return (Object_t*) createBoolean(left->value > right->value);
        case TOKEN_EQ:
            return (Object_t*) createBoolean(left->value == right->value);
        case TOKEN_NOT_EQ:
            return (Object_t*) createBoolean(left->value != right->value);
        
        default: 
            return NULL;
    }
}