#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include "../parser/ast.h"
#include <stdbool.h>

typedef struct {
    int constants_folded;
    int dead_code_removed;
    bool modified;  // Track if optimizations were applied
} OptimizationStats;

ASTNode* optimize_ir(ASTNode* ast, OptimizationStats* stats);

#endif
ASTNode* constant_folding(ASTNode* node, OptimizationStats* stats);
ASTNode* dead_code_elimination(ASTNode* node, OptimizationStats* stats);