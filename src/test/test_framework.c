#include "test_framework.h"
#include <stdio.h>
#include <string.h>

void init_test_stats(TestStats* stats) {
    stats->tests_run = 0;
    stats->tests_passed = 0;
    stats->tests_failed = 0;
}

bool assert_int_equals(int expected, int actual, const char* test_name) {
    printf("Running test: %s\n", test_name);
    if (expected == actual) {
        printf("PASS: %s\n", test_name);
        return true;
    } else {
        printf("FAIL: %s\n", test_name);
        printf("Expected: %d, Got: %d\n", expected, actual);
        return false;
    }
}

bool assert_str_equals(const char* expected, const char* actual, const char* test_name) {
    printf("Running test: %s\n", test_name);
    if (strcmp(expected, actual) == 0) {
        printf("PASS: %s\n", test_name);
        return true;
    } else {
        printf("FAIL: %s\n", test_name);
        printf("Expected: %s\nGot: %s\n", expected, actual);
        return false;
    }
}

void print_test_results(TestStats* stats) {
    printf("\nTest Summary:\n");
    printf("Total tests: %d\n", stats->tests_run);
    printf("Passed: %d\n", stats->tests_passed);
    printf("Failed: %d\n", stats->tests_failed);
    printf("Success rate: %.2f%%\n", 
           (stats->tests_run > 0) ? 
           ((float)stats->tests_passed / stats->tests_run) * 100 : 0);
}