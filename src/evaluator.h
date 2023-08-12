#ifndef _EVALUATOR_H_
#define _EVALUATOR_H_

#include "object.h"
#include "ast.h"

Object_t* evalProgram(Program_t* prog);
Object_t* evalStatment(Statement_t* stmt);
Object_t* evalExpression(Expression_t* expr);


#endif