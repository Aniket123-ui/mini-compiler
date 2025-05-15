#include "utils/symbol_table.h"
#include <stdio.h>

int main() {
    SymbolTable* table = create_symbol_table(10);

    insert_symbol(table, "x", SYMBOL_VARIABLE, DATA_TYPE_INT);
    insert_symbol(table, "y", SYMBOL_VARIABLE, DATA_TYPE_FLOAT);

    SymbolEntry* found = lookup_symbol(table, "x");
    if (found) {
        printf("Found symbol: %s (type: %d)\n", found->name, found->data_type);
    } else {
        printf("Symbol not found.\n");
    }

    free_symbol_table(table);
    return 0;
}
