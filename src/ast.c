#include <malloc.h>

#include "ast.h"
#include "token.h"
#include "utils.h"
#include "sbuf.h"
#include "refcount.h"

/************************************ 
 *       COMMON UTILS DEF           *
 ************************************/

static void cleanupStatementVec(Vector_t** statements);
static char* statementVecToString(Vector_t* statements);


/************************************ 
 *         EXPRESSION NODE          *
 ************************************/

static ExpressionCleanupFn_t expressionCleanupFns[] = {
    [EXPRESSION_IDENTIFIER]=(ExpressionCleanupFn_t)cleanupIdentifier,
    [EXPRESSION_INTEGER_LITERAL]=(ExpressionCleanupFn_t)cleanupIntegerLiteral,
    [EXPRESSION_BOOLEAN_LITERAL]=(ExpressionCleanupFn_t)cleanupBooleanLiteral,
    [EXPRESSION_PREFIX_EXPRESSION]=(ExpressionCleanupFn_t)cleanupPrefixExpression,
    [EXPRESSION_INFIX_EXPRESSION]=(ExpressionCleanupFn_t)cleanupInfixExpression,
    [EXPRESSION_IF_EXPRESSION]=(ExpressionCleanupFn_t)cleanupIfExpression,
    [EXPRESSION_FUNCTION_LITERAL]=(ExpressionCleanupFn_t)cleanupFunctionLiteral,
    [EXPRESSION_CALL_EXPRESSION]=(ExpressionCleanupFn_t)cleanupCallExpression,
    [EXPRESSION_INVALID]=NULL
};

static ExpressionCopyFn_t expressionCopyFns[] = {
    [EXPRESSION_IDENTIFIER]=(ExpressionCopyFn_t)copyIdentifier,
    [EXPRESSION_INTEGER_LITERAL]=(ExpressionCopyFn_t)copyIntegerLiteral,
    [EXPRESSION_BOOLEAN_LITERAL]=(ExpressionCopyFn_t)copyBooleanLiteral,
    [EXPRESSION_PREFIX_EXPRESSION]=(ExpressionCopyFn_t)copyPrefixExpression,
    [EXPRESSION_INFIX_EXPRESSION]=(ExpressionCopyFn_t)copyInfixExpression,
    [EXPRESSION_IF_EXPRESSION]=(ExpressionCopyFn_t)copyIfExpression,
    [EXPRESSION_FUNCTION_LITERAL]=(ExpressionCopyFn_t)copyFunctionLiteral,
    [EXPRESSION_CALL_EXPRESSION]=(ExpressionCopyFn_t)copyCallExpression,
    [EXPRESSION_INVALID]=NULL
};


static ExpressionToStringFn_t expressionToStringFns[] = {
    [EXPRESSION_IDENTIFIER]=(ExpressionToStringFn_t)identifierToString,
    [EXPRESSION_INTEGER_LITERAL]=(ExpressionToStringFn_t)integerLiteralToString,
    [EXPRESSION_BOOLEAN_LITERAL]=(ExpressionToStringFn_t)booleanLiteralToString,
    [EXPRESSION_PREFIX_EXPRESSION]=(ExpressionToStringFn_t)prefixExpressionToString,
    [EXPRESSION_INFIX_EXPRESSION]=(ExpressionToStringFn_t)infixExpressionToString,
    [EXPRESSION_IF_EXPRESSION]=(ExpressionToStringFn_t)ifExpressionToString,
    [EXPRESSION_FUNCTION_LITERAL]=(ExpressionToStringFn_t)functionLiteralToString,
    [EXPRESSION_CALL_EXPRESSION]=(ExpressionToStringFn_t)callExpressionToString,
    [EXPRESSION_INVALID]=NULL
};


void cleanupExpression(Expression_t** expr) { 
    if (*expr == NULL)
        return;
    
    if ((*expr)->type  >= 0  && (*expr)->type < EXPRESSION_INVALID) {
        ExpressionCleanupFn_t cleanupFn = expressionCleanupFns[(*expr)->type];
        cleanupFn((void**)expr);
    }
}

Expression_t* expressionCopy(Expression_t* expr) {
    if (expr && expr->type  >= 0 && expr->type < EXPRESSION_INVALID) {
        ExpressionCopyFn_t copyFn = expressionCopyFns[expr->type];
        return copyFn((void*)expr);
    }
    return NULL;
}

