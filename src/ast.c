#include <malloc.h>

#include "ast.h"
#include "token.h"
#include "utils.h"
#include "sbuf.h"


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
    [EXPRESSION_BOOLEAN]=(ExpressionCleanupFn_t)cleanupBoolean,
    [EXPRESSION_PREFIX_EXPRESSION]=(ExpressionCleanupFn_t)cleanupPrefixExpression,
    [EXPRESSION_INFIX_EXPRESSION]=(ExpressionCleanupFn_t)cleanupInfixExpression,
    [EXPRESSION_IF_EXPRESSION]=(ExpressionCleanupFn_t)cleanupIfExpression,
    [EXPRESSION_FUNCTION_LITERAL]=(ExpressionCleanupFn_t)cleanupFunctionLiteral,
    [EXPRESSION_INVALID]=NULL
};

static ExpressionToStringFn_t expressionToStringFns[] = {
    [EXPRESSION_IDENTIFIER]=(ExpressionToStringFn_t)identifierToString,
    [EXPRESSION_INTEGER_LITERAL]=(ExpressionToStringFn_t)integerLiteralToString,
    [EXPRESSION_BOOLEAN]=(ExpressionToStringFn_t)booleanToString,
    [EXPRESSION_PREFIX_EXPRESSION]=(ExpressionToStringFn_t)prefixExpressionToString,
    [EXPRESSION_INFIX_EXPRESSION]=(ExpressionToStringFn_t)infixExpressionToString,
    [EXPRESSION_IF_EXPRESSION]=(ExpressionToStringFn_t)ifExpressionToString,
    [EXPRESSION_FUNCTION_LITERAL]=(ExpressionToStringFn_t)functionLiteralToString,
    [EXPRESSION_INVALID]=NULL
};


Expression_t* createExpression(ExpressionType_t type, void* value) { 
    Expression_t* exp = (Expression_t*)malloc(sizeof(Expression_t));
    if (exp == NULL)
        return NULL;
    exp->type = type;
    exp->value = value;
    return exp;
}

void cleanupExpression(Expression_t** expr) { 
    if (*expr == NULL)
        return;
    
    if ((*expr)->type  >= 0  && (*expr)->type < EXPRESSION_INVALID) {
        ExpressionCleanupFn_t cleanupFn = expressionCleanupFns[(*expr)->type];
        cleanupFn(&(*expr)->value);
    }
    
    (*expr)->type = EXPRESSION_INVALID;
    free(*expr);
    *expr=NULL;
}

char* expressionToString(Expression_t* expr) {

    if (expr->type  >= 0 && expr->type < EXPRESSION_INVALID) {
        ExpressionToStringFn_t toStringFn = expressionToStringFns[expr->type];
        return toStringFn(expr->value);
    }
    return cloneString("");
}

const char* expressionTokenLiteral(Expression_t* expr) {
    // Each structure starts with a Token_t element 
    typedef struct ExpressionBase {
        Token_t* token;
    } ExpressionBase_t;
    
    if (expr->type  >= 0 && expr->type < EXPRESSION_INVALID) {
        ExpressionBase_t* base = (ExpressionBase_t*)expr->value;

        return base->token->literal; 
    }
    return "";
}


/************************************ 
 *           IDENTIFIER             *
 ************************************/

Identifier_t* createIdentifier(const Token_t* tok, const char* val) {
    Identifier_t* ident = (Identifier_t*)malloc(sizeof(Identifier_t));
    if (ident == NULL)
        return NULL; 
    ident->token = cloneToken(tok);
    ident->value = cloneString(val);
    return ident;
}

void cleanupIdentifier(Identifier_t** ident) {
    if (*ident == NULL)
        return;

    cleanupToken(&(*ident)->token);
    free((void*)(*ident)->value);
    
    free(*ident);
    *ident = NULL;
}

char* identifierToString(Identifier_t* ident) {
    // Return a copy to avoid free errors
    return cloneString(ident->value);
}


/************************************ 
 *      INTEGER LITERAL             *
 ************************************/

IntegerLiteral_t* createIntegerLiteral(const Token_t* tok) {
    IntegerLiteral_t* il = (IntegerLiteral_t*)malloc(sizeof(IntegerLiteral_t));
    if (il == NULL)
        return NULL;
    il->token = cloneToken(tok);
    il->value = 0;
    return il;
}

