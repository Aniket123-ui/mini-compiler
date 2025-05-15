#include <stdio.h>
#include <string.h>
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "semantic/semantic_analyzer.h"
#include "ir/optimizer.h"
#include "interpreter/interpreter.h"
#include "codegen/asm_generator.h"

#define MAX_INPUT 1024

int main() {
    char input[MAX_INPUT];
    FILE* asm_output = fopen("output.asm", "w");
    if (!asm_output) {
        fprintf(stderr, "Failed to open output file\n");
        return 1;
    }

    while (1) {
        printf("calc> ");
        if (fgets(input, MAX_INPUT, stdin) == NULL) {
            break;
        }

        // Remove newline
        input[strcspn(input, "\n")] = 0;

        if (strlen(input) == 0) {
            continue;
        }

        // Create lexer, parser and get AST
        Lexer* lexer = create_lexer(input);
        Parser* parser = create_parser(lexer);
        ASTNode* ast = parse(parser);

        // Perform semantic analysis
        SemanticAnalyzer* analyzer = create_semantic_analyzer();
        if (!analyze_ast(analyzer, ast)) {
            fprintf(stderr, "Semantic analysis failed\n");
            free_semantic_analyzer(analyzer);
            free_ast(ast);
            free_parser(parser);
            free_lexer(lexer);
            continue;
        }
        free_semantic_analyzer(analyzer);

        // Optimize the AST
        OptimizationStats opt_stats = {0};
        ast = optimize_ir(ast, &opt_stats);
        
        if (opt_stats.constants_folded > 0 || opt_stats.dead_code_removed > 0) {
            printf("Optimizations applied:\n");
            printf("- Constants folded: %d\n", opt_stats.constants_folded);
            printf("- Dead code removed: %d\n", opt_stats.dead_code_removed);
        }

        // Generate assembly code
        AsmGenerator* gen = create_asm_generator(asm_output);
        generate_assembly(gen, ast);
        free_asm_generator(gen);

        // Cleanup
        free_ast(ast);
        free_parser(parser);
        free_lexer(lexer);
    }

    fclose(asm_output);
    return 0;
}