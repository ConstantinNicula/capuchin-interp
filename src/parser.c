#include <malloc.h>
#include <stdbool.h>
#include <string.h> 

#include "parser.h"
#include "utils.h"

/* Operator precedence levels */
typedef enum PrecValue{
    PREC_LOWEST = 0,
    PREC_EQUALS, 
    PREC_LESSGREATER,
    PREC_SUM, 
    PREC_PRODUCT, 
    PREC_PREFIX, 
    PREC_CALL
} PrecValue_t;

/* Precedences for operators, values which are not define default to 0-PREC_LOWEST*/
static PrecValue_t _precedences[] = {
    [TOKEN_EQ]=PREC_EQUALS,
    [TOKEN_NOT_EQ]=PREC_EQUALS,
    [TOKEN_LT]=PREC_LESSGREATER,
    [TOKEN_GT]=PREC_LESSGREATER,
    [TOKEN_PLUS]=PREC_SUM,
    [TOKEN_MINUS]=PREC_SUM,
    [TOKEN_SLASH]=PREC_PRODUCT,
    [TOKEN_ASTERISK]=PREC_PRODUCT
};

/* Parsing functions */

static Statement_t* parserParseStatement(Parser_t* parser);
static Statement_t* parserParseLetStatement(Parser_t* parser);
static Statement_t* parserParseReturnStatement(Parser_t* parser);
static Statement_t* parserParseExpressionStatement(Parser_t* parser);

static Expression_t* parserParseExpression(Parser_t* parser, PrecValue_t precedence);
static Expression_t* parserParseIdentifier(Parser_t* parser);
static Expression_t* parserParseIntegerLiteral(Parser_t* parser);
static Expression_t* parserParsePrefixExpression(Parser_t* parser);
static Expression_t* parserParseInfixExpression(Parser_t* parser, Expression_t* left);
static Expression_t* parserParseBoolean(Parser_t* parser);
static Expression_t* parserParseGroupedExpression(Parser_t* parser);

static PrecValue_t parserPeekPrecedence(Parser_t* parser);
static PrecValue_t parserCurPrecedence(Parser_t* parser);

static void parserRegisterPrefix(Parser_t* parser, TokenType_t tokType, PrefixParseFn_t fn);
static void parserRegisterInfix(Parser_t* parser, TokenType_t tokType, InfixParseFn_t fn);


/* Token manipulation */

static void parserNextToken(Parser_t* parser);
static bool parserCurTokenIs(Parser_t* parser, TokenType_t tokType);
static bool parserPeekTokenIs(Parser_t* parser, TokenType_t tokType);
static bool parserExpectPeek(Parser_t* parser, TokenType_t tokType);

/* ERROR handling */

static void parserAppendError(Parser_t* parser, const char*);
static void parserNoPrefixParseFnError(Parser_t* parser, TokenType_t tok);
static void parserPeekError(Parser_t* parser, TokenType_t expTokenType);


/* Allocation & Cleanup functions */

Parser_t* createParser(Lexer_t* lexer) {
    Parser_t* parser = (Parser_t*) malloc(sizeof(Parser_t));
    if (parser == NULL)
        return NULL;

    parser->lexer = lexer;
    parser->curToken = parser->peekToken = NULL;

    memset(parser->prefixParseFns, 0, sizeof(PrefixParseFn_t) * _TOKEN_TYPE_CNT);
    parserRegisterPrefix(parser, TOKEN_IDENT, parserParseIdentifier);
    parserRegisterPrefix(parser, TOKEN_INT, parserParseIntegerLiteral);
    parserRegisterPrefix(parser, TOKEN_BANG, parserParsePrefixExpression);
    parserRegisterPrefix(parser, TOKEN_MINUS, parserParsePrefixExpression);
    parserRegisterPrefix(parser, TOKEN_TRUE, parserParseBoolean);
    parserRegisterPrefix(parser, TOKEN_FALSE, parserParseBoolean);
    parserRegisterPrefix(parser, TOKEN_LPAREN, parserParseGroupedExpression);

    memset(parser->infixParserFns, 0, sizeof(InfixParseFn_t) * _TOKEN_TYPE_CNT);
    parserRegisterInfix(parser, TOKEN_PLUS, parserParseInfixExpression);
    parserRegisterInfix(parser, TOKEN_MINUS, parserParseInfixExpression);
    parserRegisterInfix(parser, TOKEN_SLASH, parserParseInfixExpression);
    parserRegisterInfix(parser, TOKEN_ASTERISK, parserParseInfixExpression);
    parserRegisterInfix(parser, TOKEN_EQ, parserParseInfixExpression);
    parserRegisterInfix(parser, TOKEN_NOT_EQ, parserParseInfixExpression);
    parserRegisterInfix(parser, TOKEN_LT, parserParseInfixExpression);
    parserRegisterInfix(parser, TOKEN_GT, parserParseInfixExpression);

    parser->errors = createVector(sizeof(char*));

    parserNextToken(parser);
    parserNextToken(parser);
    return parser;
}

