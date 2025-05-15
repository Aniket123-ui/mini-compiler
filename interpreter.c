#include "interpreter.h"

Interpreter* create_interpreter(Parser* parser) {
    Interpreter* interpreter = (Interpreter*)malloc(sizeof(Interpreter));
    interpreter->parser = parser;
    return interpreter;
}

static int visit(ASTNode* node) {
    if (node->type == AST_NUMBER) {
        return node->token->value;
    }

    if (node->type == AST_BINOP) {
        if (node->token->type == TOKEN_PLUS) {
            return visit(node->left) + visit(node->right);
        } else if (node->token->type == TOKEN_MINUS) {
            return visit(node->left) - visit(node->right);
        }
    }

    printf("Error: Invalid node type\n");
    exit(1);
}

int interpret(Interpreter* interpreter) {
    ASTNode* tree = parse(interpreter->parser);
    int result = visit(tree);
    free_ast(tree);
    return result;
}

void free_interpreter(Interpreter* interpreter) {
    free(interpreter);
}