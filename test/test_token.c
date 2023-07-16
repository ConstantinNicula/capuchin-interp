#include "unity.h"
#include "token.h"


void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

void token_create_test(void) {
    Token_t* tok = createToken(TOKEN_EOF, "");
    TEST_ASSERT_NOT_NULL(tok);
}


void token_create_test_2(void) {
    Token_t* tok = createToken(TOKEN_ASSIGN, "=");

    TEST_ASSERT_EQUAL_INT_MESSAGE(TOKEN_ASSIGN, tok->type, "Token type check");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("=", tok->literal, "Token literal check");
}




// not needed when using generate_test_runner.rb
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(token_create_test);
     RUN_TEST(token_create_test_2);
    return UNITY_END();
}