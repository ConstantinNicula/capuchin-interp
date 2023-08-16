#include <stdlib.h> 

#include "unity.h"
#include "evaluator.h"
#include "parser.h"
#include "ast.h"
#include "utils.h"
#include "gc.h"

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

typedef enum {
    EXPECT_INTEGER,
    EXPECT_BOOL, 
    EXPECT_STRING,
    EXPECT_NULL
} ExpectType_t;

typedef struct GenericExpect {
    ExpectType_t type;
    union {
        int64_t il;
        bool bl;
        const char* sl;
    };
}GenericExpect_t;

#define _BOOL(x) (GenericExpect_t){.type=EXPECT_BOOL, .bl=(x)}
#define _INT(x) (GenericExpect_t){.type=EXPECT_INTEGER, .il=(x)}
#define _STRING(x) (GenericExpect_t){.type=EXPECT_STRING, .sl=(x)}
#define _NIL (GenericExpect_t){.type=EXPECT_NULL}

Object_t* testEval(const char* input);
void testIntegerObject(Object_t* obj, int64_t expected);
void testBooleanObject(Object_t* obj, bool expected);
void testNullObject(Object_t* obj);
void testErrorObject(Object_t* obj, const char* expected);


void evaluatorTestEvalIntegerExpression() {
    typedef struct TestCase {
        const char* input;
        int64_t expected;
    } TestCase_t;

    TestCase_t tests[] = {
        {"5", 5}, 
        {"10", 10},
        {"-5", -5},
        {"-10", -10},
        {"5 + 5 + 5 + 5 - 10", 10},
        {"2 * 2 * 2 * 2 * 2", 32},
        {"-50 + 100 + -50", 0},
        {"5 * 2 + 10", 20},
        {"5 + 2 * 10", 25},
        {"20 + 2 * -10", 0},
        {"50 / 2 * 2 + 10", 60},
        {"2 * (5 + 10)", 30},
        {"3 * 3 * 3 + 10", 37},
        {"3 * (3 * 3) + 10", 37},
        {"(5 + 10 * 2 + 15 / 3) * 2 + -10", 50},
    };

    uint32_t cnt = sizeof(tests) / sizeof(TestCase_t);

    for (uint32_t i = 0; i < cnt; i++ ) {
        TestCase_t *tc = &tests[i];
        Object_t* evalRes = testEval(tc->input);
        testIntegerObject(evalRes, tc->expected);
        gcFreeExtRef(evalRes);
    }
}

void evaluatorTestEvalBooleanExpression() {
    typedef struct TestCase {
        const char* input;
        bool expected;
    } TestCase_t;

    TestCase_t tests[] = {
        {"true", true}, 
        {"false", false},
        {"1 < 2", true},
        {"1 > 2", false},
        {"1 < 1", false},
        {"1 > 1", false},
        {"1 == 1", true},
        {"1 != 1", false},
        {"1 == 2", false},
        {"1 != 2", true},
        {"true == true", true},
        {"false == false", true},
        {"true == false", false},
        {"true != false", true},
        {"false != true", true},
        {"(1 < 2) == true", true},
        {"(1 < 2) == false", false},
        {"(1 > 2) == true", false},
        {"(1 > 2) == false", true}
    };

    uint32_t cnt = sizeof(tests) / sizeof(TestCase_t);

    for (uint32_t i = 0; i < cnt; i++ ) {
        TestCase_t *tc = &tests[i];
        Object_t* evalRes = testEval(tc->input);
        testBooleanObject(evalRes, tc->expected);
        gcFreeExtRef(evalRes);
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
        gcFreeExtRef(evalRes);
    }
}

