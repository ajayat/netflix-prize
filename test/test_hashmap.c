#include "unity.h"
#include "hashmap.h"

void setUp(void) {}
void tearDown(void) {}

void hashmap_running(void)
{
    Hashmap *h = hashmap_create(4);
    TEST_ASSERT_NOT_NULL(h);
    TEST_ASSERT_NOT_NULL(h->items);
    TEST_ASSERT_EQUAL(0, hashmap_get(h,10));
    hashmap_insert(h,1,2);
    hashmap_insert(h,5,10);
    TEST_ASSERT_EQUAL(2, hashmap_find(h,5));
    hashmap_insert(h,4,8);
    hashmap_insert(h,7,14);
    TEST_ASSERT_EQUAL(8,h->size);
    TEST_ASSERT_EQUAL(5, hashmap_find(h,5));
    TEST_ASSERT_EQUAL(10, hashmap_get(h,5));
    uint r = hashmap_remove(h,5);
    TEST_ASSERT_EQUAL(10,r);
    TEST_ASSERT_EQUAL(TOMBSTONE,h->items[5].key);
    hashmap_free(h);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(hashmap_running);
    return UNITY_END();
}