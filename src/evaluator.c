#include "evaluator.h"
#include "utils.h"

static Object_t* evalStatement(Statement_t* stmt, Environment_t* env);
static Object_t* evalBlockStatement(BlockStatement_t* stmt, Environment_t* env);

static Object_t* evalExpression(Expression_t* expr, Environment_t* env);
static Object_t* evalIfExpression(IfExpression_t* expr, Environment_t* env);

static Object_t* evalPrefixExpression(TokenType_t operator, Object_t* right);
static Object_t* evalBangOperatorPrefixExpression(Object_t* right);
static Object_t* evalMinusOperatorPrefixExpression(Object_t* right);
static Object_t* evalInfixExpression(TokenType_t operator, Object_t* left, Object_t* right);
static Object_t* evalIntegerInfixExpression(TokenType_t operator, Integer_t* left, Integer_t* right);

static bool isTruthy(Object_t* obj);
static bool isError(Object_t* obj);


Object_t* evalProgram(Program_t* prog, Environment_t* env) {
    uint32_t count = programGetStatementCount(prog);
    Statement_t** stmts = programGetStatements(prog);
    Object_t* result = NULL;

    for (uint32_t i = 0; i < count; i++) {
        if (result != NULL)
            cleanupObject(&result);

        result = evalStatement(stmts[i], env);
        
        if (!result)
            return (Object_t*) createError(cloneString("evalStatement return NULL ptr"));

        switch(result->type) {
            case OBJECT_RETURN_VALUE: { 
                Object_t* value = ((ReturnValue_t*)result)->value;
                cleanupObject(&result);
                return value;
            }   
            case OBJECT_ERROR: {
                return result;
            }
            default:
                break; // nothing to do  
        }
    }

    return result;

}

static Object_t* evalStatement(Statement_t* stmt, Environment_t* env) {
    switch (stmt->type)
    {
        case STATEMENT_EXPRESSION: 
            return evalExpression(((ExpressionStatement_t*)stmt)->expression, env);
        case STATEMENT_BLOCK: 
            return evalBlockStatement((BlockStatement_t*)stmt, env);
        case STATEMENT_RETURN: {
            Object_t* evalRes = evalExpression(((ReturnStatement_t*)stmt)->returnValue, env);
            if (isError(evalRes)) return evalRes;
            return (Object_t*)createReturnValue(evalRes);
        } 
        case STATEMENT_LET: {
            Object_t* evalRes = evalExpression(((LetStatement_t*)stmt)->value, env);
            if (isError(evalRes)) return evalRes;
            environmentSet(env, ((LetStatement_t*)stmt)->name->value, evalRes);
            cleanupObject(&evalRes);
            return (Object_t*)createNull();
        }
        default:
            return NULL;
    }
}

static Object_t* evalBlockStatement(BlockStatement_t* stmt, Environment_t* env) {
    Statement_t** stmts = blockStatementGetStatements((BlockStatement_t*)stmt);
    uint32_t count = blockStatementGetStatementCount((BlockStatement_t*)stmt);

    Object_t* result = NULL;
    for (uint32_t i = 0; i < count; i++) {
        result = evalStatement(stmts[i], env);
        
        if (result->type == OBJECT_RETURN_VALUE || result->type == OBJECT_ERROR) {
            return result;
        }
        
        if (i < count-1 ){
            cleanupObject(&result);
        }
    }
    return result;
}


static Object_t* evalExpression(Expression_t* expr, Environment_t* env) {
    switch(expr->type) 
    {
        case EXPRESSION_INTEGER_LITERAL: {
            return (Object_t*)createInteger(((IntegerLiteral_t*)expr)->value);
        }

        case EXPRESSION_BOOLEAN_LITERAL: {
            return (Object_t*)createBoolean(((BooleanLiteral_t*)expr)->value);
        }

        case EXPRESSION_IF_EXPRESSION: {
            return (Object_t*)evalIfExpression((IfExpression_t*)expr, env);
        }

        case EXPRESSION_PREFIX_EXPRESSION: {
            TokenType_t op =  ((PrefixExpression_t*) expr)->token->type;
            
            Object_t* evalRight = evalExpression(((PrefixExpression_t*) expr)->right, env);
            if (isError(evalRight)) {
                return evalRight;
            }
            
            Object_t* evalRes = evalPrefixExpression(op, evalRight);
            
            cleanupObject(&evalRight);
            return evalRes;
        }

        case EXPRESSION_INFIX_EXPRESSION: {
            TokenType_t op = ((InfixExpression_t*) expr)->token->type;
            
            Object_t* evalLeft = evalExpression(((InfixExpression_t*) expr)->left, env);
            if (isError(evalLeft)) {
                return evalLeft;
            }

            Object_t* evalRight = evalExpression(((InfixExpression_t*) expr)->right, env);
            if (isError(evalRight)) {
                cleanupObject(&evalLeft);
                return evalRight;
            }

            Object_t* evalRes = evalInfixExpression(op, evalLeft, evalRight);

            cleanupObject(&evalLeft);
            cleanupObject(&evalRight);
            return evalRes;
        }

        case EXPRESSION_IDENTIFIER: {
            Object_t* val = environmentGet(env, ((Identifier_t*)expr)->value);
            if (!val) {
                char* message = strFormat("identifier not found: %s", ((Identifier_t*)expr)->value);
                return (Object_t*)createError(message);
            }
            return val;
        }

        case EXPRESSION_FUNCTION_LITERAL: {
            FunctionLiteral_t* flit = ((FunctionLiteral_t*)expr);
            return (Object_t*) createFunction(flit->parameters, flit->body, env);
        }

        default: 
            char* message = strFormat("unknown expression type: %d(%s)", 
                                    expr->type, 
                                    expr->token->literal);
            return (Object_t*)createError(message);
    }
}


