#include <malloc.h>
#include <stdbool.h>
#include "parser.h"

/* Operator precedence */
typedef enum PrecValue{
    PREC_LOWEST = 1,
    PREC_EQUALS, 
    PREC_LESSGREATER,
    PREC_SUM, 
    PREC_PRODUCT, 
    PREC_PREFIX, 
    PREC_CALL
} PrecValue_t;

/* Parsing functions */

static Statement_t* parserParseStatement(Parser_t* parser);
static Statement_t* parserParseLetStatement(Parser_t* parser);
static Statement_t* parserParseReturnStatement(Parser_t* parser);
static Statement_t* parserParseExpressionStatement(Parser_t* parser);

static Expression_t* parserParseExpression(Parser_t* parser, PrecValue_t precedence);
static Expression_t* parserParseIdentifier(Parser_t* parser);

static void parserRegisterPrefix(Parser_t* parser, TokenType_t tokType, PrefixParseFn_t fn);
static void parserRegisterInfix(Parser_t* parser, TokenType_t tokType, InfixParseFn_t fn);


/* Token manipulation */

static void parserNextToken(Parser_t* parser);
static void parserConsumeToken(Parser_t* parser);
static bool parserCurTokenIs(Parser_t* parser, TokenType_t tokType);
static bool parserPeekTokenIs(Parser_t* parser, TokenType_t tokType);
static bool parserExpectPeek(Parser_t* parser, TokenType_t tokType);

/* ERROR handling */

static void parserAppendError(Parser_t* parser, const char*);
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

    memset(parser->infixParserFns, 0, sizeof(InfixParseFn_t) * _TOKEN_TYPE_CNT);

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
        cleanupToken(&parser->curToken);
        parserNextToken(parser);
    }

    // Get rid of semicolon
    parserConsumeToken(parser);

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
        cleanupToken(&parser->curToken);
        parserNextToken(parser);
    }

    // Get rid of semicolon  
    parserConsumeToken(parser);

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
    if( prefix == NULL)
        return NULL;
    Expression_t* leftExp = prefix(parser);

    return leftExp;
}

static Expression_t* parserParseIdentifier(Parser_t* parser) {
    return createExpression(EXPRESSION_IDENTIFIER, 
                            createIdentifier(parser->curToken, parser->curToken->literal));
}

static void parserRegisterPrefix(Parser_t* parser, TokenType_t tokType, PrefixParseFn_t fn) {
    parser->prefixParseFns[tokType] = fn;
}
static void parserRegisterInfix(Parser_t* parser, TokenType_t tokType, InfixParseFn_t fn) {
    parser->infixParserFns[tokType] = fn;
}


/* Token manipulation functions */

static void parserNextToken(Parser_t* parser) {
    parser->curToken = parser->peekToken;
    parser->peekToken = lexerNextToken(parser->lexer);
}

static void parserConsumeToken(Parser_t* parser) {
    cleanupToken(&parser->curToken);
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

static void parserAppendError(Parser_t* parser, const char* err) {
    vectorAppend(parser->errors, (void*)&err);
}

static void parserPeekError(Parser_t* parser, TokenType_t expTokenType) {
    #define PEEK_ERROR_FMT_STR "Expected next token to be %s, got %s instead"
    size_t strSize = snprintf(NULL, 0, PEEK_ERROR_FMT_STR, 
                              tokenTypeToStr(expTokenType), 
                              tokenTypeToStr(parser->peekToken->type)) + 1;
    
    char *strBuffer = malloc(strSize);

    sprintf(strBuffer, PEEK_ERROR_FMT_STR, 
            tokenTypeToStr(expTokenType), 
            tokenTypeToStr(parser->peekToken->type));
    
    #undef PEEK_ERROR_FMT_STR

    parserAppendError(parser, strBuffer);
}
