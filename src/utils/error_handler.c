#include "error_handler.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

const char* get_error_type_string(ErrorType type) {
    switch (type) {
        case ERROR_LEXICAL:
            return "Lexical Error";
        case ERROR_SYNTAX:
            return "Syntax Error";
        case ERROR_SEMANTIC:
            return "Semantic Error";
        case ERROR_TYPE:
            return "Type Error";
        case ERROR_UNDEFINED_VAR:
            return "Undefined Variable";
        case ERROR_REDEFINITION:
            return "Redefinition Error";
        default:
            return "Unknown Error";
    }
}

void report_error(ErrorType type, int line, int column, const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    fprintf(stderr, "\033[1;31m%s\033[0m at line %d, column %d: ", 
            get_error_type_string(type), line, column);
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    
    // Print the source line with error indicator (if source line is available)
    // This could be implemented by storing source lines in a buffer
    
    va_end(args);
    exit(1);
}

void report_warning(int line, int column, const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    fprintf(stderr, "\033[1;33mWarning\033[0m at line %d, column %d: ", line, column);
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    
    va_end(args);
}