# Efficient Mini Compiler for Code Optimization and Syntax Analysis

A lightweight compiler implementation that supports a subset of C language features with optimization capabilities.

## Features
- Lexical analysis (tokenization)
- Syntax analysis (parsing)
- Intermediate code generation (Three Address Code)
- Basic optimizations (constant folding, dead code elimination)
- Error handling with line number reporting

## Supported Language Features
- Variable declarations
- Arithmetic expressions
- Assignment statements
- Conditional statements (if-else)
- Loops (while)
- Basic data types (int)

## Prerequisites
- GCC compiler
- Flex (lexical analyzer generator)
- Bison (parser generator)
- Make

## Building the Project
```bash
make