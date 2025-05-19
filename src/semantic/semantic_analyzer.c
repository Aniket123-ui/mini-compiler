#include "../parser/ast.h"
#include "semantic_analyzer.h"
#include "../utils/symbol_table.h"
#include "../utils/error_handler.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int semantic_error = 0;

// Symbol table entry with type
typedef struct VarEntry {
    char* name;
    int type; // Only int supported for now
    struct VarEntry* next;
} VarEntry;

typedef struct Scope {
    VarEntry* vars;
    struct Scope* parent;
} Scope;

static Scope* scope_push(Scope* parent) {
    Scope* s = malloc(sizeof(Scope));
    s->vars = NULL;
    s->parent = parent;
    return s;
}

static void scope_pop(Scope* s) {
    VarEntry* v = s->vars;
    while (v) {
        VarEntry* next = v->next;
        free(v->name);
        free(v);
        v = next;
    }
    free(s);
}

static int scope_insert(Scope* s, const char* name, int type) {
    for (VarEntry* v = s->vars; v; v = v->next) {
        if (strcmp(v->name, name) == 0) return 0; // redeclaration
    }
    VarEntry* v = malloc(sizeof(VarEntry));
    v->name = strdup(name);
    v->type = type;
    v->next = s->vars;
    s->vars = v;
    return 1;
}

static int scope_lookup(Scope* s, const char* name) {
    for (Scope* cur = s; cur; cur = cur->parent) {
        for (VarEntry* v = cur->vars; v; v = v->next) {
            if (strcmp(v->name, name) == 0) return 1;
        }
    }
    return 0;
}

static void analyze_node(ASTNode* node, Scope* scope, int* found_return) {
    if (!node) return;
    switch (node->type) {
        case AST_FUNCTION:
            // New scope for function
            {
                Scope* func_scope = scope_push(scope);
                *found_return = 0;
                analyze_node(node->function.body, func_scope, found_return);
                if (!*found_return) {
                    report_error(ERROR_SEMANTIC, node->token ? node->token->line : 0, node->token ? node->token->column : 0, "Missing return statement in function");
                    semantic_error = 1;
                }
                scope_pop(func_scope);
            }
            break;
        case AST_BLOCK:
        case AST_COMPOUND:
            {
                Scope* block_scope = scope_push(scope);
                for (size_t i = 0; i < node->block.count; ++i) {
                    analyze_node(node->block.statements[i], block_scope, found_return);
                }
                scope_pop(block_scope);
            }
            break;
        case AST_DECLARATION:
            if (!scope_insert(scope, node->declaration.name, 0)) {
                report_error(ERROR_REDEFINITION, node->token ? node->token->line : 0, node->token ? node->token->column : 0, "Redeclaration of variable '%s'", node->declaration.name);
                semantic_error = 1;
            }
            if (node->declaration.init)
                analyze_node(node->declaration.init, scope, found_return);
            break;
        case AST_ASSIGNMENT:
            if (!scope_lookup(scope, node->assignment.name)) {
                report_error(ERROR_UNDEFINED_VAR, node->token ? node->token->line : 0, node->token ? node->token->column : 0, "Assignment to undeclared variable '%s'", node->assignment.name);
                semantic_error = 1;
            }
            analyze_node(node->assignment.value, scope, found_return);
            break;
        case AST_IDENTIFIER:
            if (!scope_lookup(scope, node->identifier)) {
                report_error(ERROR_UNDEFINED_VAR, node->token ? node->token->line : 0, node->token ? node->token->column : 0, "Use of undeclared variable '%s'", node->identifier);
                semantic_error = 1;
            }
            break;
        case AST_BINARY_OP:
            analyze_node(node->binop.left, scope, found_return);
            analyze_node(node->binop.right, scope, found_return);
            break;
        case AST_IF:
            analyze_node(node->if_stmt.condition, scope, found_return);
            analyze_node(node->if_stmt.then_branch, scope, found_return);
            if (node->if_stmt.else_branch)
                analyze_node(node->if_stmt.else_branch, scope, found_return);
            break;
        case AST_WHILE:
            analyze_node(node->while_stmt.condition, scope, found_return);
            analyze_node(node->while_stmt.body, scope, found_return);
            break;
        case AST_RETURN:
            *found_return = 1;
            analyze_node(node->return_stmt.expr, scope, found_return);
            break;
        default:
            break;
    }
}

void analyze_semantics(ASTNode* root) {
    fprintf(stderr, "[DEBUG] Entering analyze_semantics (new)\n");
    Scope* global = scope_push(NULL);
    int found_return = 0;
    analyze_node(root, global, &found_return);
    scope_pop(global);
    fprintf(stderr, "[DEBUG] Exiting analyze_semantics (new)\n");
}

int get_semantic_error() {
    return semantic_error;
}
