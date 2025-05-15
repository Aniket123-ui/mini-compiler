#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdbool.h>

// Symbol types
typedef enum {
    SYMBOL_VARIABLE,
    SYMBOL_FUNCTION
} SymbolType;

// Supported data types
typedef enum {
    DATA_TYPE_INT,
    DATA_TYPE_FLOAT,
    DATA_TYPE_VOID
} DataType;

// Symbol entry structure
typedef struct SymbolEntry {
    char* name;
    SymbolType symbol_type;
    DataType data_type;
    struct SymbolEntry* next;  // For handling hash collisions (simple linked list)
} SymbolEntry;

typedef struct {
    SymbolEntry** buckets;
    int size;
} SymbolTable;

// Symbol table functions
SymbolTable* create_symbol_table(int size);
bool insert_symbol(SymbolTable* table, const char* name, SymbolType type, DataType data_type);
SymbolEntry* lookup_symbol(SymbolTable* table, const char* name);
void free_symbol_table(SymbolTable* table);

#endif // SYMBOL_TABLE_H
