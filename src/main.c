#include <stdio.h>
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "semantic/semantic_analyzer.h"
#include "parser/ast.h"
#include "utils/symbol_table.h"
#include "ir/ir_generator.h" // Include the IR generator header
#include "codegen/asm_generator.h"
#include<stdlib.h>
// For analyze_semantics

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    Lexer* lexer = create_lexer_from_file(argv[1]);
    if (!lexer) {
        fprintf(stderr, "Failed to open input file.\n");
        return 1;
    }

    Parser* parser = create_parser(lexer);
    if (!parser) {
        fprintf(stderr, "Failed to create parser.\n");
        free_lexer(lexer);
        return 1;
    }

    // Parse the program (use parse_program instead of parse)
    ASTNode* root = parse_program(parser);
    if (!root) {
        fprintf(stderr, "Parsing failed.\n");
        free_parser(parser);
        return 1;
    }

    // Remove debug and IR output to stdout, only emit assembly
    analyze_semantics(root);
    if (get_semantic_error()) {
        fprintf(stderr, "[ERROR] Semantic errors detected. Aborting code generation.\n");
        free_ast(root);
        free_parser(parser);
        free_lexer(lexer);
        return 1;
    }
    // Write assembly directly to output.asm
    FILE* asm_file = fopen("output.asm", "w");
    if (asm_file) {
        AsmGenerator* gen = create_asm_generator(asm_file);
        fprintf(asm_file, "section .data\n");
        fprintf(asm_file, "__return_value dq 0\n");
        fprintf(asm_file, "fmt db 'Result: %%lld', 10, 0\n");
        SymbolEntry* current = symbol_table ? symbol_table->symbols : NULL;
        while (current) {
            fprintf(asm_file, "%s dq 0\n", current->name);
            current = current->next;
        }

        // Do NOT emit a dq 0 for local variables a, b, etc.
        fprintf(asm_file, "section .text\n");
        fprintf(asm_file, "global main\n");
        fprintf(asm_file, "extern printf\n");
        fprintf(asm_file, "main:\n");
        generate_assembly(gen, root);
        // Print __return_value at the end if assigned
        if (gen->has_return_value) {
            fprintf(asm_file, "    mov rdx, qword [rel __return_value]\n");
            fprintf(asm_file, "    lea rcx, [rel fmt]\n");
            fprintf(asm_file, "    call printf\n");
        }
        fprintf(asm_file, "    ret\n");
        free_asm_generator(gen);
        fclose(asm_file);
    } else {
        fprintf(stderr, "[ERROR] Could not open output.asm for writing.\n");
        perror("fopen failed");
    }

    free_ast(root);
    free_parser(parser);
    free_lexer(lexer);
    if (symbol_table) free_symbol_table(symbol_table);
    return 0;
}
