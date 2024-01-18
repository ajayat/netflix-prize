#include <errno.h>
#include <stdlib.h>

#include "unity.h"
#include "predictors.h"
#include "stats.h"
#include "utils.h"

void setUp(void) {}
void tearDown(void) {}

void test_parse_probe(void)
{
    Arguments args = {
        .folder = "stats/",
        .limit = INT16_MAX,
        .movie_id = 0,
        .nb_customer_ids = 0,
        .customer_ids = NULL,
        .nb_bad_reviewers = 0,
        .bad_reviewers = NULL,
        .min = 0,
        .time = false,
        .likes_file = NULL,
        .nb_recommandations = 10
    };
    char *mv_filepath = get_filepath("/data/movie_data.bin");
    MovieData *mv_data = read_movie_data_from_file(mv_filepath);
    free(mv_filepath);

    char *user_filepath = get_filepath("/data/user_data.bin");
    UserData *user_data = read_user_data_from_file(user_filepath);
    free(user_filepath);

    Stats *stats = read_stats_from_data(mv_data, user_data, &args);
    parse_probe("data/probe.txt", stats, mv_data, user_data);

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
    Movie* movie = mv_data->movies[idm1-1];
    for (uint r = 0; r < movie->nb_ratings; r++) {
        if (get_customer_id(movie->ratings[r]) == idu1)
            TEST_ASSERT_EQUAL(movie->ratings[r].score, score1);
        else if (get_customer_id(movie->ratings[r]) == idu2)
            TEST_ASSERT_EQUAL(movie->ratings[r].score, score2);
    }
    // Free memory
    free_movie_data(mv_data);
    free_user_data(user_data);
    free_stats(stats);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_parse_probe);
    return UNITY_END();
}
