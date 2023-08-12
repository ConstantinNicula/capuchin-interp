#include "evaluator.h"

Object_t* evalProgram(Program_t* prog) {
    uint32_t stmtsCount = programGetStatementCount(prog);
    Statement_t** stmts = programGetStatements(prog);
    
    Object_t* result = NULL;
    for (uint32_t i = 0; i < stmtsCount; i++) {
        result = evalStatment(stmts[i]);
    }
    return result;
}


Object_t* evalStatment(Statement_t* stmt) {
    switch (stmt->type)
    {
        case STATEMENT_EXPRESSION: 
            return evalExpression(((ExpressionStatement_t*)stmt)->expression);
        default:
            return NULL;
    }
}

Object_t* evalExpression(Expression_t* expr) {
    switch(expr->type) 
    {
        case EXPRESSION_INTEGER_LITERAL:
            return (Object_t*)createInteger(((IntegerLiteral_t*)expr)->value);
        case EXPRESSION_BOOLEAN_LITERAL: 
            return (Object_t*)createBoolean(((BooleanLiteral_t*)expr)->value);
        default: 
            return NULL;
    }
}
