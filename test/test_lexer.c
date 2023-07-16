#include "unity.h"
#include "lexer.h"

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

void test_get_zero(void) {
    int exp_zero = 1;
    TEST_ASSERT_EQUAL_INT(exp_zero, get_zero()); 
}


// not needed when using generate_test_runner.rb
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_get_zero);
    return UNITY_END();
}