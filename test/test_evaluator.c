#include <stdlib.h> 

#include "unity.h"
#include "evaluator.h"
#include "parser.h"
#include "ast.h"
#include "utils.h"


void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

Object_t* testEval(const char* input);
void testIntegerObject(Object_t* obj, int64_t expected);
void testBooleanObject(Object_t* obj, bool expected);


void evaluatorTestEvalIntegerExpression() {
    typedef struct TestCase {
        const char* input;
        int64_t expected;
    } TestCase_t;

    TestCase_t tests[] = {
        {"5", 5}, 
        {"10", 10}
    };

    uint32_t cnt = sizeof(tests) / sizeof(TestCase_t);

    for (uint32_t i = 0; i < cnt; i++ ) {
        TestCase_t *tc = &tests[i];
        Object_t* evalRes = testEval(tc->input);
        testIntegerObject(evalRes, tc->expected);
        cleanupObject(&evalRes);
    }
}

void evaluatorTestEvalBooleanExpression() {
    typedef struct TestCase {
        const char* input;
        bool expected;
    } TestCase_t;

    TestCase_t tests[] = {
        {"true", true}, 
        {"false", false}
    };

    uint32_t cnt = sizeof(tests) / sizeof(TestCase_t);

    for (uint32_t i = 0; i < cnt; i++ ) {
        TestCase_t *tc = &tests[i];
        Object_t* evalRes = testEval(tc->input);
        testBooleanObject(evalRes, tc->expected);
        cleanupObject(&evalRes);
    }
}


void evaluatorTestBangOperator() {
    typedef struct TestCase {
        const char* input;
        bool expected;
    } TestCase_t;

    TestCase_t tests[] = {
        {"!true", false},
        {"!false", true},
        {"!5", false},
        {"!!true", true},
        {"!!false", false},
        {"!!5", true}
    };

    uint32_t cnt = sizeof(tests) / sizeof(TestCase_t);

    for (uint32_t i = 0; i < cnt; i++ ) {
        TestCase_t *tc = &tests[i];
        Object_t* evalRes = testEval(tc->input);
        testBooleanObject(evalRes, tc->expected);
        cleanupObject(&evalRes);
    }
}


Object_t* testEval(const char* input) {
    Lexer_t* lexer = createLexer(input);
    Parser_t* parser = createParser(lexer);
    Program_t* program = parserParseProgram(parser);

    Object_t* ret = evalProgram(program);

    cleanupProgram(&program);
    cleanupParser(&parser);

    return ret; 
}

void testIntegerObject(Object_t* obj, int64_t expected) {
    TEST_ASSERT_NOT_NULL_MESSAGE(obj, "Object is null");
    TEST_ASSERT_EQUAL_INT_MESSAGE(OBJECT_INTEGER, obj->type, "Object type not OBJECT_INTEGER");
    Integer_t* intObj = (Integer_t*) obj;
    TEST_ASSERT_EQUAL_INT64_MESSAGE(expected, intObj->value, "Object value is not correct");
}

void testBooleanObject(Object_t* obj, bool expected) {
    TEST_ASSERT_NOT_NULL_MESSAGE(obj, "Object is null");
    TEST_ASSERT_EQUAL_INT_MESSAGE(OBJECT_BOOLEAN, obj->type, "Object type not OBJECT_BOOLEAN");
    Boolean_t* boolObj = (Boolean_t*) obj;
    TEST_ASSERT_EQUAL_INT_MESSAGE(expected, boolObj->value, "Object value is not correct");
}


// not needed when using generate_test_runner.rb
int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(evaluatorTestEvalIntegerExpression);
    RUN_TEST(evaluatorTestEvalBooleanExpression);
    RUN_TEST(evaluatorTestBangOperator);

    return UNITY_END();
}