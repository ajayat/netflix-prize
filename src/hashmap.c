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
    for (uint i = 0; i<h->size; i++) {
        if (h->items[i] != NULL)
            free(h->items[i]);
    }
    free(h->items);
    free(h);
}

void hashmap_resize(Hashmap* h)
{
    h->size *= 2;
    Item** new_tab = calloc(h->size, sizeof(Item*));
    for (uint i=0; i<(h->size/2); i++) {
        if (h->items[i] != NULL) {
            uint index = hash_function(h->size, h->items[i]->key);
            uint jump = 0;
            while (new_tab[index] != NULL) {
                jump++;
                index += jump;
            }
            new_tab[index] = h->items[i];
        }
    }
    free(h->items);
    h->items = new_tab;
}

uint hashmap_find(Hashmap* h, uint key)
{
    uint index = hash_function(h->size, key);
    Item* current = h->items[index];
    uint jump = 0;
    while (current != NULL && current->key != key) {
        jump++;
        index += jump;
        if (index >= h->size)
            return UINT32_MAX; // Key not found
        current = h->items[index];
    }
    if (current == NULL)
        return UINT32_MAX; // Key not found
    return index;
}

uint hashmap_insert(Hashmap* h, uint key, uint value)
{
    uint index = hash_function(h->size, key);
    Item* current = h->items[index];
    uint jump = 0;

    if (h->count/h->size > 0.5) // the load is too high
        hashmap_resize(h);

    while (current != NULL && current->key != key) {
        jump++;
        index += jump;
        if (index >= h->size)
            hashmap_resize(h);
        current = h->items[index];
    }

    if (current == NULL) { // the key doesn't exist
        h->items[index] = create_item(key, value);
        h->count++;
    }

    // Otherwise, the hashmap already contains the key.

    return value;
}

uint hashmap_remove(Hashmap* h, uint key)
{
    // ! À COMPLÉTER
    return key;
}