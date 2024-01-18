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

    MovieData *movie_data = read_movie_data_from_file("data/movie_data.bin");
    UserData *user_data = read_user_data_from_file("data/user_data.bin");

    Stats *stats = read_stats_from_data(movie_data, user_data, &args);
  
    double test1 = stats->movies[8].average;
    TEST_ASSERT_EQUAL(movie_data->movies[10]->nb_ratings, stats->movies[10].nb_ratings);
    TEST_ASSERT_EQUAL(movie_data->movies[20]->date, stats->movies[20].date);
    TEST_ASSERT_EQUAL(1, stats->movies[8].min);
    TEST_ASSERT_EQUAL(5, stats->movies[8].max);
    free_stats(stats);

    // Test (2) with options
    args.limit = days_from_epoch(2005, 6, 25); 
    args.min = 20;
    args.bad_reviewers = malloc(2 * sizeof(ulong));
    args.bad_reviewers[0] = 1329923;
    args.bad_reviewers[1] = 2472537;

    stats = read_stats_from_data(movie_data, user_data, &args);
    double test2 = stats->movies[8].average;
    TEST_ASSERT_NOT_EQUAL(test1, test2);
    TEST_ASSERT_NOT_EQUAL(user_data->nb_users, stats->nb_users);

    free(args.bad_reviewers);
    free_stats(stats);
    free_user_data(user_data);
    free_movie_data(movie_data);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_read_stats_from_data);
    return UNITY_END();
}