char* expressionToString(Expression_t* expr) {
    if (expr && expr->type  >= 0 && expr->type < EXPRESSION_INVALID) {
        ExpressionToStringFn_t toStringFn = expressionToStringFns[expr->type];
        return toStringFn(expr);
    }
    return cloneString("");
}

const char* expressionTokenLiteral(Expression_t* expr) {
    if (expr && expr->type  >= 0 && expr->type < EXPRESSION_INVALID) {
        return expr->token->literal; 
    }
    return "";
}


/************************************ 
 *           IDENTIFIER             *
 ************************************/

Identifier_t* createIdentifier(Token_t* tok, const char* val) {
    Identifier_t* ident = createRefCountPtr(sizeof(Identifier_t));

    *ident =  (Identifier_t) {
        .type = EXPRESSION_IDENTIFIER,
        .token = cloneToken(tok),
        .value = cloneString(val)
    };

    return ident;
}

Identifier_t* copyIdentifier(Identifier_t* ident){
    refCountPtrInc(ident);
    return ident;
}

void cleanupIdentifier(Identifier_t** ident) {
    if (!(*ident) || refCountPtrDec(*ident) != 0)
        return;

    cleanupToken(&(*ident)->token);
    free((void*)(*ident)->value);
    
    cleanupRefCountedPtr(*ident);
    *ident = NULL;
}

char* identifierToString(Identifier_t* ident) {
    // Return a copy to avoid free errors
    return cloneString(ident->value);
}


/************************************ 
 *      INTEGER LITERAL             *
 ************************************/

IntegerLiteral_t* createIntegerLiteral(Token_t* tok) {
    IntegerLiteral_t* il = createRefCountPtr(sizeof(IntegerLiteral_t));

    *il = (IntegerLiteral_t) {
        .type = EXPRESSION_INTEGER_LITERAL,
        .token = cloneToken(tok),
        .value = 0
    };
    return il;
}

IntegerLiteral_t* copyIntegerLiteral(IntegerLiteral_t* il){
    refCountPtrInc(il);
    return il;
}

void cleanupIntegerLiteral(IntegerLiteral_t** il) {
    if (!(*il) || refCountPtrDec(*il) != 0)
        return;
    
    cleanupToken(&(*il)->token);
    
    cleanupRefCountedPtr(*il);
    *il = NULL;
}

char* integerLiteralToString(IntegerLiteral_t* il) {
    return cloneString(il->token->literal);
}


/************************************ 
 *          BOOLEAN                 *
 ************************************/

BooleanLiteral_t* createBooleanLiteral(Token_t* tok) {
    BooleanLiteral_t* bl = createRefCountPtr(sizeof(BooleanLiteral_t));

    *bl = (BooleanLiteral_t) {
        .type = EXPRESSION_BOOLEAN_LITERAL,
        .token = cloneToken(tok),
        .value = false
    };

    return bl;
}

BooleanLiteral_t* copyBooleanLiteral(BooleanLiteral_t* bl) {
    refCountPtrInc(bl);
    return bl;
}

void cleanupBooleanLiteral(BooleanLiteral_t** bl) {
    if (!(*bl) || refCountPtrDec(*bl) != 0)
        return;
    
    cleanupToken(&(*bl)->token);

    cleanupRefCountedPtr(*bl);
    *bl = NULL;
}

char* booleanLiteralToString(BooleanLiteral_t* bl) {
    return bl->value ? cloneString("true") : cloneString("false");
}



/************************************ 
 *      PREFIX EXPRESSION           *
 ************************************/

PrefixExpression_t* createPrefixExpresion(Token_t* tok) {
    PrefixExpression_t* exp = createRefCountPtr(sizeof(PrefixExpression_t));

    *exp = (PrefixExpression_t) {
        .type = EXPRESSION_PREFIX_EXPRESSION,
        .token = cloneToken(tok),
        .operator = NULL,
        .right = NULL
    };

    return exp;
}

PrefixExpression_t* copyPrefixExpression(PrefixExpression_t* exp) {
    refCountPtrInc(exp);
    return exp;
}

void cleanupPrefixExpression(PrefixExpression_t** exp) {
    if (!(*exp) || refCountPtrDec(*exp) != 0)
        return;

    cleanupToken(&(*exp)->token);
    free((*exp)->operator);
    cleanupExpression(&(*exp)->right);
    
    cleanupRefCountedPtr(*exp);
    *exp = NULL;
}

