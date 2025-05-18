#include <stdio.h>
#include <stdlib.h>
#include "interpreter.h"

Interpreter* create_interpreter(Parser* parser) {
    Interpreter* interpreter = (Interpreter*)malloc(sizeof(Interpreter));
    interpreter->parser = parser;
    return interpreter;
}

static int visit(ASTNode* node) {
    if (node->type == AST_NUMBER) {
        return node->value;
    }

    if (node->type == AST_BINARY_OP) {
        switch (node->binop.op_type) {
            case OP_ADD:
                return visit(node->binop.left) + visit(node->binop.right);
            case OP_SUB:
                return visit(node->binop.left) - visit(node->binop.right);
            case OP_MUL:
                return visit(node->binop.left) * visit(node->binop.right);
            case OP_DIV:
                return visit(node->binop.left) / visit(node->binop.right);
            default:
                printf("Error: Unknown binary operator\n");
                exit(1);
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