#pragma once

#include <stdint.h>

typedef struct Item {
    int *key;
    uint16_t *value;
} Item;

typedef struct HashTable {
    Item** items;
    int size;
    int count;
} HashTable;
