# Mini Compiler

## Project Overview
A compact yet powerful compiler implementation written in C that demonstrates fundamental compiler construction concepts. This project implements a complete compilation pipeline including lexical analysis, parsing, semantic analysis, optimization, and code generation.

### Key Components
- Lexical Analyzer (Lexer)
- Parser with AST Generation
- Semantic Analyzer
- IR Optimizer
- Assembly Code Generator
- Test Framework

## Installation Instructions

### Prerequisites
- C Compiler (GCC recommended)
- Make build system
- Unix-like environment (or Windows with appropriate build tools)

### Build Steps
1. Clone the repository
2. Navigate to the project directory
3. Run make to build the project:
   ```bash
   make
   ```
   Or, on Windows (if Makefile is not available):
   ```powershell
   gcc src/main.c src/lexer/lexer.c src/parser/parser.c src/parser/ast.c src/semantic/semantic_analyzer.c src/codegen/asm_generator.c src/utils/symbol_table.c src/utils/error_handler.c -o mini_compiler.exe
   ```

## Usage

### Compiling a C-like Source File
To compile a C-like source file (e.g., `test.c`) to x86-64 NASM assembly:

```bash
./mini_compiler test.c
```
Or on Windows:
```powershell
mini_compiler.exe test.c
```
This will generate `output.asm` in the project directory.

### Assembling and Running the Output
To assemble and run the generated NASM assembly (on Windows):

```powershell
nasm -f win64 output.asm -o output.obj
gcc -nostartfiles -o output.exe output.obj -lkernel32 -lmsvcrt
./output.exe
```
On Unix-like systems, adjust the NASM and GCC flags as needed for your platform.

## Features
- Supports variable declarations, assignments, arithmetic, comparisons, if/else, while, return, and function definitions.
- Semantic analysis: declaration-before-use, type compatibility, scope, and return type checking.
- Generates valid x86-64 NASM assembly with RIP-relative addressing for all data references.
- Only global/static data is emitted in the `.data` section.
- Robust error handling and diagnostics.

## Example
Given the following `test.c`:
```c
int main() {
    int a = 5;
    int b = 3;
    return a + b;
}
```
Run:
```bash
./mini_compiler test.c
```
Then assemble and run as described above. The output should print:
```
Result: 8
```

## Testing
A test framework is included in `src/test/`. To run tests:
```bash
make test
```
Or run the test executables in the `test/` directory.

## License
MIT License