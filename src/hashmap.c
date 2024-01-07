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
    h->items = (Item*) calloc(h->size, sizeof(Item));
    return h;
}

void hashmap_free(Hashmap* h)
{
    free(h->items);
    free(h);
}

void hashmap_resize(Hashmap* h)
{
    h->size *= 2;
    Item* new_tab = calloc(h->size, sizeof(Item));
    for (uint i=0; i<(h->size/2); i++) {
        if (h->items[i].key > 0) {
            uint index = hash_function(h->size, h->items[i].key);
            uint jump = 0;
            while (new_tab[index].key > 0) {
                jump++;
                index += jump;
            }
            new_tab[index].key = h->items[i].key;
            new_tab[index].value = h->items[i].value;
        }
    }
    free(h->items);
    h->items = new_tab;
}

uint hashmap_find(Hashmap* h, uint key)
{
    uint index = hash_function(h->size, key);
    uint current = h->items[index].key;
    uint jump = 0;

    while (current > 0 && current != key) {
        jump++;
        index += jump;
        if (index >= h->size)
            return UINT32_MAX; // Key not found
        current = h->items[index].key;
    }
    if (current == 0)
        return UINT32_MAX; // Key not found
    return index;
}

uint hashmap_insert(Hashmap* h, uint key, uint16_t value)
{
    uint index = hash_function(h->size, key);
    uint current = h->items[index].key;
    uint jump = 0;

    if (h->count/h->size > 0.5) // the load is too high
        hashmap_resize(h);

    while (current > 0 && current != key) {
        jump++;
        index += jump;
        if (index >= h->size)
            hashmap_resize(h);
        current = h->items[index].key;
    }

    if (current == 0) { // the key doesn't exist
        h->items[index].key = key;
        h->items[index].value = value;
        h->count++;
    }

    // Otherwise, the hashmap already contains the key.

    return value;
}

uint hashmap_remove(Hashmap* h, uint key)
{
    uint hash = hash_function(h->size, key);
    uint index = hash;
    uint current = h->items[index].key;
    uint jump = 0;

    while (current > 0 && current != key) {
        jump++;
        index += jump;
        if (index >= h->size)
            return UINT32_MAX; // Key not found
        current = h->items[index].key;
    }

    if (current == 0)
        return UINT32_MAX; // Key not found

    h->items[index].key = 0;
    h->items[index].value = 0;
    jump++;
    uint last_index = index;
    uint test_index = index+jump;
    uint test_key = h->items[test_index].key;

    while (test_index < h->size && (test_key == 0 || hash_function(h->size, test_key) == hash)) {
        if (test_key != 0)
            last_index += test_index;
        jump++;
        test_index += jump;
        test_key = h->items[test_index].key;
    }

    h->items[index].key = h->items[last_index].key;
    h->items[index].value = h->items[last_index].value;
    h->items[last_index].key = 0;
    h->items[last_index].value = 0;

    return key;
}