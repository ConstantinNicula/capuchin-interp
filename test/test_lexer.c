#include "unity.h"
#include "lexer.h"
#include "token.h"



void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

void lexer_simple_test(void) {
    char input[] = "=+(){},;";

    Lexer_t* lexer = createLexer(input);

    static Token_t expToken[] = {
        {TOKEN_ASSIGN, "="},
        {TOKEN_PLUS, "+"},
        {TOKEN_LPAREN, "("},
        {TOKEN_RPAREN, ")"},
        {TOKEN_LBRACE, "{"},
        {TOKEN_RBRACE, "}"},
        {TOKEN_COMMA, ","},
        {TOKEN_SEMICOLON, ";"},
        {TOKEN_EOF, ""},
    };

    int32_t numTests = sizeof(expToken) / sizeof(Token_t);
    Token_t* tok;

    char msg[30];
    for (int32_t i = 0; i < numTests; i++) {
        sprintf(msg, "Test_%d", i);

        tok = lexerNextToken(lexer);
        TEST_ASSERT_NOT_NULL_MESSAGE(tok, msg);

        TEST_ASSERT_EQUAL_INT_MESSAGE(expToken[i].type, tok->type , msg); 
        TEST_ASSERT_EQUAL_STRING_MESSAGE(expToken[i].literal, tok->literal, msg);
    }

}


// not needed when using generate_test_runner.rb
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(lexer_simple_test);
    return UNITY_END();
}