#pragma once

#include <stdint.h>

typedef struct Item {
    uint key;
    uint value;
} Item;

typedef struct Hashmap {
    Item** items;
    uint size;
    uint count;
} Hashmap;
