#include "parser/ast.h"
#include<stdlib.h>
#include <stdio.h>

int main() {
    Token* tok = malloc(sizeof(Token));
    tok->type = TOKEN_NUMBER;
    tok->value = "42";
    tok->line = 1;
    tok->column = 5;

    ASTNode* num_node = create_number_node(42, tok);

    printf("Node type: %d\n", num_node->type);
    printf("Value: %d\n", num_node->value);
    printf("Token: %s at line %d\n", num_node->token->value, num_node->token->line);

    free(num_node->token);
    free(num_node);

    return 0;
}
