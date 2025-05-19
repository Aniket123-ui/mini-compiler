#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdbool.h>
typedef struct SymbolEntry {
    char* name;
    struct SymbolEntry* next;
} SymbolEntry;

typedef struct SymbolTable {
    struct SymbolTable* parent;
    SymbolEntry* symbols;
} SymbolTable;

extern SymbolTable* symbol_table;
void add_symbol(const char* name);

SymbolTable* create_symbol_table(SymbolTable* parent);
void free_symbol_table(SymbolTable* table);
bool insert_symbol(SymbolTable* table, const char* name);
bool lookup_symbol(SymbolTable* table, const char* name);

#endif
