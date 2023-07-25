#include <stdio.h> 
#include <stdlib.h>
#include <stdbool.h>
#include <string.h> 

#include "lexer.h"
#include "utils.h"
#define PROMPT ">> "

int main() {
    char inputBuffer[256] = "";
    Lexer_t* lexer = NULL;
    Token_t* tok = NULL;

    while (true) {
        printf("%s", PROMPT);
        gets(inputBuffer);

        lexer = createLexer(inputBuffer);
        tok = lexerNextToken(lexer);
        while(tok != NULL && tok->type != TOKEN_EOF)  {
            char* tokString = cloneString(tok->literal);
            printf("{%s : '%s'}\n", tokenTypeToStr(tok->type), tokString);
            
            cleanupToken(&tok);
            tok = lexerNextToken(lexer);
        }
        cleanupLexer(&lexer);
    }


    return 0;
}