void cleanupParser(Parser_t** parser) {
    if (*parser == NULL) 
        return;
    cleanupLexer(&(*parser)->lexer);
    cleanupToken(&(*parser)->peekToken);
    cleanupToken(&(*parser)->curToken);

    const char** errorStr = parserGetErrors(*parser);
    uint32_t errorCnt = parserGetErrorCount(*parser);

    for (uint32_t i = 0u; i < errorCnt; i++){
        free((void*)errorStr[i]);
    }
    cleanupVector(&(*parser)->errors);
    
    free(*parser);
    *parser = NULL;
}

/* Core parsing logic */

Program_t* parserParseProgram(Parser_t* parser) {
    Program_t* program = createProgram();

    while (!parserCurTokenIs(parser, TOKEN_EOF)) {
        Statement_t* stmt = parserParseStatement(parser);
        
        if (stmt != NULL) {
            programAppendStatement(program, stmt);
        } 
    
        parserNextToken(parser);
    }

    return program;
}

static Statement_t* parserParseStatement(Parser_t* parser) {
    switch (parser->curToken->type) {
        case TOKEN_LET: 
            return parserParseLetStatement(parser);
        case TOKEN_RETURN: 
            return parserParseReturnStatement(parser);
        default:
            return parserParseExpressionStatement(parser);
    }
}

static Statement_t* parserParseLetStatement(Parser_t* parser) {
    LetStatement_t* stmt = createLetStatement(parser->curToken);

    if (!parserExpectPeek(parser, TOKEN_IDENT)) {
        goto cleanup;
    }

    stmt->name = createIdentifier(parser->curToken, parser->curToken->literal);

    if (!parserExpectPeek(parser, TOKEN_ASSIGN)) {
        goto cleanup;
    }

    // TODO: We're skipping the expression until we 
    // encounter a semicolon
    while (!parserCurTokenIs(parser, TOKEN_SEMICOLON)) {
        parserNextToken(parser);
    }

    return createStatement(STATEMENT_LET, stmt);

cleanup:
    cleanupLetStatement(&stmt);
    return NULL;
}


static Statement_t* parserParseReturnStatement(Parser_t* parser) {
    ReturnStatement_t* stmt = createReturnStatement(parser->curToken);
    parserNextToken(parser);

    // TODO: We're skipping the expressions until we 
    // encounter a semicolon

    while(!parserCurTokenIs(parser, TOKEN_SEMICOLON)) {
        parserNextToken(parser);
    }

    return createStatement(STATEMENT_RETURN, stmt);
}


static Statement_t* parserParseExpressionStatement(Parser_t* parser) {
    ExpressionStatement_t* stmt = createExpressionStatement(parser->curToken);

    stmt->expression = parserParseExpression(parser, PREC_LOWEST);

    // optional semicolon at the end of sequence
    if (parserPeekTokenIs(parser, TOKEN_SEMICOLON)) {
        parserNextToken(parser);
    }

    return createStatement(STATEMENT_EXPRESSION, stmt);
}

static Expression_t* parserParseExpression(Parser_t* parser, PrecValue_t precedence) { 
    PrefixParseFn_t prefix = parser->prefixParseFns[parser->curToken->type];
    
    if( prefix == NULL) {
        parserNoPrefixParseFnError(parser, parser->curToken->type);
        return NULL;
    }

    Expression_t* leftExp = prefix(parser);

    while (!parserPeekTokenIs(parser, TOKEN_SEMICOLON) && precedence < parserPeekPrecedence(parser)) {
        InfixParseFn_t infix = parser->infixParserFns[parser->peekToken->type];
        if (infix == NULL) {
            return leftExp;
        }

        parserNextToken(parser);

        leftExp = infix(parser, leftExp);
    }

    return leftExp;
}

static Expression_t* parserParseIdentifier(Parser_t* parser) {
    return createExpression(EXPRESSION_IDENTIFIER, 
                            createIdentifier(parser->curToken, parser->curToken->literal));
}

