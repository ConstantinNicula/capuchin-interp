#include <malloc.h>
#include <stdbool.h>
#include "parser.h"


static void parserNextToken(Parser_t* parser);
static Statement_t* parserParseStatement(Parser_t* parser);
static Statement_t* parserParseLetStatement(Parser_t* parser);

static bool parserCurTokenIs(Parser_t* parser, TokenType_t tokType);
static bool parserPeekTokenIs(Parser_t* parser, TokenType_t tokType);
static bool parserExpectPeek(Parser_t* parser, TokenType_t tokType);

Parser_t* createParser(Lexer_t* lexer) {
    Parser_t* parser = (Parser_t*) malloc(sizeof(Parser_t));
    if (parser == NULL)
        return NULL;

    parser->lexer = lexer;
    parser->curToken = parser->peekToken = NULL;

    parserNextToken(parser);
    parserNextToken(parser);
    return parser;
}

void cleanupParser(Parser_t** parser)
{
    if (*parser == NULL) 
        return;
    cleanupLexer(&(*parser)->lexer);
    cleanupToken(&(*parser)->peekToken);
    cleanupToken(&(*parser)->curToken);
    *parser = NULL;
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


static bool parserCurTokenIs(Parser_t* parser, TokenType_t tokType) {
    return parser->curToken->type == tokType;
}
static bool parserPeekTokenIs(Parser_t* parser, TokenType_t tokType) {
    return parser->peekToken->type == tokType;
}
static bool parserExpectPeek(Parser_t* parser, TokenType_t tokType) {
    if( parser->peekToken->type == tokType) {
        parserNextToken(parser);
        return true;
    }
    return false;
}