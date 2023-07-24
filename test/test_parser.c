#include <stdlib.h> 

#include "unity.h"
#include "parser.h"
#include "ast.h"
#include "utils.h"


void testLetStatement(Statement_t* s, const char* name);
void testIntegerLiteral(Expression_t* exp, int64_t value);
void checkParserErrors(Parser_t* parser);

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

void parseTestLetStatement(void) {
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
    cleanupProgram(&program);
}

void parserTestInvalidLetStatement(void) {
    const char* input = "let x  5;\
                    let 10;\
                    let  = 838383;";

    Lexer_t* l = createLexer(input);
    Parser_t* p = createParser(l);
    
    Program_t* program = parserParseProgram(p);


    cleanupParser(&p);
    cleanupProgram(&program);
}

void parserTestReturnStatements() {
    const char* input = "return 5;\
                        return 10;\
                        return 993322;";

    Lexer_t* l = createLexer(input);
    Parser_t* p = createParser(l);
    
    Program_t* program = parserParseProgram(p);
    checkParserErrors(p);


    TEST_ASSERT_NOT_NULL_MESSAGE(program, "ParserParseProgram returned NULL!");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(3u, programGetStatementCount(program), "Program does not contain 3 statements!");
    
    int num_tests = 3;
    Statement_t** statements = programGetStatements(program);

    for (int i = 0 ; i < num_tests; i++) {
        Statement_t* s = statements[i];
        TEST_ASSERT_EQUAL_INT_MESSAGE(STATEMENT_RETURN, s->type, "Check statement type!");
        TEST_ASSERT_EQUAL_STRING_MESSAGE("return", statementTokenLiteral(s), "Check statement literal!");
    }
    
    cleanupParser(&p);
    cleanupProgram(&program);
}


void parserTestIdentifierExpression() {
    const char* input = "foobar;";

    Lexer_t* lexer = createLexer(input);
    Parser_t* parser = createParser(lexer);
    Program_t* prog = parserParseProgram(parser);
    checkParserErrors(parser);

    TEST_ASSERT_EQUAL_INT_MESSAGE(1, programGetStatementCount(prog), "Not enough statements in code");
    Statement_t** stmts = programGetStatements(prog);

    TEST_ASSERT_EQUAL_INT_MESSAGE(STATEMENT_EXPRESSION, stmts[0]->type, "Statements[0] is not Expression Statement");
    ExpressionStatement_t* es = (ExpressionStatement_t*)stmts[0]->value;

    TEST_ASSERT_EQUAL_INT_MESSAGE(EXPRESSION_IDENTIFIER, es->expression->type, "Expression type not EXPRESSION_IDENTIFIER");
    Identifier_t* ident = (Identifier_t*)es->expression->value;

    TEST_ASSERT_EQUAL_STRING_MESSAGE("foobar", ident->value, "Check ident value");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("foobar", ident->token->literal, "Check literal value");

    cleanupParser(&parser);
    cleanupProgram(&prog);
}


void parserTestIntegerLiteralExpression() {
    const char* input = "5;";

    Lexer_t* lexer = createLexer(input);
    Parser_t* parser = createParser(lexer);
    Program_t* prog = parserParseProgram(parser);
    checkParserErrors(parser);


    TEST_ASSERT_EQUAL_INT_MESSAGE(1, programGetStatementCount(prog), "Not enough statements in code");
    
    Statement_t** stmts = programGetStatements(prog);
    TEST_ASSERT_EQUAL_INT_MESSAGE(STATEMENT_EXPRESSION, stmts[0]->type, "Statements[0] is not Expression Statement");
    ExpressionStatement_t* es = (ExpressionStatement_t*)stmts[0]->value;

    testIntegerLiteral(es->expression, 5);

    cleanupParser(&parser);
    cleanupProgram(&prog);
}

void parserTestPrefixExpressions() {
    typedef struct TestCase {
        const char* input;
        const char* operator;
        int64_t integerValue;
    } TestCase_t;
    
    TestCase_t prefixTests[] = {
        {"!5;", "!", 5},
        {"-15;", "-", 15}
    };

    uint32_t cnt = sizeof(prefixTests) / sizeof(TestCase_t);

    for (uint32_t i = 0; i < cnt; ++i) {
        TestCase_t *tc = &prefixTests[i];

        Lexer_t* lexer = createLexer(tc->input);
        Parser_t* parser = createParser(lexer);
        Program_t* prog = parserParseProgram(parser);
        checkParserErrors(parser);

        TEST_ASSERT_EQUAL_INT_MESSAGE(1, programGetStatementCount(prog), "Statement count check!");

        Statement_t** stmts = programGetStatements(prog);
        TEST_ASSERT_EQUAL_INT_MESSAGE(STATEMENT_EXPRESSION, stmts[0]->type, "Statements[0] is not Expression Statement");

        ExpressionStatement_t* es = (ExpressionStatement_t*)stmts[0]->value;
        TEST_ASSERT_EQUAL_INT_MESSAGE(EXPRESSION_PREFIX_EXPRESSION, es->expression->type, "Expression type not EXPRESSION_PREFIX_EXPRESSION");

        PrefixExpression_t* pe = (PrefixExpression_t*)es->expression->value;
        TEST_ASSERT_EQUAL_STRING_MESSAGE(tc->operator, pe->operator, "Operator check!");
        testIntegerLiteral(pe->right, tc->integerValue);

        cleanupProgram(&prog);
        cleanupParser(&parser);
    }
}


void testLetStatement(Statement_t* s, const char* name) {
    TEST_ASSERT_EQUAL_STRING_MESSAGE("let", statementTokenLiteral(s), "Check statement literal!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(STATEMENT_LET, s->type, "Check statement type!");

    LetStatement_t* letStmt = (LetStatement_t*) s->value;
    TEST_ASSERT_EQUAL_STRING_MESSAGE(name, letStmt->name->value, "Check name value!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(name, letStmt->name->token->literal, "Check name literal!");
}


void testIntegerLiteral(Expression_t* exp, int64_t value) {
    TEST_ASSERT_EQUAL_INT_MESSAGE(EXPRESSION_INTEGER_LITERAL, exp->type, "Expression type not EXPRESSION_INTEGER_LITERAL");
    IntegerLiteral_t* il = (IntegerLiteral_t*)exp->value;

    TEST_ASSERT_EQUAL_INT_MESSAGE(value, il->value, "Check ident value");

    char* lit = strFormat("%d", value);
    TEST_ASSERT_EQUAL_STRING_MESSAGE(lit, il->token->literal, "Check literal value");
    free(lit);
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
    
    RUN_TEST(parseTestLetStatement);
    RUN_TEST(parserTestInvalidLetStatement);
    RUN_TEST(parserTestReturnStatements);
    RUN_TEST(parserTestIdentifierExpression);
    RUN_TEST(parserTestIntegerLiteralExpression);
    RUN_TEST(parserTestPrefixExpressions);
    return UNITY_END();
}