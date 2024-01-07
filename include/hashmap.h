#pragma once

#include <stdint.h>

typedef unsigned int uint;

const uint EMPTY = 0;
const uint TOMBSTONE = UINT32_MAX;

typedef struct Item {
    uint key;
    uint value;
} Item;

typedef struct Hashmap {
    Item* items;
    uint size;
    uint count;
} Hashmap;

/**
 * @brief Create a hashmap with an initial size.
 * 
 * @param size Initial size of the hashmap.
 * @return Hashmap* A pointer to the created hashmap.
 */
Hashmap* hashmap_create(uint size);

/**
 * @brief Free the hashmap.
 * 
 * @param h The hashmap to free.
 */
void hashmap_free(Hashmap* h);

/**
 * @brief Resize the given hashmap by 2.
 * 
 * @param h The hashmap to resize.
 * @param new_size New size for the hashmap.
 * @return Hashmap* The hashmap 
 */
Hashmap* hashmap_resize(Hashmap* h, uint new_size);


/**
 * @brief Find the index of a given key in the hashmap.
 * 
 * @param h The given hashmap.
 * @param key The key to find.
 * @return int -1 if the key doesn't exist, its index otherwise.
 */
int hashmap_find(Hashmap* h, uint key);

/**
 * @brief Find the value associated to he given key in the hashmap.
 * 
 * @param h  The given hashmap.
 * @param key The key we are looking for.
 * @return uint The value associated to the key.
 */
uint hashmap_get(Hashmap* h, uint key);

/**
 * @brief Insert a new element in the hashmap, if it doesn't already exist.
 * 
 * @param h The given hashmap.
 * @param key The key to add.
 * @param value The associated value.
 */
void hashmap_insert(Hashmap* h, uint key, uint value);

/**
 * @brief Remove the item with the corresponding key from the hashmap.
 * 
 * @param h The given hashmap.
 * @param key The key to remove.
 * @return uint The removed key.
 */
uint hashmap_remove(Hashmap* h, uint key);