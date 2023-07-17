#include <malloc.h> 
#include <string.h> 
#include "token.h"


Token_t* createToken(TokenType_t type, const char* literal, uint16_t len) {
    Token_t* token = (Token_t*) malloc(sizeof(Token_t));
    if (token == NULL)
        return NULL;

    token->type = type;
    token->literal = literal;
    token->len = len;

    return token;
}

void cleanupToken(Token_t** token)
{
    if (*token == NULL)
        return;
    free(*token);
    *token = NULL;
}

TokenType_t lookupIdent(const char* ident, uint32_t len)
{
    if(strncmp(ident, "let", len) == 0)
        return TOKEN_LET;
    else if (strncmp(ident, "fn", len) == 0) 
        return TOKEN_FUNCTION;
    else if (strncmp(ident, "true", len) == 0) 
        return TOKEN_TRUE;
    else if (strncmp(ident, "false", len) == 0) 
        return TOKEN_FALSE;
    else if (strncmp(ident, "if", len) == 0) 
        return TOKEN_IF;
    else if (strncmp(ident, "else", len) == 0) 
        return TOKEN_ELSE;
    else if (strncmp(ident, "return", len) == 0) 
        return TOKEN_RETURN;
    return TOKEN_IDENT;
}

static const char* TokenTypeStrings[32] = {
    "TOKEN_ILLEGAL", "TOKEN_EOF",
    "TOKEN_IDENT","TOKEN_INT",
    "TOKEN_ASSIGN", "TOKEN_PLUS", 
    "TOKEN_MINUS", "TOKEN_BANG",
    "TOKEN_ASTERISK", "TOKEN_SLASH",    
    "TOKEN_LT", "TOKEN_GT",
    "TOKEN_EQ", "TOKEN_NOT_EQ",
    "TOKEN_COMMA", "TOKEN_SEMICOLON", 
    "TOKEN_LPAREN", "TOKEN_RPAREN", 
    "TOKEN_LBRACE", "TOKEN_RBRACE", 
    "TOKEN_FUNCTION", "TOKEN_LET",
    "TOKEN_TRUE", "TOKEN_FALSE",
    "TOKEN_IF",  "TOKEN_ELSE",
    "TOKEN_RETURN"
};

const char * tokenTypeToStr(TokenType_t tokType)
{
    if (tokType < 0 || tokType >= _TOKEN_LAST) {
        return "INVLAID_TOKEN_TYPE";
    }
    return TokenTypeStrings[tokType];
}

const char * tokenCopyLiteral(Token_t* token) 
{
    char* literal = (char*) malloc(token->len + 1);
    memmove(literal, token->literal, token->len);
    literal[token->len] = '\0';
    return literal;
}