static Expression_t* parserParseIntegerLiteral(Parser_t* parser) {
    IntegerLiteral_t* lit = createIntegerLiteral(parser->curToken);

    if (!strToInteger(lit->token->literal, &lit->value)) {
        char* err = strFormat("Could not parser %s as integer", lit->token->literal);
        parserAppendError(parser, err);
        cleanupIntegerLiteral(&lit);
        return NULL;
    }

    return createExpression(EXPRESSION_INTEGER_LITERAL, lit);
}

static Expression_t* parserParsePrefixExpression(Parser_t* parser) {
    PrefixExpression_t* expression = createPrefixExpresion(parser->curToken);
    expression->operator = cloneString(parser->curToken->literal);

    parserNextToken(parser);
    expression->right = parserParseExpression(parser, PREC_PREFIX);

    return createExpression(EXPRESSION_PREFIX_EXPRESSION, expression);
}

static Expression_t* parserParseInfixExpression(Parser_t* parser, Expression_t* left) {
    InfixExpression_t* expression = createInfixExpresion(parser->curToken);
    expression->left = left;
    expression->operator = cloneString(parser->curToken->literal);
    
    PrecValue_t precedence = parserCurPrecedence(parser);
    parserNextToken(parser);
    expression->right = parserParseExpression(parser, precedence);

    return createExpression(EXPRESSION_INFIX_EXPRESSION, expression);
}

static Expression_t* parserParseBoolean(Parser_t* parser) {
    Boolean_t* bl = createBoolean(parser->curToken);
    bl->value = parserCurTokenIs(parser, TOKEN_TRUE);
    return createExpression(EXPRESSION_BOOLEAN, bl);
}

static Expression_t* parserParseGroupedExpression(Parser_t* parser) {
    parserNextToken(parser);
    
    Expression_t* exp = parserParseExpression(parser, PREC_LOWEST);

    if (!parserExpectPeek(parser, TOKEN_RPAREN)) {
        cleanupExpression(&exp);
        return NULL;
    }

    return exp;
}


static PrecValue_t parserPeekPrecedence(Parser_t* parser) {
    return _precedences[parser->peekToken->type];
}

static PrecValue_t parserCurPrecedence(Parser_t* parser) {
    return _precedences[parser->curToken->type];
}


static void parserRegisterPrefix(Parser_t* parser, TokenType_t tokType, PrefixParseFn_t fn) {
    parser->prefixParseFns[tokType] = fn;
}
static void parserRegisterInfix(Parser_t* parser, TokenType_t tokType, InfixParseFn_t fn) {
    parser->infixParserFns[tokType] = fn;
}


/* Token manipulation functions */

static void parserNextToken(Parser_t* parser) {
    if (parser->curToken)
        cleanupToken(&parser->curToken);
    parser->curToken = parser->peekToken;
    parser->peekToken = lexerNextToken(parser->lexer);
}

static bool parserCurTokenIs(Parser_t* parser, TokenType_t tokType) {
    return parser->curToken->type == tokType;
}

static bool parserPeekTokenIs(Parser_t* parser, TokenType_t tokType) {
    return parser->peekToken->type == tokType;
}

static bool parserExpectPeek(Parser_t* parser, TokenType_t expTokType) {
    if( parser->peekToken->type == expTokType) {
        parserNextToken(parser);
        return true;
    } else {
        parserPeekError(parser, expTokType);
        return false;
    }
    
}


/* ERROR handling functions */

const char** parserGetErrors(Parser_t* parser) {
    return (const char**) vectorGetBuffer(parser->errors);
}

uint32_t parserGetErrorCount(Parser_t* parser) {
    return vectorGetCount(parser->errors);
}


static void parserNoPrefixParseFnError(Parser_t* parser, TokenType_t tokType) {
    char* msg = strFormat("No prefix parser function for %s found", tokenTypeToStr(tokType));
    parserAppendError(parser, msg);
}

static void parserAppendError(Parser_t* parser, const char* err) {
    vectorAppend(parser->errors, (void*)&err);
}

static void parserPeekError(Parser_t* parser, TokenType_t expTokenType) {
    char* strBuffer = strFormat("Expected next token to be %s, got %s instead", 
                                tokenTypeToStr(expTokenType),   
                                tokenTypeToStr(parser->peekToken->type));
    parserAppendError(parser, strBuffer);
}
