#include "symbol_table.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

SymbolTable* create_symbol_table(SymbolTable* parent) {
    SymbolTable* table = malloc(sizeof(SymbolTable));
    table->parent = parent;
    table->symbols = NULL;
    return table;
}

bool insert_symbol(SymbolTable* table, const char* name) {
    SymbolEntry* entry = malloc(sizeof(SymbolEntry));
    entry->name = strdup(name);
    entry->next = table->symbols;
    table->symbols = entry;
    return true;
}

bool lookup_symbol(SymbolTable* table, const char* name) {
    for (SymbolTable* t = table; t != NULL; t = t->parent) {
        for (SymbolEntry* e = t->symbols; e != NULL; e = e->next) {
            if (strcmp(e->name, name) == 0) {
                return true;
            }
        }
    }
    return false;
}

void free_symbol_table(SymbolTable* table) {
    SymbolEntry* current = table->symbols;
    while (current) {
        SymbolEntry* next = current->next;
        free(current->name);
        free(current);
        current = next;
    }
    free(table);
}
