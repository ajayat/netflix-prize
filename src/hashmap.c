#include <stdio.h>
#include <stdlib.h>

#include "hashmap.h"

static uint hash_function(int size, uint key)
{
    return (key % size);
}

Hashmap* hashmap_create(uint size)
{
    Hashmap* h = (Hashmap*) malloc(sizeof(Hashmap));
    h->size = size;
    h->count = 0;
    h->items = (Item**) calloc(h->size, sizeof(Item*));

    for (uint i = 0; i < h->size; i++)
        h->items[i] = NULL;

    return h;
}

Item* create_item(uint key, uint value)
{
    Item* i = (Item*) malloc(sizeof(Item));
    i->key = key;
    i->value = value;
    return i;
}

void hashmap_free(Hashmap* h)
{
    for (uint i = 0; i < h->size; i++) {
        if (h->items[i] != NULL)
            free(h->items[i]);
    }
    free(h->items);
    free(h);
}

uint hashmap_insert(Hashmap* h, uint key, uint value)
{
    Item* i = create_item(key, value);
    uint index = hash_function(h->size, key);
    Item* current = h->items[index];
    uint jump = 0;

    if (h->count/h->size > 0.5) // the load is too high
        goto resize;

    while (current != NULL && current->key != key) {
        jump++;
        index += jump;
        if (index >= h->size)
            goto resize;
        current = h->items[index];
    }

    if (current == NULL) { // the key doesn't exist
        h->items[index] = i;
        h->count++;
    }

    // Otherwise, the hashmap already contains the key.

    return value;

resize:
    h->size *= 2;
    h->items = realloc(h->items, h->size*sizeof(Item*));
    if (h->items == NULL)
        printf("Reallocation  of the hashmap failed.");
}