#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

// Function to report errors with line and column information
void report_error(int line, int column, const char* format, ...);

// Function to report warnings with line and column information
void report_warning(int line, int column, const char* format, ...);

#endif