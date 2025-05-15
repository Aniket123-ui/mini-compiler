@echo off
echo Running Mini Compiler Test Suite
echo ==============================

gcc src/test/test_main.c src/test/test_framework.c src/test/test_lexer.c src/test/test_parser.c src/lexer/lexer.c src/parser/parser.c -o bin/test_runner.exe

if %ERRORLEVEL% NEQ 0 (
    echo Compilation failed!
    exit /b 1
)

echo.
echo Running tests...
echo.

bin\test_runner.exe

if %ERRORLEVEL% NEQ 0 (
    echo Some tests failed!
    exit /b 1
) else (
    echo All tests passed!
)