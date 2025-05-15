#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

// Error types enum for specific error categorization
typedef enum {
    ERROR_LEXICAL,
    ERROR_SYNTAX,
    ERROR_SEMANTIC,
    ERROR_TYPE,
    ERROR_UNDEFINED_VAR,
    ERROR_REDEFINITION
} ErrorType;

// Function to report errors with line, column, and error type information
void report_error(ErrorType type, int line, int column, const char* format, ...);

// Function to report warnings with line, column, and specific warning information
void report_warning(int line, int column, const char* format, ...);

// Function to get error type string representation
const char* get_error_type_string(ErrorType type);

#endif