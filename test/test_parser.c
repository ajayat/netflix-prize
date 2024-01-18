#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "unity.h"
#include "parser.h"

void setUp(void) {}
void tearDown(void) {}

static char *get_filepath(char *filename)
{
    char *filepath = malloc(1024 * sizeof(char));
    if (getcwd(filepath, 512) == NULL)
        perror("getcwd() error");
    strncat(filepath, filename, 512);
    return filepath;
}

/**
 * @brief Test about the ability to write correctly in the binary file.
 */
void test_write_movie_data_to_file(void)
{
    char *filepath = get_filepath("/data/movie_data.bin");
    MovieData *data = parse();
    write_movie_data_to_file(filepath, data);
    free(filepath);
    TEST_ASSERT_EQUAL(0, errno);
    free_movie_data(data); // Free memory
}

void test_read_movie_data_from_file(void)
{
    char *filepath = get_filepath("/data/movie_data.bin");
    MovieData *data = read_movie_data_from_file(filepath);
    free(filepath);
    TEST_ASSERT_EQUAL(0, errno);
    TEST_ASSERT_NOT_EQUAL(NULL, data);
    TEST_ASSERT_EQUAL(17770, data->nb_movies);
    free_movie_data(data); // Free memory
}

void test_write_user_data_to_file(void)
{
    char *mv_filepath = get_filepath("/data/movie_data.bin");
    char *user_filepath = get_filepath("/data/user_data.bin");

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
    char *filepath = get_filepath("/data/user_data.bin");

    UserData *data = read_user_data_from_file(filepath);
    free(filepath);
    TEST_ASSERT_NOT_EQUAL(NULL, data);
    TEST_ASSERT_EQUAL(480189, data->nb_users);
    free_user_data(data); // Free memory
}

void test_parse_probe(void)
{
    char *mv_filepath = get_filepath("/data/movie_data.bin");

    MovieData *data = read_movie_data_from_file(mv_filepath);
    free(mv_filepath);
    TEST_ASSERT_EQUAL(0, errno);

    parse_probe("data/probe.txt", data);

    FILE* probe_file = fopen("data/probe.txt", "r");
    FILE* answers_file = fopen("data/probe_answers.txt", "r");
    uint idm1, idm2, score1, score2;
    ulong idu1, idu2;
    if (fscanf(probe_file, "%u:\n%lu\n%lu", &idm1, &idu1, &idu2) == EOF)
        TEST_FAIL();
    if (fscanf(answers_file, "%u:\n%u\n%u", &idm2, &score1, &score2) == EOF)
        TEST_FAIL();
    fclose(probe_file);
    fclose(answers_file);

    TEST_ASSERT_EQUAL(idm1, idm2);
    Movie* movie = data->movies[idm1-1];
    for (uint r = 0; r < movie->nb_ratings; r++) {
        if (get_customer_id(movie->ratings[r]) == idu1)
            TEST_ASSERT_EQUAL(movie->ratings[r].score, score1);
        else if (get_customer_id(movie->ratings[r]) == idu2)
            TEST_ASSERT_EQUAL(movie->ratings[r].score, score2);
    }

    free_movie_data(data);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_write_movie_data_to_file);
    RUN_TEST(test_read_movie_data_from_file);
    RUN_TEST(test_write_user_data_to_file);
    RUN_TEST(test_read_user_data_from_file);
    RUN_TEST(test_parse_probe);
    return UNITY_END();
}