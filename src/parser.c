#include <malloc.h>
#include "parser.h"

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

void parserNextToken(Parser_t* parser) {
    parser->curToken = parser->peekToken;
    parser->peekToken = lexerNextToken(parser->lexer);
}

Program_t* parserParseProgram(Parser_t* parser) {
    return NULL;
}