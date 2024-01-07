#pragma once

#include <stdint.h>

typedef unsigned int uint;

typedef struct Item {
    uint key;
    uint16_t value;
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
 */
void hashmap_resize(Hashmap* h);

/**
 * @brief Find the index in the hashmap of the given key.
 * 
 * @param h  The given hashmap.
 * @param key The key we are looking for.
 * @return uint The finden key.
 */
uint hashmap_find(Hashmap* h, uint key);

/**
 * @brief Insert a new element in the hashmap, if it doesn't already exist.
 * 
 * @param h The given hashmap.
 * @param key The key to add.
 * @param value The associated value.
 * @return uint The value.
 */
uint hashmap_insert(Hashmap* h, uint key, uint16_t value);

/**
 * @brief Remove the item with the corresponding key from the hashmap.
 * 
 * @param h The given hashmap.
 * @param key The key to remove.
 * @return uint The removed key.
 */
uint hashmap_remove(Hashmap* h, uint key);