#include <malloc.h>
#include <string.h> 
#include <stdio.h> 

#include "lexer.h"
#include "token.h"

void lexerReadChar(Lexer_t* lexer);


Lexer_t* createLexer(const char* input)
{
    if (input == NULL)
        return NULL;

    Lexer_t* lexer = (Lexer_t*) malloc(sizeof(Lexer_t));
    if (lexer == NULL)
        return NULL;
    
    lexer->input = input;
    lexer->position = 0;
    lexer->readPosition = 0;
    lexer->inputLength = strlen(lexer->input);

    lexerReadChar(lexer);
    return lexer;
}

void lexerReadChar(Lexer_t* lexer) {
    if (lexer->readPosition >= lexer->inputLength)
        lexer->ch = '\0'; // Empty string return NULL character 
    else 
        lexer->ch = lexer->input[lexer->readPosition];

    lexer->position = lexer->readPosition;
    lexer->readPosition++;
}

Token_t* lexerNextToken(Lexer_t* lexer) {
    Token_t *tok = NULL;
    switch(lexer->ch) {
        case '=': 
            tok = createToken(TOKEN_ASSIGN, "=");
            break;
        case ';':
            tok = createToken(TOKEN_SEMICOLON, ";");
            break;
        case '(':
            tok = createToken(TOKEN_LPAREN, "(");
            break;
        case ')':
            tok = createToken(TOKEN_RPAREN, ")");
            break;
        case ',': 
            tok = createToken(TOKEN_COMMA, ",");
            break;
        case '+':
            tok = createToken(TOKEN_PLUS, "+");
            break;
        case '{':
            tok = createToken(TOKEN_LBRACE, "{");
            break;
        case '}':
            tok = createToken(TOKEN_RBRACE, "}");
            break;
        case '\0':
            tok = createToken(TOKEN_EOF, "");
            break;  
    }

    lexerReadChar(lexer);
    return tok;
}