void evaluatorTestIfElseExpression() {
    typedef struct TestCase {
        const char* input;
        GenericExpect_t expected;
    } TestCase_t;

    TestCase_t tests[] = {
        {"if (true) { 10 }", _INT(10)},
        {"if (false) { 10 }", _NIL},
        {"if (1) { 10 }", _INT(10)},
        {"if (1 < 2) { 10 }", _INT(10)},
        {"if (1 > 2) { 10 }", _NIL},
        {"if (1 > 2) { 10 } else { 20 }", _INT(20)},
        {"if (1 < 2) { 10 } else { 20 }", _INT(10)}
    };

    uint32_t cnt = sizeof(tests) / sizeof(TestCase_t);

    for (uint32_t i = 0; i < cnt; i++ ) {
        TestCase_t *tc = &tests[i];
        Object_t* evalRes = testEval(tc->input);

        switch(tc->expected.type) {
            case EXPECT_INTEGER:
                testIntegerObject(evalRes, tc->expected.il);
                break;
            default: 
                testNullObject(evalRes);
        }

        gcFreeExtRef(evalRes);
    } 
}

void evaluatorTestReturnStatements() {
    typedef struct TestCase {
        const char* input;
        int64_t expected;
    } TestCase_t;

    TestCase_t tests[] = {
        {"return 10;", 10},
        {"return 10; 9;", 10},
        {"return 2 * 5; 9;", 10},
        {"9; return 2 * 5; 9;", 10},
        {"if(10 > 1) {if ( 10>1) {return 10;} return 1;}", 10},
    };

    uint32_t cnt = sizeof(tests) / sizeof(TestCase_t);

    for (uint32_t i = 0; i < cnt; i++ ) {
        TestCase_t *tc = &tests[i];
        Object_t* evalRes = testEval(tc->input);
        testIntegerObject(evalRes, tc->expected);
        gcFreeExtRef(evalRes);
    }
}

void evaluatorTestLetStatements() {
    typedef struct {
        const char* input;
        int64_t expected;
    } TestCase_t;

    TestCase_t tests[] = {
        {"let a = 5; a;", 5},
        {"let a = 5 * 5; a;", 25},
        {"let a = 5; let b = a; b;", 5},
        {"let a = 5; let b =a; let c = a + b + 5;c;", 15},
    };

    uint32_t cnt = sizeof(tests) / sizeof(TestCase_t);
    for (uint32_t i = 0; i < cnt; i++ ) {
        TestCase_t *tc = &tests[i];
        Object_t* evalRes = testEval(tc->input);
        testIntegerObject(evalRes, tc->expected);
        gcFreeExtRef(evalRes);
    }
}

void evaluatorTestErrorHandling() {
    typedef struct TestCase {
        const char* input;
        const char* expected;
    } TestCase_t;

    TestCase_t tests[] = {
        {"5 + true;",
         "type mismatch: INTEGER + BOOLEAN"},
        {"5 + true; 5;",
        "type mismatch: INTEGER + BOOLEAN"},
        {"-true",
        "unknown operator: -BOOLEAN"},
        {"true + false;",
        "unknown operator: BOOLEAN + BOOLEAN"},
        {"5; true + false; 5",
        "unknown operator: BOOLEAN + BOOLEAN"},
        {"if (10 > 1) { true + false; }",
        "unknown operator: BOOLEAN + BOOLEAN"},
        {"if (10 > 1) {"
            "if (10 > 1) {"
                "return true + false;"
            "}"
            "return 1;"
        "}",
        "unknown operator: BOOLEAN + BOOLEAN"},
        {"foobar",
         "identifier not found: foobar"}
    };

    uint32_t cnt = sizeof(tests) / sizeof(TestCase_t);

    for (uint32_t i = 0; i < cnt; i++ ) {
        TestCase_t *tc = &tests[i];
        Object_t* evalRes = testEval(tc->input);

        testErrorObject(evalRes, tc->expected);
        gcFreeExtRef(evalRes);
    }
}