char* prefixExpressionToString(PrefixExpression_t* exp) {
    Strbuf_t* sbuf = createStrbuf();

    strbufWrite(sbuf, "(");
    strbufWrite(sbuf, exp->operator);
    strbufConsume(sbuf, expressionToString(exp->right));
    strbufWrite(sbuf, ")");

    return detachStrbuf(&sbuf);
}



/************************************ 
 *      INFIX EXPRESSION           *
 ************************************/

InfixExpression_t* createInfixExpresion(Token_t* tok) {
    InfixExpression_t* exp = createRefCountPtr(sizeof(InfixExpression_t));

    *exp = (InfixExpression_t) {
        .type = EXPRESSION_INFIX_EXPRESSION,
        .token = cloneToken(tok),
        .left = NULL, 
        .operator = NULL, 
        .right = NULL
    };

    return exp;
}

InfixExpression_t* copyInfixExpression(InfixExpression_t* exp) {
    refCountPtrInc(exp);
    return exp;
}

void cleanupInfixExpression(InfixExpression_t** exp) {
    if (!(*exp) || refCountPtrDec(*exp) != 0)
        return;
    
    cleanupToken(&(*exp)->token);
    cleanupExpression(&(*exp)->left);
    free((*exp)->operator);
    cleanupExpression(&(*exp)->right);

    cleanupRefCountedPtr(*exp);
    *exp = NULL;
}

char* infixExpressionToString(InfixExpression_t* exp) {
    Strbuf_t* sbuf = createStrbuf();

    strbufWrite(sbuf, "(");
    strbufConsume(sbuf, expressionToString(exp->left));
    strbufWrite(sbuf, " ");
    strbufWrite(sbuf, exp->operator);
    strbufWrite(sbuf, " ");
    strbufConsume(sbuf, expressionToString(exp->right));
    strbufWrite(sbuf, ")");

    return detachStrbuf(&sbuf);
}


/************************************ 
 *          IF EXPRESSION           *
 ************************************/

IfExpression_t* createIfExpresion(Token_t* tok) {
    IfExpression_t* exp = createRefCountPtr(sizeof(IfExpression_t));

    *exp = (IfExpression_t) {
        .type = EXPRESSION_IF_EXPRESSION, 
        .token = cloneToken(tok),
        .condition = NULL, 
        .consequence = NULL, 
        .alternative = NULL
    };

    return exp;
}

IfExpression_t* copyIfExpression(IfExpression_t* exp) {
    refCountPtrInc(exp);
    return exp;
}

void cleanupIfExpression(IfExpression_t** exp) {
    if (!(*exp) || refCountPtrDec(*exp) != 0)
        return;

    cleanupToken(&(*exp)->token);
    cleanupExpression(&(*exp)->condition);
    cleanupBlockStatement(&(*exp)->consequence);
    cleanupBlockStatement(&(*exp)->alternative);

    cleanupRefCountedPtr(*exp);
    *exp = NULL;
}

char* ifExpressionToString(IfExpression_t* exp) {
    Strbuf_t* sbuf = createStrbuf();

    strbufWrite(sbuf, "if");
    strbufConsume(sbuf, expressionToString(exp->condition));
    strbufWrite(sbuf, " ");
    strbufConsume(sbuf, blockStatementToString(exp->consequence));

    if (exp->alternative) {
        strbufWrite(sbuf, "else ");
        strbufConsume(sbuf, blockStatementToString(exp->consequence));
    }

    return detachStrbuf(&sbuf);
}



/************************************ 
 *    FUNCTION EXPRESSION           *
 ************************************/

FunctionLiteral_t* createFunctionLiteral(Token_t* tok) {
    FunctionLiteral_t *exp = createRefCountPtr(sizeof(FunctionLiteral_t));

    *exp = (FunctionLiteral_t) {
        .type = EXPRESSION_FUNCTION_LITERAL, 
        .token = cloneToken(tok),
        .parameters = createVector(sizeof(Identifier_t*)),
        .body = NULL
    };

    return exp;
}

FunctionLiteral_t* copyFunctionLiteral(FunctionLiteral_t* exp) {
    refCountPtrInc(exp);
    return exp;
}

void cleanupFunctionLiteral(FunctionLiteral_t** exp) {
    if (!(*exp) || refCountPtrDec(*exp) != 0)
        return;

    cleanupToken(&(*exp)->token);
    cleanupVector(&(*exp)->parameters, (VectorElemCleanupFn_t)cleanupIdentifier);
    cleanupBlockStatement(&(*exp)->body);

    cleanupRefCountedPtr(*exp);
    *exp = NULL;
}

