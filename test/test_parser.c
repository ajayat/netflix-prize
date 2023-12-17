#include <errno.h>
#include <unistd.h>
#include <string.h>

#include "unity.h"
#include "parser.h"

void setUp(void) {}
void tearDown(void) {}

/**
 * @brief Test about the ability to write correctly in the binary file.
 */
void test_write_to_file(void)
{
    char filepath[1024];
    if (getcwd(filepath, 1024) == NULL)
        perror("getcwd() error");
    strncat(filepath, "/data/data.bin", 17);

    FILE *file = fopen(filepath, "wb");
    TEST_ASSERT_EQUAL(0, errno);
    TEST_ASSERT_NOT_EQUAL(NULL, file);

    Data *data = parse();
    write_to_file(file, data);
    fclose(file);
    free_data(data); // Free memory
}

void test_read_from_file(void)
{
    char filepath[1024];
    if (getcwd(filepath, 1024) == NULL)
        perror("getcwd() error");
    strncat(filepath, "/data/data.bin", 17);

    FILE *file = fopen(filepath, "rb");
    TEST_ASSERT_EQUAL(0, errno);
    TEST_ASSERT_NOT_EQUAL(NULL, file);

    Data *data = read_from_file(file);
    TEST_ASSERT_NOT_EQUAL(NULL, data);
    TEST_ASSERT_EQUAL(17770, data->nb_movies);
    fclose(file);
    free_data(data); // Free memory
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_write_to_file);
    RUN_TEST(test_read_from_file);
    return UNITY_END();
}