static Object_t* evalIfExpression(IfExpression_t* expr, Environment_t* env) {
    Object_t* condition = evalExpression(expr->condition, env);
    if (isError(condition)) {
        return condition;
    }

    Object_t* evalRes = NULL;
    if (isTruthy(condition)) {
        evalRes = evalStatement((Statement_t*)expr->consequence, env);
    } else if (expr->alternative) {
        evalRes = evalStatement((Statement_t*)expr->alternative, env);
    } else {
        evalRes = (Object_t*)createNull();
    }
    
    cleanupObject(&condition);
    return evalRes; 
}


static Object_t* evalPrefixExpression(TokenType_t operator, Object_t* right) {
    // TO DO use proper enum, easier to switch on token type instead of "operator" field    
    switch(operator) {
        case TOKEN_BANG: 
            return evalBangOperatorPrefixExpression(right);
        case TOKEN_MINUS: 
            return evalMinusOperatorPrefixExpression(right);
        default: 
            char* err = strFormat("unknown operator: %s%s", 
                                    tokenTypeToStr(operator),
                                    objectTypeToString(right->type));
            return (Object_t*)createError(err);
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
        char* message = strFormat("unknown operator: -%s", objectTypeToString(right->type));
        return (Object_t*)createError(message);
    }

    int64_t value = ((Integer_t*)right)->value;
    return (Object_t*) createInteger(-value);
}


static Object_t* evalInfixExpression(TokenType_t operator, Object_t* left, Object_t* right) {
    // Early exit on mismatched types     
    if (left->type != right->type) {
        char* message = strFormat("type mismatch: %s %s %s", 
                            objectTypeToString(left->type), 
                            tokenTypeToStr(operator), 
                            objectTypeToString(right->type));
        return (Object_t*) createError(message);
    }

    // Integers
    if (left->type == OBJECT_INTEGER && right->type == OBJECT_INTEGER) {
        return evalIntegerInfixExpression(operator, (Integer_t*)left, (Integer_t*)right);
    }

    // Booleans
    if (left->type == OBJECT_BOOLEAN && right->type == OBJECT_BOOLEAN) {
        bool leftVal = ((Boolean_t*) left)->value;
        bool rightVal = ((Boolean_t*) right)->value;

        switch(operator) {
            case TOKEN_EQ:
                return (Object_t*) createBoolean(leftVal == rightVal);
            case TOKEN_NOT_EQ:
                return (Object_t*) createBoolean(leftVal != rightVal);
            default: 
                // Do nothing (no op)
                break;
        }
    }

    // No Op found 
    char* message = strFormat("unknown operator: %s %s %s",  
                            objectTypeToString(left->type), 
                            tokenTypeToStr(operator), 
                            objectTypeToString(right->type));
    return (Object_t*) createError(message);
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
            char* message = strFormat("unkown operator: %s %s %s", 
                            objectTypeToString(left->type), 
                            tokenTypeToStr(operator), 
                            objectTypeToString(right->type));
            return (Object_t*)createError(message);
    }
}

static bool isTruthy(Object_t* obj) {
    switch(obj->type) {
        case OBJECT_BOOLEAN:
            return ((Boolean_t*)obj)->value;
        case OBJECT_NULL:
            return false; 
        default:    
            return true;
    }
}

static bool isError(Object_t* obj) {
    if (obj) {
        return obj->type == OBJECT_ERROR;
    }

    return false;
}