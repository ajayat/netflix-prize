#include "unity.h"
#include "parser.h"

void setUp(void) {}
void tearDown(void) {}

void test_parse(void)
{
    TEST_ASSERT_EQUAL(0, parse("unused input"));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_parse);
    return UNITY_END();
}