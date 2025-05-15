#include "symbol_table.h"
#include "error_handler.h"
#include <string.h>
#include <stdlib.h>

// Hash function for symbol names
static unsigned int hash(const char* name, int table_size) {
    unsigned int hash = 0;
    while (*name) {
        hash = (hash * 31 + *name) % table_size;
        name++;
    }
    return hash;
}

SymbolTable* create_symbol_table(int size) {
    SymbolTable* table = (SymbolTable*)malloc(sizeof(SymbolTable));
    table->entries = (SymbolEntry**)calloc(size, sizeof(SymbolEntry*));
    table->size = size;
    table->scope_level = 0;
    table->parent = NULL;
    return table;
}

void free_symbol_table(SymbolTable* table) {
    if (!table) return;

    // Free all entries in the current table
    for (int i = 0; i < table->size; i++) {
        SymbolEntry* entry = table->entries[i];
        while (entry) {
            SymbolEntry* next = entry->next;
            free(entry->name);
            free(entry);
            entry = next;
        }
    }

    // Free the entries array and table itself
    free(table->entries);
    free(table);
}

bool insert_symbol(SymbolTable* table, const char* name, SymbolType sym_type, DataType data_type) {
    // Check for redefinition in current scope
    SymbolEntry* existing = lookup_symbol(table, name);
    if (existing && existing->scope_level == table->scope_level) {
        report_error(ERROR_REDEFINITION, 0, 0, 
                    "Symbol '%s' already defined in current scope", name);
        return false;
    }

    // Create new entry
    SymbolEntry* entry = (SymbolEntry*)malloc(sizeof(SymbolEntry));
    entry->name = strdup(name);
    entry->symbol_type = sym_type;
    entry->type = data_type;
    entry->scope_level = table->scope_level;
    
    // Insert into hash table
    unsigned int index = hash(name, table->size);
    entry->next = table->entries[index];
    table->entries[index] = entry;
    
    return true;
}

SymbolEntry* lookup_symbol(SymbolTable* table, const char* name) {
    SymbolTable* current = table;
    
    while (current) {
        unsigned int index = hash(name, current->size);
        SymbolEntry* entry = current->entries[index];
        
        while (entry) {
            if (strcmp(entry->name, name) == 0) {
                return entry;
            }
            entry = entry->next;
        }
        
        current = current->parent;
    }
    
    return NULL;
}

void enter_scope(SymbolTable* table) {
    SymbolTable* new_scope = create_symbol_table(table->size);
    new_scope->scope_level = table->scope_level + 1;
    new_scope->parent = table;
    table = new_scope;
}

void exit_scope(SymbolTable* table) {
    if (table->parent) {
        SymbolTable* parent = table->parent;
        free_symbol_table(table);
        table = parent;
    }
}