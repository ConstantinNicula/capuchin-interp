#include <malloc.h>
#include <stdbool.h>
#include "parser.h"


static void parserNextToken(Parser_t* parser);
static Statement_t* parserParseStatement(Parser_t* parser);
static Statement_t* parserParseLetStatement(Parser_t* parser);

static void parserAppendError(Parser_t* parser, const char*);

static bool parserCurTokenIs(Parser_t* parser, TokenType_t tokType);
static bool parserPeekTokenIs(Parser_t* parser, TokenType_t tokType);
static bool parserExpectPeek(Parser_t* parser, TokenType_t tokType);

static void parserPeekError(Parser_t* parser, TokenType_t expTokenType);


Parser_t* createParser(Lexer_t* lexer) {
    Parser_t* parser = (Parser_t*) malloc(sizeof(Parser_t));
    if (parser == NULL)
        return NULL;

    parser->lexer = lexer;
    parser->curToken = parser->peekToken = NULL;

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

const char** parserGetErrors(Parser_t* parser) {
    return (const char**) vectorGetBuffer(parser->errors);
}

uint32_t parserGetErrorCount(Parser_t* parser) {
    return vectorGetCount(parser->errors);
}

static void parserAppendError(Parser_t* parser, const char* err) {
    vectorAppend(parser->errors, (void*)&err);
}


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


static void parserNextToken(Parser_t* parser) {
    parser->curToken = parser->peekToken;
    parser->peekToken = lexerNextToken(parser->lexer);
}

static Statement_t* parserParseStatement(Parser_t* parser)
{
    switch (parser->curToken->type) {
        case TOKEN_LET: 
            return parserParseLetStatement(parser);
        default:
            return NULL;
    }
}

static Statement_t* parserParseLetStatement(Parser_t* parser) 
{
    LetStatement_t* stmt = createLetStatement(parser->curToken);

    if (!parserExpectPeek(parser, TOKEN_IDENT)) {
        goto cleanup;
    }

    stmt->name = createIdentifier(parser->curToken, tokenCopyLiteral(parser->curToken));

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