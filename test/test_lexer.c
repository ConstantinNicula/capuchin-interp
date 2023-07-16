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
        TEST_ASSERT_EQUAL_STRING_LEN_MESSAGE(expToken[i].literal, tok->literal, tok->len, msg);
    }

}


void lexer_test_monkey1(void) {

    char input[] = "let five = 5;\
                    let ten = 10;\
                    let add = fn(x, y) {\
                            x + y;\
                        };\
                    let result = add(five, ten);";

    Lexer_t* lexer = createLexer(input);

    static Token_t expToken[] = {
        {TOKEN_LET, "let"},
        {TOKEN_IDENT, "five"},
        {TOKEN_ASSIGN, "="},
        {TOKEN_INT, "5"},
        {TOKEN_SEMICOLON, ";"},
        {TOKEN_LET, "let"},
        {TOKEN_IDENT, "ten"},
        {TOKEN_ASSIGN, "="},
        {TOKEN_INT, "10"},
        {TOKEN_SEMICOLON, ";"},
        {TOKEN_LET, "let"},
        {TOKEN_IDENT, "add"},
        {TOKEN_ASSIGN, "="},
        {TOKEN_FUNCTION, "fn"},
        {TOKEN_LPAREN, "("},
        {TOKEN_IDENT, "x"},
        {TOKEN_COMMA, ","},
        {TOKEN_IDENT, "y"},
        {TOKEN_RPAREN, ")"},
        {TOKEN_LBRACE, "{"},
        {TOKEN_IDENT, "x"},
        {TOKEN_PLUS, "+"},
        {TOKEN_IDENT, "y"},
        {TOKEN_SEMICOLON, ";"},
        {TOKEN_RBRACE, "}"},
        {TOKEN_SEMICOLON, ";"},
        {TOKEN_LET, "let"},
        {TOKEN_IDENT, "result"},
        {TOKEN_ASSIGN, "="},
        {TOKEN_IDENT, "add"},
        {TOKEN_LPAREN, "("},
        {TOKEN_IDENT, "five"},
        {TOKEN_COMMA, ","},
        {TOKEN_IDENT, "ten"},
        {TOKEN_RPAREN, ")"},
        {TOKEN_SEMICOLON, ";"},
        {TOKEN_EOF, ""}
    };

    int32_t numTests = sizeof(expToken) / sizeof(Token_t);
    Token_t* tok;

    char msg[30];
    for (int32_t i = 0; i < numTests; i++) {
        sprintf(msg, "Test_%d", i+1);

        tok = lexerNextToken(lexer);
        TEST_ASSERT_NOT_NULL_MESSAGE(tok, msg);

        TEST_ASSERT_EQUAL_INT_MESSAGE(expToken[i].type, tok->type , msg); 
        TEST_ASSERT_EQUAL_STRING_LEN_MESSAGE(expToken[i].literal, tok->literal, tok->len, msg);
    }
}

void lexer_test_monkey2(void) {

    char input[] = "let five = 5;\
                    let ten = 10;\
                    \
                    let add = fn(x, y) {\
                        x + y;\
                    };\
                    \
                    let result = add(five, ten);\
                    !-/*5;\
                    5 < 10 > 5;\
                    \
                    if (5 < 10) {\
                        return true;\
                    } else {\
                        return false;\
                    }\
                    \
                    10 == 10;\
                    10 != 9;";

    Lexer_t* lexer = createLexer(input);

    static Token_t expToken[] = {
        {TOKEN_LET, "let"},
		{TOKEN_IDENT, "five"},
		{TOKEN_ASSIGN, "="},
		{TOKEN_INT, "5"},
		{TOKEN_SEMICOLON, ";"},
		{TOKEN_LET, "let"},
		{TOKEN_IDENT, "ten"},
		{TOKEN_ASSIGN, "="},
		{TOKEN_INT, "10"},
		{TOKEN_SEMICOLON, ";"},
		{TOKEN_LET, "let"},
		{TOKEN_IDENT, "add"},
		{TOKEN_ASSIGN, "="},
		{TOKEN_FUNCTION, "fn"},
		{TOKEN_LPAREN, "("},
		{TOKEN_IDENT, "x"},
		{TOKEN_COMMA, ","},
		{TOKEN_IDENT, "y"},
		{TOKEN_RPAREN, ")"},
		{TOKEN_LBRACE, "{"},
		{TOKEN_IDENT, "x"},
		{TOKEN_PLUS, "+"},
		{TOKEN_IDENT, "y"},
		{TOKEN_SEMICOLON, ";"},
		{TOKEN_RBRACE, "}"},
		{TOKEN_SEMICOLON, ";"},
		{TOKEN_LET, "let"},
		{TOKEN_IDENT, "result"},
		{TOKEN_ASSIGN, "="},
		{TOKEN_IDENT, "add"},
		{TOKEN_LPAREN, "("},
		{TOKEN_IDENT, "five"},
		{TOKEN_COMMA, ","},
		{TOKEN_IDENT, "ten"},
		{TOKEN_RPAREN, ")"},
		{TOKEN_SEMICOLON, ";"},
		{TOKEN_BANG, "!"},
		{TOKEN_MINUS, "-"},
		{TOKEN_SLASH, "/"},
		{TOKEN_ASTERISK, "*"},
		{TOKEN_INT, "5"},
		{TOKEN_SEMICOLON, ";"},
		{TOKEN_INT, "5"},
		{TOKEN_LT, "<"},
		{TOKEN_INT, "10"},
		{TOKEN_GT, ">"},
		{TOKEN_INT, "5"},
		{TOKEN_SEMICOLON, ";"},
		{TOKEN_IF, "if"},
		{TOKEN_LPAREN, "("},
		{TOKEN_INT, "5"},
		{TOKEN_LT, "<"},
		{TOKEN_INT, "10"},
		{TOKEN_RPAREN, ")"},
		{TOKEN_LBRACE, "{"},
		{TOKEN_RETURN, "return"},
		{TOKEN_TRUE, "true"},
		{TOKEN_SEMICOLON, ";"},
		{TOKEN_RBRACE, "}"},
		{TOKEN_ELSE, "else"},
		{TOKEN_LBRACE, "{"},
		{TOKEN_RETURN, "return"},
		{TOKEN_FALSE, "false"},
		{TOKEN_SEMICOLON, ";"},
		{TOKEN_RBRACE, "}"},
		{TOKEN_INT, "10"},
		{TOKEN_EQ, "=="},
		{TOKEN_INT, "10"},
		{TOKEN_SEMICOLON, ";"},
		{TOKEN_INT, "10"},
		{TOKEN_NOT_EQ, "!="},
		{TOKEN_INT, "9"},
		{TOKEN_SEMICOLON, ";"},
		{TOKEN_EOF, ""}
    };

    int32_t numTests = sizeof(expToken) / sizeof(Token_t);
    Token_t* tok;

    char msg[30];
    for (int32_t i = 0; i < numTests; i++) {
        sprintf(msg, "Test_%d", i+1);

        tok = lexerNextToken(lexer);
        TEST_ASSERT_NOT_NULL_MESSAGE(tok, msg);

        TEST_ASSERT_EQUAL_INT_MESSAGE(expToken[i].type, tok->type , msg); 
        TEST_ASSERT_EQUAL_STRING_LEN_MESSAGE(expToken[i].literal, tok->literal, tok->len, msg);
    }
}





// not needed when using generate_test_runner.rb
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(lexer_simple_test);
    RUN_TEST(lexer_test_monkey1);
    RUN_TEST(lexer_test_monkey2);
    return UNITY_END();
}