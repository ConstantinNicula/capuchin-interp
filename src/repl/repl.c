#include <stdio.h> 
#include <stdlib.h>
#include <stdbool.h>
#include <string.h> 

#include "lexer.h"

#define PROMPT ">> "

int main() {
    char inputBuffer[256] = "";
    char tokBuffer[64] = "";
    Lexer_t* lexer = NULL;
    Token_t* tok = NULL;

    while (true) {
        printf("%s", PROMPT);
        gets(inputBuffer);

        lexer = createLexer(inputBuffer);
        tok = lexerNextToken(lexer);
        while(tok != NULL && tok->type != TOKEN_EOF)  {
            strncpy(tokBuffer, tok->literal, tok->len);
            tokBuffer[tok->len] = '\0';

            printf("{%s : '%s'}\n", tokenToStr(tok->type), tokBuffer);
            
            cleanupToken(&tok);
            tok = lexerNextToken(lexer);
        }
        cleanupLexer(&lexer);
    }


    return 0;
}