char* functionLiteralToString(FunctionLiteral_t* exp) {
    Strbuf_t* sbuf = createStrbuf();

    strbufWrite(sbuf, exp->token->literal);
    strbufWrite(sbuf, "(");
    uint32_t paramCnt = functionLiteralGetParameterCount(exp);
    Identifier_t** params = functionLiteralGetParameters(exp);
    for (uint32_t i = 0; i < paramCnt; i++) {
        strbufConsume(sbuf, identifierToString(params[i]));
    }
    strbufWrite(sbuf, ")");
    strbufConsume(sbuf, blockStatementToString(exp->body));

    return detachStrbuf(&sbuf);
}

void functionLiteralAppendParameter(FunctionLiteral_t* exp, Identifier_t* param) {
    vectorAppend(exp->parameters, (void*) &param);
}

uint32_t functionLiteralGetParameterCount(FunctionLiteral_t* exp) {
    return vectorGetCount(exp->parameters);
}

Identifier_t** functionLiteralGetParameters(FunctionLiteral_t* exp) {
    return (Identifier_t**) vectorGetBuffer(exp->parameters);
}


/************************************ 
 *        CALL EXPRESSION           *
 ************************************/

CallExpression_t* createCallExpression(Token_t* tok) {
    CallExpression_t* exp = createRefCountPtr(sizeof(CallExpression_t));

    *exp = (CallExpression_t) {
        .type = EXPRESSION_CALL_EXPRESSION, 
        .token =  cloneToken(tok),
        .function = NULL, 
        .arguments = createVector(sizeof(Expression_t*))
    };
 
    return exp;
}

CallExpression_t* copyCallExpression(CallExpression_t* exp) {
    refCountPtrInc(exp);
    return exp;
}

void cleanupCallExpression(CallExpression_t** exp) {
    if (!(*exp) || refCountPtrDec(*exp) != 0)
        return;
    
    cleanupToken(&(*exp)->token);
    cleanupExpression(&(*exp)->function);
    cleanupVector(&(*exp)->arguments,(VectorElemCleanupFn_t) cleanupExpression);

    cleanupRefCountedPtr(*exp);
    *exp = NULL;
}

char* callExpressionToString(CallExpression_t* exp) {
    Strbuf_t* sbuf = createStrbuf();

    strbufConsume(sbuf, expressionToString(exp->function));
    strbufWrite(sbuf, "(");
    
    uint32_t argCnt = callExpresionGetArgumentCount(exp);
    Expression_t** args = callExpressionGetArguments(exp);

    for (uint32_t i = 0; i < argCnt; i++) {
        strbufConsume(sbuf, expressionToString(args[i]));
        if (i != argCnt - 1) {
            strbufWrite(sbuf, ", ");
        }
    }

    strbufWrite(sbuf, ")");
    return detachStrbuf(&sbuf);
}


void callExpressionAppendArgument(CallExpression_t* exp, Expression_t* arg) {
    vectorAppend(exp->arguments, (void*) &arg);
}
uint32_t callExpresionGetArgumentCount(CallExpression_t* exp) {
    return vectorGetCount(exp->arguments);
}

Expression_t** callExpressionGetArguments(CallExpression_t* exp) {
    return (Expression_t**) vectorGetBuffer(exp->arguments);
}





/************************************ 
 *         GENERIC STATEMENT        *
 ************************************/

static StatementCleanupFn_t statementCleanupFns[] = {
    [STATEMENT_LET]=(StatementCleanupFn_t)cleanupLetStatement,
    [STATEMENT_RETURN]=(StatementCleanupFn_t)cleanupReturnStatement,
    [STATEMENT_EXPRESSION]=(StatementCleanupFn_t)cleanupExpressionStatement,
    [STATEMENT_BLOCK]=(StatementCleanupFn_t)cleanupBlockStatement,
    [STATEMENT_INVALID]=NULL
};

static StatementCopyFn_t statementCopyFns[] = {
    [STATEMENT_LET]=(StatementCopyFn_t)copyLetStatement,
    [STATEMENT_RETURN]=(StatementCopyFn_t)copyReturnStatement,
    [STATEMENT_EXPRESSION]=(StatementCopyFn_t)copyExpressionStatement,
    [STATEMENT_BLOCK]=(StatementCopyFn_t)copyBlockStatement,
    [STATEMENT_INVALID]=NULL
};