void cleanupIntegerLiteral(IntegerLiteral_t** il) {
    if (*il == NULL)
        return;
    
    cleanupToken(&(*il)->token);
    
    free(*il);
    *il = NULL;
}

char* integerLiteralToString(IntegerLiteral_t* il) {
    return cloneString(il->token->literal);
}


/************************************ 
 *          BOOLEAN                 *
 ************************************/

Boolean_t* createBoolean(const Token_t* tok) {
    Boolean_t* bl = (Boolean_t*)malloc(sizeof(Boolean_t));
    if (bl == NULL)
        return NULL;
    bl->token = cloneToken(tok);
    bl->value = false;
    return bl;
}

void cleanupBoolean(Boolean_t** bl) {
    if (*bl == NULL)
        return;
    
    cleanupToken(&(*bl)->token);

    free(*bl);
    *bl = NULL;
}

char* booleanToString(Boolean_t* bl) {
    return bl->value ? cloneString("true") : cloneString("false");
}



/************************************ 
 *      PREFIX EXPRESSION           *
 ************************************/

PrefixExpression_t* createPrefixExpresion(const Token_t* tok) {
    PrefixExpression_t* exp = (PrefixExpression_t*)malloc(sizeof(PrefixExpression_t));
    if (exp == NULL)
        return NULL;
    exp->token = cloneToken(tok);
    exp->operator = NULL;
    exp->right = NULL;
    return exp;
}

