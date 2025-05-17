#ifndef TOKEN_H
#define TOKEN_H

typedef enum {
    TOKEN_EOF,
    TOKEN_NUMBER,
    TOKEN_IDENTIFIER,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MUL,
    TOKEN_DIV,
    TOKEN_ASSIGN,
    TOKEN_SEMICOLON,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,     // <-- Add this for '{'
    TOKEN_RBRACE,     // <-- Add this for '}'
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_WHILE,
    TOKEN_RETURN,
    TOKEN_INT,
    TOKEN_ERROR
} TokenType;




typedef struct Token {
    TokenType type;
    char* value;   // dynamically allocated string for token text
    int line;      // line number in source code
    int column;    // column number in source code
} Token;

// Add this declaration:
void free_token(Token* token);

#endif // TOKEN_H
