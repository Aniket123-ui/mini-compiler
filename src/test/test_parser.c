#include "test_framework.h"
#include "../parser/parser.h"

void test_parser(TestStats* stats) {
    printf("\nRunning Parser Tests...\n");

    // Test basic expression
    {
        Lexer* lexer = create_lexer("42");
        Parser* parser = create_parser(lexer);
        ASTNode* ast = parse(parser);
        stats->tests_run++;
        if (assert_int_equals(AST_NUMBER, ast->type, "Test number parsing")) {
            stats->tests_passed++;
        } else {
            stats->tests_failed++;
        }
        free_ast(ast);
        free_parser(parser);
        free_lexer(lexer);
    }

    // Test binary operation
    {
        Lexer* lexer = create_lexer("1 + 2");
        Parser* parser = create_parser(lexer);
        ASTNode* ast = parse(parser);
        stats->tests_run++;
        if (assert_int_equals(AST_BINOP, ast->type, "Test binary operation")) {
            stats->tests_passed++;
        } else {
            stats->tests_failed++;
        }
        free_ast(ast);
        free_parser(parser);
        free_lexer(lexer);
    }
}