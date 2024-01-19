#include "unity.h"
#include "stats.h"
#include "parser.h"
#include "utils.h"

#include <stdlib.h>

#define MAX_USER_ID 2649430

void setUp(void) {}
void tearDown(void) {}

void test_read_stats_from_data(void)
{
    Arguments args;
    // defaults values
    args.folder = "data/";
    args.limit = INT16_MAX;
    args.movie_id = 0;
    args.min = 0;
    args.time = false;
    args.nb_customer_ids = 0;
    args.nb_bad_reviewers = 0;

    MovieData *data = read_movie_data_from_file("data/data.bin");
    Stats *stats1 = read_stats_from_data(data, &args);
  
    TEST_ASSERT_EQUAL(data->movies[10]->nb_ratings, stats1->movies[10].nb_ratings);
    TEST_ASSERT_EQUAL(data->movies[20]->date, stats1->movies[20].date);
    TEST_ASSERT_EQUAL(1, stats1->movies[8].min);
    TEST_ASSERT_EQUAL(5, stats1->movies[8].max);

    // Test (2) with options
    args.limit = days_from_epoch(2005, 6, 25); 
    args.min = 20;
    args.bad_reviewers = malloc(2 * sizeof(ulong));
    args.bad_reviewers[0] = 1329923;
    args.bad_reviewers[1] = 2472537;

    Stats *stats2 = read_stats_from_data(data, &args);
    TEST_ASSERT_NOT_EQUAL(stats1->movies[8].average, stats2->movies[8].average);
    TEST_ASSERT_NOT_EQUAL(stats1->nb_users, stats2->nb_users);

    free(args.bad_reviewers);
    free_movie_data(data);
    free_stats(stats1);
    free_stats(stats2);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_read_stats_from_data);
    return UNITY_END();
}