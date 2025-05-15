#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "../types/data_types.h"
#include <stdbool.h>

// Define symbol types
typedef enum {
    SYMBOL_VARIABLE,
    SYMBOL_FUNCTION,
    SYMBOL_PARAMETER,
    SYMBOL_TYPE
} SymbolType;

typedef struct SymbolEntry {
    char* name;
    DataType type;
    SymbolType symbol_type;  // Added this field
    int scope_level;
    struct SymbolEntry* next;
} SymbolEntry;

typedef struct SymbolTable {
    SymbolEntry** entries;
    int size;
    int scope_level;
    struct SymbolTable* parent;
} SymbolTable;

// Symbol table operations
SymbolTable* create_symbol_table(int size);
void free_symbol_table(SymbolTable* table);
bool insert_symbol(SymbolTable* table, const char* name, SymbolType sym_type, DataType data_type);
SymbolEntry* lookup_symbol(SymbolTable* table, const char* name);
void enter_scope(SymbolTable* table);
void exit_scope(SymbolTable* table);

#endif