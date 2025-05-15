#include "error_handler.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

void report_error(int line, int column, const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    fprintf(stderr, "Error at line %d, column %d: ", line, column);
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    
    va_end(args);
    exit(1);
}

void report_warning(int line, int column, const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    fprintf(stderr, "Warning at line %d, column %d: ", line, column);
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    
    va_end(args);
}