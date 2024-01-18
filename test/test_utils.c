#include <stdlib.h>

#include "unity.h"
#include "utils.h"

#define EPOCH_YEAR 1998

void setUp(void) {}
void tearDown(void) {}

void test_days_from_epoch(void)
{
    TEST_ASSERT_EQUAL(0, days_from_epoch(EPOCH_YEAR, 1, 1));
    TEST_ASSERT_EQUAL(1, days_from_epoch(2000, 1, 1) - days_from_epoch(1999, 12, 31));
}

void test_dichotomic_search(void)
{
    char *array[] = {"a", "b", "c", "d", "e", "f"};
    TEST_ASSERT_EQUAL(0, dichotomic_search(array, 6, "a"));
    TEST_ASSERT_EQUAL(1, dichotomic_search(array, 6, "b"));
    TEST_ASSERT_EQUAL(2, dichotomic_search(array, 6, "c"));
    TEST_ASSERT_EQUAL(3, dichotomic_search(array, 6, "d"));
    TEST_ASSERT_EQUAL(4, dichotomic_search(array, 6, "e"));
    TEST_ASSERT_EQUAL(5, dichotomic_search(array, 6, "f"));
    TEST_ASSERT_EQUAL(-1, dichotomic_search(array, 6, "g"));
}

void test_strdup(void)
{
    char *str = "Hello World!";
    char *dup = strdup(str);
    TEST_ASSERT_EQUAL_STRING(str, dup);
    free(dup);
}

void test_get_similarity_matrix(void)
{
    float sim[] = {0.1F, 0.2F, 0.3F, 0.4F, 0.5F, 0.6F};
    TEST_ASSERT_EQUAL_FLOAT(0.1, get_similarity(sim, 0, 1));
    TEST_ASSERT_EQUAL_FLOAT(0.1, get_similarity(sim, 1, 0));
    TEST_ASSERT_EQUAL_FLOAT(0.2, get_similarity(sim, 2, 0));
    TEST_ASSERT_EQUAL_FLOAT(0.3, get_similarity(sim, 2, 1));
    TEST_ASSERT_EQUAL_FLOAT(0.4, get_similarity(sim, 3, 0));
    TEST_ASSERT_EQUAL_FLOAT(0.5, get_similarity(sim, 3, 1));
    TEST_ASSERT_EQUAL_FLOAT(0.6, get_similarity(sim, 3, 2));
    TEST_ASSERT_EQUAL_FLOAT(0.6, get_similarity(sim, 2, 3));
    TEST_ASSERT_EQUAL_FLOAT(0.0, get_similarity(sim, 3, 3));
    TEST_ASSERT_EQUAL_FLOAT(0.0, get_similarity(sim, 0, 0));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_days_from_epoch);
    RUN_TEST(test_dichotomic_search);
    RUN_TEST(test_strdup);
    RUN_TEST(test_get_similarity_matrix);
    return UNITY_END();
}