#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <stdbool.h>

typedef struct {
    int tests_run;
    int tests_passed;
    int tests_failed;
} TestStats;

// Test framework functions
void init_test_stats(TestStats* stats);
void run_test_suite(TestStats* stats);
bool assert_int_equals(int expected, int actual, const char* test_name);
bool assert_str_equals(const char* expected, const char* actual, const char* test_name);
void print_test_results(TestStats* stats);

#endif