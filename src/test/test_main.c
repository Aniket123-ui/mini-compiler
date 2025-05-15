#include "test_framework.h"
#include <stdio.h>

// Forward declarations of test suites
void test_lexer(TestStats* stats);
void test_parser(TestStats* stats);
void test_semantic(TestStats* stats);
void test_optimizer(TestStats* stats);
void test_codegen(TestStats* stats);

int main() {
    TestStats stats;
    init_test_stats(&stats);

    // Run all test suites
    test_lexer(&stats);
    test_parser(&stats);
    test_semantic(&stats);
    test_optimizer(&stats);
    test_codegen(&stats);

    // Print final results
    print_test_results(&stats);

    return stats.tests_failed > 0 ? 1 : 0;
}