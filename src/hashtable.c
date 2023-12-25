#include <stdlib.h>
#include "hashtable.h"

HashTable* create_table(int size)
{
    HashTable* table = (HashTable*) malloc(sizeof(HashTable));
    table->size = size;
    table->count = 0;
    table->items = (Item**) calloc(table->size, sizeof(Item*));

    for (int i = 0; i < table->size; i++)
        table->items[i] = NULL;

    return table;
}