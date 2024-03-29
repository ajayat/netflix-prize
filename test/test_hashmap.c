#include "unity.h"
#include "hashmap.h"

void setUp(void) {}
void tearDown(void) {}

void test_hashmap_create(void)
{
    Hashmap *h = hashmap_create(4);
    TEST_ASSERT_NOT_NULL(h);
    TEST_ASSERT_NOT_NULL(h->items);
    hashmap_free(h);
}

void test_hashmap_insert(void)
{
    Hashmap *h = hashmap_create(4);
    TEST_ASSERT_EQUAL(0, h->items[1].key);
    TEST_ASSERT_EQUAL(0, h->items[1].value);
    hashmap_insert(h,1,2);
    TEST_ASSERT_EQUAL(1, h->items[1].key);
    TEST_ASSERT_EQUAL(2, h->items[1].value);
    hashmap_insert(h,1,2);
    hashmap_insert(h,1,2);
    hashmap_insert(h,1,2);
    TEST_ASSERT_EQUAL(4, h->size);
    hashmap_free(h);
}

void test_hashmap_find(void)
{
    Hashmap *h = hashmap_create(4);
    TEST_ASSERT_EQUAL(-1, hashmap_find(h,5));
    hashmap_insert(h,1,2);
    hashmap_insert(h,5,10);
    TEST_ASSERT_EQUAL(2, hashmap_find(h,5));
    hashmap_free(h);
}

void test_hashmap_get(void)
{
    Hashmap *h = hashmap_create(4);
    TEST_ASSERT_EQUAL(0, hashmap_get(h,1));
    hashmap_insert(h,1,2);
    TEST_ASSERT_EQUAL(2, hashmap_get(h,1));
    hashmap_free(h);
}

void test_hashmap_increase(void)
{
    Hashmap *h = hashmap_create(4);
    hashmap_insert(h,1,2);
    hashmap_increase(h,1,4);
    TEST_ASSERT_EQUAL(6,h->items[1].value);
    hashmap_increase(h,3,6);
    TEST_ASSERT_EQUAL(6,h->items[3].value);
    hashmap_free(h);
}

void test_hashmap_resize(void)
{
    Hashmap *h = hashmap_create(4);
    hashmap_insert(h,1,2);
    hashmap_insert(h,5,10);
    hashmap_insert(h,4,8);
    hashmap_insert(h,7,14);
    TEST_ASSERT_EQUAL(8,h->size);
    TEST_ASSERT_EQUAL(5, hashmap_find(h,5));
    TEST_ASSERT_EQUAL(10, hashmap_get(h,5));
    hashmap_free(h);
}

void test_hashmap_remove(void)
{
    Hashmap *h = hashmap_create(4);
    hashmap_insert(h,1,2);
    hashmap_insert(h,5,10);
    hashmap_insert(h,4,8);
    hashmap_insert(h,7,14);
    uint r = hashmap_remove(h,5);
    TEST_ASSERT_EQUAL(10,r);
    TEST_ASSERT_EQUAL(TOMBSTONE,h->items[5].key);
    hashmap_free(h);
}


int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_hashmap_create);
    RUN_TEST(test_hashmap_insert);
    RUN_TEST(test_hashmap_find);
    RUN_TEST(test_hashmap_get);
    RUN_TEST(test_hashmap_increase);
    RUN_TEST(test_hashmap_resize);
    RUN_TEST(test_hashmap_remove);
    return UNITY_END();
}