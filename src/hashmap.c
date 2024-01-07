#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "hashmap.h"

const double LOAD = 0.75;

static uint hash(uint size, uint key)
{
    return key % size;
}

static uint jump(uint i)
{
    return (i * (i+1)) / 2;
}

Hashmap* hashmap_create(uint size)
{
    Hashmap* h = (Hashmap*) malloc(sizeof(Hashmap));
    h->size = 1 << (int)ceil(log2(size));
    h->count = 0;
    h->items = (Item*) calloc(h->size, sizeof(Item));
    return h;
}

void hashmap_free(Hashmap* h)
{
    free(h->items);
    free(h);
}

Hashmap* hashmap_resize(Hashmap* h, uint new_size)
{
    Hashmap *new_h = calloc(1, sizeof(Hashmap));
    new_h->size = new_size;
    new_h->count = 0;
    new_h->items = calloc(new_size, sizeof(Item));

    for (uint i=0; i<h->size; i++) {
        uint k = h->items[i].key;
        if (k != EMPTY && k != TOMBSTONE)
            hashmap_insert(new_h, k, h->items[i].value);
    }

    hashmap_free(h);
    return new_h;
}

int hashmap_find(Hashmap* h, uint key)
{
    uint index = hash(h->size, key);
    uint current_pos = index;
    uint current_key = h->items[index].key;
    uint i = 0;

    while (i < h->size && current_key != EMPTY && current_key != key) {
        current_pos = (index + jump(++i)) % h->size;
        current_key = h->items[index].key;
    }
    if (current_key == EMPTY || i >= h->size)
        return -1; // Key not found
    return current_pos;
}

uint hashmap_get(Hashmap* h, uint key)
{
    int pos = hashmap_find(h, key);
    if (pos == -1)
        return 0; // Key not found
    return h->items[pos].value;
}

void hashmap_insert(Hashmap* h, uint key, uint value)
{
    uint index = hash(h->size, key);
    uint current_pos = index;
    uint current_key = h->items[index].key;
    uint i = 0;
    int id_tombstone = -1;

    if ((double)h->count / (double)h->size >= LOAD) // the load is too high
        h = hashmap_resize(h, 2*h->size);

    while (current_key != EMPTY && current_key != key) {
        if (current_key == TOMBSTONE && id_tombstone < 0)
            id_tombstone = current_pos;
        current_pos = (index + jump(++i)) % h->size;
        current_key = h->items[current_pos].key;
    }

    if (current_key == EMPTY) { // The key doesn't exist.
        if (id_tombstone >= 0) { // Replace the tombstone.
            h->items[id_tombstone].key = key;
            h->items[id_tombstone].value = value;
        }
        else { // Full the empty case.
            h->items[current_pos].key = key;
            h->items[current_pos].value = value;
        }
        h->count++;
    }
    // Otherwise, the hashmap already contains the key.
}

uint hashmap_remove(Hashmap* h, uint key)
{
    int pos = hashmap_find(h, key);
    if (pos < 0)
        return 0;
    uint value = h->items[pos].value;
    h->items[pos].key = TOMBSTONE;
    h->items[pos].value = 0;
    return value;
}