static StatementToStringFn_t statementToStringFns[] = {
    [STATEMENT_LET]=(StatementToStringFn_t)letStatementToString,
    [STATEMENT_RETURN]=(StatementToStringFn_t)returnStatementToString,
    [STATEMENT_EXPRESSION]=(StatementToStringFn_t)expressionStatementToString,
    [STATEMENT_BLOCK]=(StatementToStringFn_t)blockStatementToString,
    [STATEMENT_INVALID]=NULL
};



void cleanupStatement(Statement_t** st) {
    if (*st == NULL)
        return;

    if ((*st)->type >= 0 &&  (*st)->type < STATEMENT_INVALID) {
        StatementCleanupFn_t cleanupFn = statementCleanupFns[(*st)->type];
        cleanupFn((void**) st);
    }
}

Statement_t* copyStatement(Statement_t* st) {
    if (st && (st->type >= 0) && (st->type <= STATEMENT_INVALID) ) {
        StatementCopyFn_t copyFn = statementCopyFns[st->type];
        return copyFn(st);
    }
    return NULL;
}

const char* statementTokenLiteral(Statement_t* st) {
    if (st && st->type >= 0 && st->type <STATEMENT_INVALID ) {
        return st->token->literal;
    }
    return "";
}

char* statementToString(Statement_t* st) {
    if (st && (st->type >= 0) && (st->type <=STATEMENT_INVALID) ) {
        StatementToStringFn_t toStringFn = statementToStringFns[st->type];
        return toStringFn(st);
    }

    return cloneString("");
}


/************************************ 
 *         LET STATEMENT            *
 ************************************/

LetStatement_t* createLetStatement(Token_t* token) {
    LetStatement_t* st = createRefCountPtr(sizeof(LetStatement_t));

    *st = (LetStatement_t) {
        .type = STATEMENT_LET, 
        .token = cloneToken(token),
        .name = NULL,
        .value = NULL
    };

    return st;
}

LetStatement_t* copyLetStatement(LetStatement_t* st) {
    refCountPtrInc(st);
    return st;
}

void cleanupLetStatement(LetStatement_t** st) {
    if (!(*st) || refCountPtrDec(*st) != 0)
        return;

    cleanupToken(&(*st)->token);
    cleanupIdentifier(&(*st)->name);
    cleanupExpression(&(*st)->value);
    
    cleanupRefCountedPtr(*st);
    *st = NULL;
}

char* letStatementToString(LetStatement_t* st) {
    Strbuf_t* sbuf = createStrbuf();
    
    strbufWrite(sbuf, st->token->literal);
    strbufWrite(sbuf, " ");
    strbufConsume(sbuf, identifierToString(st->name));
    strbufWrite(sbuf, " = ");
    if (st->value != NULL) {
        strbufWrite(sbuf, expressionToString(st->value));
    }
    strbufWrite(sbuf, ";");

    return detachStrbuf(&sbuf);
}


/************************************ 
 *      RETURN STATEMENT            *
 ************************************/

ReturnStatement_t* createReturnStatement(Token_t* token) {
    ReturnStatement_t* st = createRefCountPtr(sizeof(ReturnStatement_t));
    
    *st = (ReturnStatement_t) {
        .type = STATEMENT_RETURN,
        .token = cloneToken(token),
        .returnValue = NULL 
    };

    return st;
}

ReturnStatement_t* copyReturnStatement(ReturnStatement_t* st) {
    refCountPtrInc(st);
    return st;
}

void cleanupReturnStatement(ReturnStatement_t** st) {
    if (!(*st) || refCountPtrDec(*st) != 0)
        return;
    
    cleanupToken(&(*st)->token);
    cleanupExpression(&(*st)->returnValue);
    
    cleanupRefCountedPtr(*st);
    *st = NULL;
}

char* returnStatementToString(ReturnStatement_t* st) {
    Strbuf_t* sbuf = createStrbuf();

    strbufWrite(sbuf, st->token->literal);
    strbufWrite(sbuf, " ");

    if (st->returnValue != NULL) {
        strbufConsume(sbuf, expressionToString(st->returnValue));
    }
    strbufWrite(sbuf, ";");

    return detachStrbuf(&sbuf);
}

/************************************ 
 *      EXPRESSION STATEMENT        *
 ************************************/

