#ifndef _TOKEN_H_
#define _TOKEN_H_

#include <stdio.h>

typedef enum {
    TOK_ILLEGAL, 
    TOK_EOF, 

    // Identifieres + literals 
    TOK_IDENT, // add, foobar, x, y, ..
    TOK_INT,  // 1343456

    // Operators
    TOK_ASSIGN,
    TOK_PLUS, 

    // Delimiters
    TOK_COMMA, 
    TOK_SEMICOLON, 

    TOK_LPAREN, 
    TOK_RPAREN, 
    TOK_LBRACE, 
    TOK_RBRACE, 

    // Keywords, 
    TOK_FUNCTION, 
    TOK_LET

} TokenType; 

struct Token {
    TokenType type;
    const char* literal;
};




#endif