#include "unity.h"
#include "parser.h"

void testLetStatement(Statement_t* s, const char* name);
void checkParserErrors(Parser_t* parser);

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

void parser_basic_test(void) {
    const char* input = "let x = 5;\
                    let y = 10;\
                    let foobar = 838383;";

    Lexer_t* l = createLexer(input);
    Parser_t* p = createParser(l);
    
    Program_t* program = parserParseProgram(p);
    
    checkParserErrors(p);


    TEST_ASSERT_NOT_NULL_MESSAGE(program, "parserParseProgram returned NULL");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(3u, programGetStatementCount(program), "program does not contain 3 statements");
    
    const char* tests[] = {
        "x", "y", "foobar"
    };
    int num_tests = 3;
    Statement_t** statements = programGetStatements(program);

    for (int i = 0 ; i < num_tests; i++) {
        testLetStatement(statements[i], tests[i]);
    }
    
    cleanupParser(&p);
}



void testLetStatement(Statement_t* s, const char* name) {
    TEST_ASSERT_EQUAL_STRING_MESSAGE("let", nodeTokenLiteral((Node_t*)s->value), "Check statement literal!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(STATEMENT_LET, s->type, "Check statement type!");

    LetStatement_t* letStmt = (LetStatement_t*) s->value;
    TEST_ASSERT_EQUAL_STRING_MESSAGE(name, letStmt->name->value, "Check name value!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(name, nodeTokenLiteral((Node_t*)letStmt->name), "Check name literal!");
}


void checkParserErrors(Parser_t* parser) {
    uint32_t errCnt = parserGetErrorCount(parser);
    const char** errors = parserGetErrors(parser);
    for (uint32_t i = 0; i < errCnt; i++) 
    {
        TEST_MESSAGE(errors[i]);
    }

    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0u, errCnt, "Parser has errors!");
}

// not needed when using generate_test_runner.rb
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(parser_basic_test);
    return UNITY_END();
}