ExpressionStatement_t* createExpressionStatement(Token_t* token) {
    ExpressionStatement_t* st = createRefCountPtr(sizeof(ExpressionStatement_t));

    *st = (ExpressionStatement_t) {
        .type = STATEMENT_EXPRESSION, 
        .token = cloneToken(token),
        .expression = NULL
    };

    return st;
}

ExpressionStatement_t* copyExpressionStatement(ExpressionStatement_t* st) {
    refCountPtrInc(st);
    return st;
}

void cleanupExpressionStatement(ExpressionStatement_t** st) {
    if (!(*st) || refCountPtrDec(*st) != 0)
        return;

    cleanupToken(&(*st)->token);
    cleanupExpression(&(*st)->expression);

    cleanupRefCountedPtr(*st);
    *st = NULL;
}

char* expressionStatementToString(ExpressionStatement_t* st) {
    if (st->expression != NULL) {
        return expressionToString(st->expression);
    }
    return cloneString("");
}



/************************************ 
 *         BLOCK STATEMENT          *
 ************************************/

BlockStatement_t* createBlockStatement(Token_t* token) {
    BlockStatement_t* st = createRefCountPtr(sizeof(BlockStatement_t));

    *st = (BlockStatement_t) {
        .type = STATEMENT_BLOCK, 
        .token = cloneToken(token),
        .statements = createVector(sizeof(Statement_t*))
    };

    return st;
}

BlockStatement_t* copyBlockStatement(BlockStatement_t* st) {
    refCountPtrInc(st);
    return st;
}

void cleanupBlockStatement(BlockStatement_t** st) {
    if (!(*st) || refCountPtrDec(*st) != 0)
        return;
    
    cleanupToken(&(*st)->token);
    cleanupStatementVec(&(*st)->statements);
    
    cleanupRefCountedPtr(*st);
    *st = NULL;
}

char* blockStatementToString(BlockStatement_t* st) {
    return statementVecToString(st->statements);
}

uint32_t blockStatementGetStatementCount(BlockStatement_t* st) {
    return vectorGetCount(st->statements);
}

Statement_t** blockStatementGetStatements(BlockStatement_t* st) {
    return (Statement_t**) vectorGetBuffer(st->statements);
}

void blockStatementAppendStatement(BlockStatement_t* block, Statement_t* st) {
    vectorAppend(block->statements, (void*)&st);
}


/************************************ 
 *      PROGRAM NODE                *
 ************************************/

Program_t* createProgram() {
    Program_t* prog = (Program_t*) malloc(sizeof(Program_t));
    if (prog == NULL)
        return NULL;
    prog->statements = createVector(sizeof(Statement_t*));
    return prog;
}

Statement_t** programGetStatements(Program_t* prog) {
    return (Statement_t**)vectorGetBuffer(prog->statements);
}

uint32_t programGetStatementCount(Program_t* prog) {
    return vectorGetCount(prog->statements);
}

Program_t* copyProgram(Program_t* prog) {
    refCountPtrInc(prog);
    return prog;
}

void cleanupProgram(Program_t** prog) {
    if (*prog == NULL) 
        return;

    cleanupStatementVec(&(*prog)->statements);
    
    free(*prog);
    *prog = NULL;
}

void programAppendStatement(Program_t* prog, Statement_t* st) {
    vectorAppend(prog->statements, (void*)&st);
}

const char* programTokenLiteral(Program_t* prog) {
    if (programGetStatementCount(prog) > 0u) 
    {   
        Statement_t** stmts = programGetStatements(prog);
        return statementTokenLiteral(stmts[0]);
    } else  {
        return "";
    }
}


char* programToString(Program_t* prog) {
    return statementVecToString(prog->statements);
}


/************************************ 
 *         COMMON UTILS              *
 ************************************/

static void cleanupStatementVec(Vector_t** statements) {
    cleanupVector(statements, (VectorElemCleanupFn_t)cleanupStatement);
    *statements = NULL;
}


static char* statementVecToString(Vector_t* statements) {
    Strbuf_t* sbuf = createStrbuf();
    
    int32_t cnt = vectorGetCount(statements);
    Statement_t** stmts = (Statement_t**) vectorGetBuffer(statements);

    for (uint32_t i = 0; i < cnt; i++) {
        strbufConsume(sbuf, statementToString(stmts[i]));
    }

    return detachStrbuf(&sbuf);
}