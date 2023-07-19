#ifndef _PARSER_H_
#define _PARSER_H_

#include "lexer.h"
#include "ast.h"

typedef struct Parser {
    Lexer_t* lexer;
    Token_t* curToken;
    Token_t* peekToken;

    Vector_t* errors;
} Parser_t;


Parser_t* createParser(Lexer_t* lexer);
void cleanupParser(Parser_t** parser);


Program_t* parserParseProgram(Parser_t* parser);
const char** parserGetErrors(Parser_t* parser);
uint32_t parserGetErrorCount(Parser_t* parser);

#endif 