void evaluatorTestFunctionObject() {
    const char* input = "fn(x) { x + 2; };";
    Object_t* eval = testEval(input);

    TEST_ASSERT_EQUAL_INT_MESSAGE(OBJECT_FUNCTION, eval->type, "Object in not FUNCTION");
    Function_t* func = (Function_t*) eval;

    TEST_ASSERT_EQUAL_INT_MESSAGE(1, functionGetParameterCount(func), "Wrong number of parameters");
    Identifier_t* param = functionGetParameters(func)[0];
    
    char* identStr = identifierToString(param);
    TEST_ASSERT_EQUAL_STRING_MESSAGE("x", identStr, "Wrong parameter name");
    free(identStr);

    char* bodyStr = blockStatementToString(func->body);
    TEST_ASSERT_EQUAL_STRING_MESSAGE("(x + 2)", bodyStr, "Wrong function body");
    free(bodyStr);

    gcFreeExtRef(eval);
}

void evaluatorTestFunctionApplication() {
    typedef struct {
        const char* input;
        int64_t expected;
    } TestCase_t;

    TestCase_t tests[] = {
        {"let identity = fn(x) { x; }; identity(5);", 5},
        {"let identity = fn(x) { return x; }; identity(5);", 5},
        {"let double = fn(x) { x * 2; }; double(5);", 10},
        {"let add = fn(x, y) { x + y; }; add(5, 5);", 10},
        {"let add = fn(x, y) { x + y; }; add(5 + 5, add(5, 5));", 20},
        {"fn(x) { x; }(5)", 5},
    };

    uint32_t cnt = sizeof(tests) / sizeof(TestCase_t);
    for (uint32_t i = 0; i < cnt; i++ ) {
        TestCase_t *tc = &tests[i];
        Object_t* evalRes = testEval(tc->input);
        testIntegerObject(evalRes, tc->expected);
        gcFreeExtRef(evalRes);
    }
}

void evalatorTestClosures() {
    const char* input = "let newAdder = fn(x) {"
                            "fn(y) { x + y };"
                        "};"
                        "let addTwo = newAdder(2);"
                        "addTwo(2);";

    Object_t* evalRes = testEval(input);
    testIntegerObject(evalRes, 4);
    gcFreeExtRef(evalRes);
}

Object_t* testEval(const char* input) {
    Lexer_t* lexer = createLexer(input);
    Parser_t* parser = createParser(lexer);
    Program_t* program = parserParseProgram(parser);
    Environment_t* env = createEnvironment(NULL);

    Object_t* ret = evalProgram(program, env);
    if (ret->type == OBJECT_ERROR) {
        TEST_MESSAGE(((Error_t*)ret)->message);
    }
    cleanupProgram(&program);
    cleanupParser(&parser);
    gcFreeExtRef(env);

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

void testNullObject(Object_t* obj) {
    TEST_ASSERT_EQUAL_INT_MESSAGE(OBJECT_NULL, obj->type, "Object type not OBJECT_BOOLEAN");
}

void testErrorObject(Object_t* obj, const char* expected) {
    TEST_ASSERT_NOT_NULL_MESSAGE(obj, "Object is null");
    TEST_ASSERT_EQUAL_INT_MESSAGE(OBJECT_ERROR, obj->type, "Object type not OBJECT_ERROR");
    Error_t* errObj = (Error_t*) obj;
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected, errObj->message, "Wrong error message");
}

// not needed when using generate_test_runner.rb
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(evaluatorTestEvalIntegerExpression);
    RUN_TEST(evaluatorTestEvalBooleanExpression);
    RUN_TEST(evaluatorTestBangOperator);
    RUN_TEST(evaluatorTestIfElseExpression);
    RUN_TEST(evaluatorTestReturnStatements);
    RUN_TEST(evaluatorTestErrorHandling);
    RUN_TEST(evaluatorTestLetStatements);
    RUN_TEST(evaluatorTestFunctionObject);
    RUN_TEST(evaluatorTestFunctionApplication);
    RUN_TEST(evalatorTestClosures);
    return UNITY_END();
}