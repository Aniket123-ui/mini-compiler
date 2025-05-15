#include "test_framework.h"
#include "../lexer/lexer.h"

void test_lexer(TestStats* stats) {
    printf("\nRunning Lexer Tests...\n");

    // Test basic tokenization
    {
        Lexer* lexer = create_lexer("42");
        Token* token = get_next_token(lexer);
        stats->tests_run++;
        if (assert_int_equals(TOKEN_NUMBER, token->type, "Test number token")) {
            stats->tests_passed++;
        } else {
            stats->tests_failed++;
        }
        free_token(token);
        free_lexer(lexer);
    }

    // Test operators
    {
        Lexer* lexer = create_lexer("+ - * /");
        Token* token = get_next_token(lexer);
        stats->tests_run++;
        if (assert_int_equals(TOKEN_PLUS, token->type, "Test plus operator")) {
            stats->tests_passed++;
        } else {
            stats->tests_failed++;
        }
        free_token(token);
        free_lexer(lexer);
    }

    // Test invalid input
    {
        Lexer* lexer = create_lexer("@");
        Token* token = get_next_token(lexer);
        stats->tests_run++;
        if (assert_int_equals(TOKEN_ERROR, token->type, "Test invalid character")) {
            stats->tests_passed++;
        } else {
            stats->tests_failed++;
        }
        free_token(token);
        free_lexer(lexer);
    }
}