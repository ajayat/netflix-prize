#include "unity.h"
#include "utils.h"

#define EPOCH_YEAR 1900

void setUp(void) {}
void tearDown(void) {}

void test_days_from_epoch(void)
{
    TEST_ASSERT_EQUAL(0, days_from_epoch(EPOCH_YEAR, 1, 1));
    TEST_ASSERT_EQUAL(1, days_from_epoch(2023, 12, 1) - days_from_epoch(2023, 11, 30));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_days_from_epoch);
    return UNITY_END();
}