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
void test_write_movie_data_to_file(void)
{
    char filepath[1024];
    if (getcwd(filepath, 1024) == NULL)
        perror("getcwd() error");
    strncat(filepath, "/data/movie_data.bin", 21);

    FILE *file = fopen(filepath, "wb");
    TEST_ASSERT_EQUAL(0, errno);
    TEST_ASSERT_NOT_EQUAL(NULL, file);

    MovieData *data = parse();
    write_movie_data_to_file(file, data);
    fclose(file);
    free_movie_data(data); // Free memory
}

void test_read_movie_data_from_file(void)
{
    char filepath[1024];
    if (getcwd(filepath, 1024) == NULL)
        perror("getcwd() error");
    strncat(filepath, "/data/movie_data.bin", 21);

    FILE *file = fopen(filepath, "rb");
    TEST_ASSERT_EQUAL(0, errno);
    TEST_ASSERT_NOT_EQUAL(NULL, file);

    MovieData *data = read_movie_data_from_file(file);
    TEST_ASSERT_NOT_EQUAL(NULL, data);
    TEST_ASSERT_EQUAL(17770, data->nb_movies);
    fclose(file);
    free_movie_data(data); // Free memory
}

void test_write_user_data_to_file(void)
{
    char user_filepath[1024];
    char mv_filepath[1024];
    if (getcwd(user_filepath, 1024) == NULL ||
        getcwd(mv_filepath, 1024) == NULL)
        perror("getcwd() error");
    strncat(user_filepath, "/data/user_data.bin", 20);
    strncat(mv_filepath, "/data/movie_data.bin", 21);

    FILE *mv_file = fopen(mv_filepath, "rb");
    MovieData *data = read_movie_data_from_file(mv_file);
    fclose(mv_file);

    FILE *user_file = fopen(user_filepath, "wb");
    TEST_ASSERT_EQUAL(0, errno);
    TEST_ASSERT_NOT_EQUAL(NULL, user_file);

    UserData *user_data = to_user_oriented(data);
    write_user_data_to_file(user_file, user_data);
    fclose(user_file);
    free_user_data(user_data); // Free memory
    free_movie_data(data); // Free memory
}

void test_read_user_data_from_file(void)
{
    char filepath[1024];
    if (getcwd(filepath, 1024) == NULL)
        perror("getcwd() error");
    strncat(filepath, "/data/user_data.bin", 20);

    FILE *file = fopen(filepath, "rb");
    TEST_ASSERT_EQUAL(0, errno);
    TEST_ASSERT_NOT_EQUAL(NULL, file);

    UserData *data = read_user_data_from_file(file);
    TEST_ASSERT_NOT_EQUAL(NULL, data);
    TEST_ASSERT_EQUAL(480189, data->nb_users);
    fclose(file);
    free_user_data(data); // Free memory
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