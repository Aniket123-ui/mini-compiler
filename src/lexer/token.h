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
    TOKEN_LBRACE,     // {
    TOKEN_RBRACE,     // }
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_WHILE,
    TOKEN_RETURN,
    TOKEN_INT,
    TOKEN_PUNCTUATION,  // ✅ Added this for punctuation like , (comma), etc.
    TOKEN_ERROR,
    TOKEN_OPERATOR,
    TOKEN_LT,      // <
    TOKEN_GT,      // >
    TOKEN_LE,      // <=
    TOKEN_GE,      // >=
    TOKEN_EQ,      // ==
    TOKEN_NEQ     // !=
} TokenType;

typedef struct Token {
    TokenType type;
    char* value;   // dynamically allocated string for token text
    int line;      // line number in source code
    int column;    // column number in source code
} Token;

void free_token(Token* token);

#endif // TOKEN_H
