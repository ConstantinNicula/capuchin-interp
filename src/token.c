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