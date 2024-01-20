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
        .n = 10,
        .percent = 0.85
    };
    char *filepath = get_filepath("/data/data.bin");
    MovieData *data = read_movie_data_from_file(filepath);
    free(filepath);

    UserData *udata = to_user_oriented(data);

    Stats *stats = read_stats_from_data(data, &args);
    parse_probe("data/probe.txt", stats, data);

    FILE* probe_file = fopen("data/probe.txt", "r");
    FILE* predictions_file = fopen("data/probe_predictions.txt", "r");
    uint idm_1, idm_2, score, predicted;
    ulong idu_1, idu_2;

    if (fscanf(probe_file, "%u:", &idm_1) == EOF)
        TEST_FAIL();
    if (fscanf(predictions_file, "%u:\n", &idm_2) == EOF)
        TEST_FAIL();
    TEST_ASSERT_EQUAL(idm_1, idm_2);
    Movie* movie = data->movies[idm_1-1];
    for (int i = 0; i < 3 ; i++) {
        if (fscanf(probe_file, "%lu\n", &idu_1) == EOF)
            TEST_FAIL();
        if (fscanf(predictions_file, "%lu,%u,%u\n", &idu_2, &score, &predicted) == EOF)
            TEST_FAIL();
        for (uint r = 0; r < movie->nb_ratings; r++) {
            if (get_customer_id(movie->ratings[r]) == idu_2) {
                TEST_ASSERT_EQUAL(movie->ratings[r].score, score);
                TEST_ASSERT_EQUAL(predicted, knn_predictor(stats, udata->users[idu_1-1], idm_1));
                break;
            }
        }
    }

    fclose(probe_file);
    fclose(predictions_file);

    // Free memory
    free_movie_data(data);
    //free_user_data(udata);
    free_stats(stats);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_parse_probe);
    return UNITY_END();
}
