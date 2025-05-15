@echo off
echo ============================
echo Building Mini Compiler Project
echo ============================

REM Create build directory if missing
if not exist build (
    mkdir build
)

REM Compile all source files
gcc -Wall -g -I src ^
src/lexer/lexer.c ^
src/parser/parser.c ^
src/parser/ast.c ^
src/semantic/semantic_analyzer.c ^
src/utils/symbol_table.c ^
main.c ^
-o build/compiler.exe

if %ERRORLEVEL% NEQ 0 (
    echo ❌ Compilation failed!
    pause
    exit /b %ERRORLEVEL%
)

echo ✅ Compilation successful!
echo Running compiler...

build\compiler.exe

pause
