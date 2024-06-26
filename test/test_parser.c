#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "unity.h"
#include "parser.h"
#include "utils.h"

void setUp(void) {}
void tearDown(void) {}

/**
 * @brief Test about the ability to write correctly in the binary file.
 */
void test_write_movie_data_to_file(void)
{
    char *filepath = get_filepath("data", "data.bin");
    MovieData *data = parse();
    write_movie_data_to_file(filepath, data);
    free(filepath);
    TEST_ASSERT_EQUAL(0, errno);
    free_movie_data(data); // Free memory
}

void test_read_movie_data_from_file(void)
{
    char *filepath = get_filepath("data", "data.bin");
    MovieData *data = read_movie_data_from_file(filepath);
    free(filepath);
    TEST_ASSERT_EQUAL(0, errno);
    TEST_ASSERT_NOT_EQUAL(NULL, data);
    TEST_ASSERT_EQUAL(17770, data->nb_movies);
    free_movie_data(data); // Free memory
}

void test_write_user_data_to_file(void)
{
    char *mv_filepath = get_filepath("data", "data.bin");
    char *user_filepath = get_filepath("data", "data.bin.test");

    MovieData *data = read_movie_data_from_file(mv_filepath);
    free(mv_filepath);
    TEST_ASSERT_EQUAL(0, errno);

    UserData *user_data = to_user_oriented(data);
    write_user_data_to_file(user_filepath, user_data);
    free(user_filepath);
    TEST_ASSERT_EQUAL(0, errno);

    free_user_data(user_data); // Free memory
    free_movie_data(data); // Free memory
}

void test_read_user_data_from_file(void)
{
    char *filepath = get_filepath("data", "data.bin.test");

    UserData *data = read_user_data_from_file(filepath);
    free(filepath);
    TEST_ASSERT_NOT_EQUAL(NULL, data);
    TEST_ASSERT_EQUAL(480189, data->nb_users);
    free_user_data(data); // Free memory
    remove("data/data.bin.test");
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_write_movie_data_to_file);
    RUN_TEST(test_read_movie_data_from_file);
    RUN_TEST(test_write_user_data_to_file);
    RUN_TEST(test_read_user_data_from_file);
    return UNITY_END();
}