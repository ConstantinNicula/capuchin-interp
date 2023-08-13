#include <stdio.h> 
#include <stdlib.h>
#include <stdbool.h>
#include <string.h> 

#include "../parser.h"
#include "../lexer.h"
#include "../utils.h"
#include "../evaluator.h"

#define PROMPT ">> "

static void printParserErrors(const char**err, uint32_t cnt){
    printf("Woops! We ran into some monkey business here!\n");
    printf(" parser errors:\n");

    for (uint32_t i = 0; i < cnt ; i++) {
        printf ("\t%s\n", err[i]);
    }
}

int main() {
    char inputBuffer[256] = "";

    while (true) {
        printf("%s", PROMPT);
        gets(inputBuffer);
    
        Lexer_t* lexer = createLexer(inputBuffer);
        Parser_t* parser = createParser(lexer);

        Program_t* program = parserParseProgram(parser);
        if (parserGetErrorCount(parser) != 0) {
            printParserErrors(parserGetErrors(parser), parserGetErrorCount(parser));
        } else {
            Object_t* evalRes = evalProgram(program);
            
            if (evalRes != NULL) {
                printf("%s\n", objectInspect(evalRes));
            } else {
                printf("Eval failed \n");
            }

            cleanupObject(&evalRes);
        }

        cleanupParser(&parser);
        cleanupProgram(&program);
    }


    return 0;
}