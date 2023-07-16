#include <malloc.h>
#include <string.h> 
#include <stdio.h> 
#include <stdint.h>
#include <stdbool.h> 


#include "lexer.h"
#include "token.h"

void lexerReadChar(Lexer_t* lexer);
void lexerReadIdentifier(Lexer_t* lexer, uint32_t* len);
void lexerReadDigit(Lexer_t* lexer, uint32_t* len);

void lexerSkipWhiteSpace(Lexer_t* lexer);

bool isLetter(char ch);
bool isWhiteSpace(char ch);
bool isDigit(char ch);

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


void lexerReadIdentifier(Lexer_t* lexer, uint32_t* len) {
    *len = 0u;
    while (isLetter(lexer->ch)) {
        lexerReadChar(lexer);
        (*len)++;
    }

}

void lexerReadDigit(Lexer_t* lexer, uint32_t* len) {
    *len = 0u;
    while(isDigit(lexer->ch)) {
        lexerReadChar(lexer);
        (*len)++;
    }
}

void lexerSkipWhiteSpace(Lexer_t* lexer) {
    while (isWhiteSpace(lexer->ch)) {
        lexerReadChar(lexer);
    }
}

Token_t* lexerNextToken(Lexer_t* lexer) {
    Token_t *tok = NULL;
    const char* tokLiteral = NULL;
    uint32_t tokLen = 0u;

    lexerSkipWhiteSpace(lexer);
    
    tokLiteral = &lexer->input[lexer->position]; 
    tokLen = 1u;

    switch(lexer->ch) {
        case '=': 
            tok = createToken(TOKEN_ASSIGN, tokLiteral, tokLen);
            break;
        case ';':
            tok = createToken(TOKEN_SEMICOLON, tokLiteral, tokLen);
            break;
        case '(':
            tok = createToken(TOKEN_LPAREN, tokLiteral, tokLen);
            break;
        case ')':
            tok = createToken(TOKEN_RPAREN, tokLiteral, tokLen);
            break;
        case ',': 
            tok = createToken(TOKEN_COMMA, tokLiteral, tokLen);
            break;
        case '+':
            tok = createToken(TOKEN_PLUS, tokLiteral, tokLen);
            break;
        case '{':
            tok = createToken(TOKEN_LBRACE, tokLiteral, tokLen);
            break;
        case '}':
            tok = createToken(TOKEN_RBRACE, tokLiteral, tokLen);
            break;
        case '\0':
            tok = createToken(TOKEN_EOF, tokLiteral, tokLen);
            break;  

        default: 
            if (isLetter(lexer->ch)) {
                lexerReadIdentifier(lexer, &tokLen);
                tok = createToken(lookupIdent(tokLiteral, tokLen), tokLiteral, tokLen);
                return tok;
            } else if(isDigit(lexer->ch)) {
                lexerReadDigit(lexer, &tokLen);
                tok = createToken(TOKEN_INT, tokLiteral, tokLen);
                return tok;
            } else { 
                tok = createToken(TOKEN_ILLEGAL, tokLiteral, tokLen);
            }

    }

    lexerReadChar(lexer);
    return tok;
}


bool isLetter(char ch) {
    return (('a' <= ch) && (ch <='z')) || (('A' <= ch) && (ch <= 'Z')) || (ch == '_');
}

bool isWhiteSpace(char ch) {
    return (ch == ' ') || (ch == '\t') || (ch == '\n') || (ch == '\r');
}

bool isDigit(char ch) {
    return ('0' <= ch) && (ch <= '9');
}