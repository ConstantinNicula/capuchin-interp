#ifndef _TOKEN_H_
#define _TOKEN_H_

#include <stdio.h>
#include <stdint.h> 

typedef enum TokenType{
    TOKEN_ILLEGAL, 
    TOKEN_EOF, 

    // Identifieres + literals 
    TOKEN_IDENT, // add, foobar, x, y, ..
    TOKEN_INT,  // 1343456

    // Operators
    TOKEN_ASSIGN,
    TOKEN_PLUS, 

    // Delimiters
    TOKEN_COMMA, 
    TOKEN_SEMICOLON, 

    TOKEN_LPAREN, 
    TOKEN_RPAREN, 
    TOKEN_LBRACE, 
    TOKEN_RBRACE, 

    // Keywords, 
    TOKEN_FUNCTION, 
    TOKEN_LET

} TokenType_t; 

typedef struct Token {
    TokenType_t type;
    const char* literal; // pointer to start of char literal in input string
    uint16_t len; // length of literal (can be 0, 1, ...)
} Token_t;


TokenType_t lookupIdent(const char* ident, uint32_t len);
Token_t* createToken(TokenType_t type, const char* literal, uint16_t len);
void cleanupToken(Token_t** token);



#endif