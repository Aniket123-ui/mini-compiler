#ifndef LEXER_H
#define LEXER_H

// Token types
typedef enum {
    TOKEN_EOF,
    TOKEN_ERROR,    // For error handling
    TOKEN_NUMBER,   // For numeric literals
    TOKEN_PLUS,     // +
    TOKEN_MINUS,    // -
    TOKEN_MULTIPLY, // *
    TOKEN_DIVIDE,   // /
    TOKEN_LPAREN,   // (
    TOKEN_RPAREN,   // )
    TOKEN_ASSIGN,   // =
    TOKEN_SEMICOLON // ;
} TokenType;

// Token structure
typedef struct {
    TokenType type;
    int value;      // For numeric values
    int line;       // Line number for error reporting
    int column;     // Column number for error reporting
} Token;

// Lexer structure
typedef struct {
    const char* input;
    int position;
    int line;
    int column;
} Lexer;

// Lexer functions
Lexer* create_lexer(const char* input);
Token* get_next_token(Lexer* lexer);
void free_lexer(Lexer* lexer);
Token* create_token(TokenType type, int value, int line, int column);
void free_token(Token* token);

#endif