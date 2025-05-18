#include <stdio.h>
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "semantic/semantic_analyzer.h"
#include "parser/ast.h"
#include "ir/ir_generator.h" // Include the IR generator header
#include "codegen/asm_generator.h"

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

    ASTNode* ast_root = parse(parser);
    if (!ast_root) {
        fprintf(stderr, "Parsing failed.\n");
        free_parser(parser);
        free_lexer(lexer);
        return 1;
    }

    // Remove debug and IR output to stdout, only emit assembly
    analyze_semantics(ast_root);
    if (get_semantic_error()) {
        fprintf(stderr, "[ERROR] Semantic errors detected. Aborting code generation.\n");
        free_ast(ast_root);
        free_parser(parser);
        free_lexer(lexer);
        return 1;
    }
    // Write assembly directly to output.asm
    FILE* asm_file = fopen("output.asm", "w");
    if (asm_file) {
        AsmGenerator* gen = create_asm_generator(asm_file);
        fprintf(asm_file, "section .data\n");
        fprintf(asm_file, "x dq 0\n");
        fprintf(asm_file, "y dq 0\n");
        fprintf(asm_file, "z dq 0\n");
        fprintf(asm_file, "fmt db 'Result: %%lld', 10, 0\n");
        fprintf(asm_file, "section .text\n");
        fprintf(asm_file, "global main\n");
        fprintf(asm_file, "extern printf\n");
        fprintf(asm_file, "main:\n");
        generate_assembly(gen, ast_root);
        // Print z at the end (Windows x64: RCX = format, RDX = z)
        fprintf(asm_file, "    mov rdx, [z]\n");
        fprintf(asm_file, "    lea rcx, [fmt]\n");
        fprintf(asm_file, "    call printf\n");
        fprintf(asm_file, "    ret\n");
        free_asm_generator(gen);
        fclose(asm_file);
    } else {
        fprintf(stderr, "[ERROR] Could not open output.asm for writing.\n");
        perror("fopen failed");
    }

    free_ast(ast_root);
    free_parser(parser);
    free_lexer(lexer);

    return 0;
}
