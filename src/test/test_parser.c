#include <stdio.h>  // ✅ Needed for printf
#include "../parser/parser.h"

void test_parser() {
    printf("\nRunning Parser Tests...\n");

    const char* input = "7 + 8";
    Lexer* lexer = create_lexer(input);
    Parser* parser = create_parser(lexer);

    ASTNode* ast = parse(parser);
    if (ast != NULL) {
        printf("AST root node type: %d\n", ast->type);
        free_ast(ast);
    }

    free_parser(parser);
    free_lexer(lexer);
}
