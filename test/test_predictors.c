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
    };
    char *filepath = get_filepath("/data/data.bin");
    MovieData *data = read_movie_data_from_file(filepath);
    free(filepath);

    Stats *stats = read_stats_from_data(data, &args);
    parse_probe("data/probe.txt", stats, data);

    FILE* probe_file = fopen("data/probe.txt", "r");
    FILE* predictions_file = fopen("data/probe_predictions.txt", "r");
    uint idm1, idm2, score1, score2;
    ulong idu1, idu2;
    if (fscanf(probe_file, "%u:\n%lu\n%lu", &idm1, &idu1, &idu2) == EOF)
        TEST_FAIL();
    if (fscanf(predictions_file, "%u:\n%u\n%u", &idm2, &score1, &score2) == EOF)
        TEST_FAIL();
    fclose(probe_file);
    fclose(predictions_file);

    TEST_ASSERT_EQUAL(idm1, idm2);
    Movie* movie = data->movies[idm1-1];
    for (uint r = 0; r < movie->nb_ratings; r++) 
    {
        if (get_customer_id(movie->ratings[r]) == idu1)
            TEST_ASSERT_EQUAL(movie->ratings[r].score, score1);
        else if (get_customer_id(movie->ratings[r]) == idu2)
            TEST_ASSERT_EQUAL(movie->ratings[r].score, score2);
    }
    // Free memory
    free_movie_data(data);
    free_stats(stats);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_parse_probe);
    return UNITY_END();
}
