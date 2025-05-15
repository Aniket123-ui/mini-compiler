#include "symbol_table.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define HASH_MULTIPLIER 31

// Simple hash function
static int hash(const char* str, int size) {
    int hash_val = 0;
    while (*str) {
        hash_val = (hash_val * HASH_MULTIPLIER + *str) % size;
        str++;
    }
    return hash_val;
}

SymbolTable* create_symbol_table(int size) {
    SymbolTable* table = malloc(sizeof(SymbolTable));
    table->buckets = calloc(size, sizeof(SymbolEntry*));
    table->size = size;
    return table;
}

bool insert_symbol(SymbolTable* table, const char* name, SymbolType type, DataType data_type) {
    int index = hash(name, table->size);

    SymbolEntry* current = table->buckets[index];
    while (current) {
        if (strcmp(current->name, name) == 0) {
            return false;  // Symbol already exists
        }
        current = current->next;
    }

    SymbolEntry* new_entry = malloc(sizeof(SymbolEntry));
    new_entry->name = strdup(name);
    new_entry->symbol_type = type;
    new_entry->data_type = data_type;
    new_entry->next = table->buckets[index];
    table->buckets[index] = new_entry;
    return true;
}

SymbolEntry* lookup_symbol(SymbolTable* table, const char* name) {
    int index = hash(name, table->size);
    SymbolEntry* entry = table->buckets[index];
    while (entry) {
        if (strcmp(entry->name, name) == 0) {
            return entry;
        }
        entry = entry->next;
    }
    return NULL;
}

void free_symbol_table(SymbolTable* table) {
    for (int i = 0; i < table->size; i++) {
        SymbolEntry* entry = table->buckets[i];
        while (entry) {
            SymbolEntry* temp = entry;
            entry = entry->next;
            free(temp->name);
            free(temp);
        }
    }
    free(table->buckets);
    free(table);
}
