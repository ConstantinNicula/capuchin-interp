#include <malloc.h> 
#include "token.h"


Token_t* createToken(TokenType_t type, const char* literal) {
    Token_t* token = (Token_t*) malloc(sizeof(Token_t));
    if (token == NULL)
        return NULL;

    token->type = type;
    token->literal = literal;
    return token;
}

void cleanupToken(Token_t** token)
{
    if (*token == NULL)
        return;
    
    #warning "Memory cleanup not implemented yet"
    free(*token);

    *token = NULL;
}