void cleanupPrefixExpression(PrefixExpression_t** exp) {
    if (*exp == NULL)
        return;

    cleanupToken(&(*exp)->token);
    free((*exp)->operator);
    cleanupExpression(&(*exp)->right);
    
    free(*exp);
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

InfixExpression_t* createInfixExpresion(const Token_t* tok) {
    InfixExpression_t* exp = (InfixExpression_t*)malloc(sizeof(InfixExpression_t));
    if (exp == NULL)
        return NULL;
    exp->token = cloneToken(tok);
    exp->left = NULL;
    exp->operator = NULL;
    exp->right = NULL;
    return exp;
}

void cleanupInfixExpression(InfixExpression_t** exp) {
    if (*exp == NULL)
        return;
    
    cleanupToken(&(*exp)->token);
    cleanupExpression(&(*exp)->left);
    free((*exp)->operator);
    cleanupExpression(&(*exp)->right);

    free(*exp);
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

IfExpression_t* createIfExpresion(const Token_t* tok) {
    IfExpression_t* exp = (IfExpression_t*) malloc(sizeof(IfExpression_t));
    if (exp == NULL)
        return NULL;
    exp->token = cloneToken(tok);
    exp->condition = NULL;
    exp->consequence = NULL;
    exp->alternative = NULL;
    return exp;
}

void cleanupIfExpression(IfExpression_t** exp) {
    if (*exp == NULL)
        return;

    cleanupToken(&(*exp)->token);
    cleanupExpression(&(*exp)->condition);
    cleanupBlockStatement(&(*exp)->consequence);
    cleanupBlockStatement(&(*exp)->alternative);

    free(*exp);
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

FunctionLiteral_t* createFunctionLiteral(const Token_t* tok) {
    FunctionLiteral_t *exp = (FunctionLiteral_t*)malloc(sizeof(FunctionLiteral_t));
    if (exp == NULL) 
        return exp;
    
    exp->token = cloneToken(tok);
    exp->parameters = createVector(sizeof(Identifier_t*));
    exp->body = NULL;

    return exp;
}

void cleanupFunctionLiteral(FunctionLiteral_t** exp) {
    if (*exp == NULL)
        return;

    cleanupToken(&(*exp)->token);
    cleanupVectorContents((*exp)->parameters, (VectorElementCleanupFn_t)cleanupIdentifier);
    cleanupVector(&(*exp)->parameters);
    cleanupBlockStatement(&(*exp)->body);

    free(*exp);
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
 *         GENERIC STATEMENT        *
 ************************************/

static StatementCleanupFn_t statementCleanupFns[] = {
    [STATEMENT_LET]=(StatementCleanupFn_t)cleanupLetStatement,
    [STATEMENT_RETURN]=(StatementCleanupFn_t)cleanupReturnStatement,
    [STATEMENT_EXPRESSION]=(StatementCleanupFn_t)cleanupExpressionStatement,
    [STATEMENT_BLOCK]=(StatementCleanupFn_t)cleanupBlockStatement,
    [STATEMENT_INVALID]=NULL
};

static StatementToStringFn_t statementToStringFns[] = {
    [STATEMENT_LET]=(StatementToStringFn_t)letStatementToString,
    [STATEMENT_RETURN]=(StatementToStringFn_t)returnStatementToString,
    [STATEMENT_EXPRESSION]=(StatementToStringFn_t)expressionStatementToString,
    [STATEMENT_BLOCK]=(StatementToStringFn_t)blockStatementToString,
    [STATEMENT_INVALID]=NULL
};

Statement_t* createStatement(StatementType_t type, void* value) {
    Statement_t* st = (Statement_t*)malloc(sizeof(Statement_t));
    if( st == NULL)
        return NULL;
    st->type = type;
    st->value = value;
    return st;
}

void cleanupStatement(Statement_t** st) {
    if (*st == NULL)
        return;

    if ((*st)->type >= 0 &&  (*st)->type < STATEMENT_INVALID) {
        StatementCleanupFn_t cleanupFn = statementCleanupFns[(*st)->type];
        cleanupFn(&((*st)->value));
    }

    (*st)->type = STATEMENT_INVALID;
    free(*st);
    *st = NULL;
}


const char* statementTokenLiteral(Statement_t* st) {
    // Each structure starts with a Token_t element 
    typedef struct StatementBase {
        Token_t* token;
    } StatementBase_t;
    
    if (st->type >= 0 && st->type <STATEMENT_INVALID ) {
        return ((StatementBase_t*)st->value)->token->literal;
    }
    return "";
}

char* statementToString(Statement_t* st) {
    if ( (st->type >= 0) && (st->type <=STATEMENT_INVALID) ) {
        StatementToStringFn_t toStringFn = statementToStringFns[st->type];
        return toStringFn(st->value);
    }

    return cloneString("");
}


/************************************ 
 *         LET STATEMENT            *
 ************************************/

LetStatement_t* createLetStatement(const Token_t* token) {
    LetStatement_t* st = (LetStatement_t*)malloc(sizeof(LetStatement_t));
    if (st == NULL)
        return NULL;
    st->token = cloneToken(token);
    st->name = NULL;
    st->value = NULL;
    return st;
}


void cleanupLetStatement(LetStatement_t** st) {
    if (*st == NULL)
        return;

    cleanupToken(&(*st)->token);
    cleanupIdentifier(&(*st)->name);
    cleanupExpression(&(*st)->value);
    
    free(*st);
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

ReturnStatement_t* createReturnStatement(const Token_t* token) {
    ReturnStatement_t* st = (ReturnStatement_t*)malloc(sizeof(ReturnStatement_t));
    if (st == NULL)
        return NULL;
    st->token = cloneToken(token);
    st->returnValue = NULL;
    return st;
}

void cleanupReturnStatement(ReturnStatement_t** st) {
    if (*st == NULL)
        return;
    
    cleanupToken(&(*st)->token);
    cleanupExpression(&(*st)->returnValue);
    
    free(*st);
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

ExpressionStatement_t* createExpressionStatement(const Token_t* token) {
    ExpressionStatement_t* st = (ExpressionStatement_t*) malloc(sizeof(ExpressionStatement_t));
    if (st == NULL)
        return NULL;
    st->token = cloneToken(token);
    st->expression = NULL;
    return st;
}

void cleanupExpressionStatement(ExpressionStatement_t** st) {
    if (*st == NULL)
        return;

    cleanupToken(&(*st)->token);
    cleanupExpression(&(*st)->expression);

    free(*st);
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

BlockStatement_t* createBlockStatement(const Token_t* token) {
    BlockStatement_t* st = (BlockStatement_t*) malloc(sizeof(BlockStatement_t));
    if (st == NULL) 
        return NULL;
    st->token = cloneToken(token);
    st->statements = createVector(sizeof(Statement_t*));
    return st;
}

void cleanupBlockStatement(BlockStatement_t** st) {
    if (*st == NULL)
        return;
    
    cleanupToken(&(*st)->token);
    cleanupStatementVec(&(*st)->statements);
    
    free(*st);
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
    cleanupVectorContents(*statements, (VectorElementCleanupFn_t)cleanupStatement);
    cleanupVector(statements);
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