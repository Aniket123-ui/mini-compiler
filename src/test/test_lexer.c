#include <stdio.h>  // ✅ Needed for printf
#include "../lexer/lexer.h"

void test_lexer() {
    printf("\nRunning Lexer Tests...\n");

    const char* input = "42 + 23";
    Lexer* lexer = create_lexer(input);

    Token* token;
    while ((token = get_next_token(lexer)) && token->type != TOKEN_EOF) {
        printf("Token: %d, Value: %s\n", token->type, token->value);
        free_token(token);
    }

    free_lexer(lexer);
}
