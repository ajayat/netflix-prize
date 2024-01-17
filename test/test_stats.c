#include "unity.h"
#include "stats.h"
#include "parser.h"
#include "utils.h"

#include <stdlib.h>

#define MAX_USER_ID 2649430

void setUp(void) {}
void tearDown(void) {}

void stats_test(void) {
     Arguments args;
    // defaults values
    args.folder = "data/";
    args.limit = INT16_MAX;
    args.movie_id = 0;
    args.min = 0;
    args.time = false;
    args.nb_customer_ids = 0;
    args.nb_bad_reviewers = 0;
    args.likes_file = NULL;

    MovieData *movie_data = NULL;
    FILE *databin = fopen("data/fulldata.bin", "rb");
    if (databin != NULL) {
        movie_data = read_from_file(databin);
        fclose(databin);
    } else
        movie_data = parse();
    UserData *user_data = to_user_oriented(movie_data);
    
    Stats *stats = malloc(sizeof(Stats));
    stats->nb_movies = movie_data->nb_movies;
    stats->nb_users = user_data->nb_users;
    stats->movies = calloc(movie_data->nb_movies, sizeof(MovieStats));
    stats->users = calloc(MAX_USER_ID, sizeof(UserStats));

    MovieData *data = calculate_movies_stats(stats, &args, movie_data, user_data);

    TEST_ASSERT_EQUAL(data->nb_movies, movie_data->nb_movies);
    for (uint m = 0; m < data->nb_movies; m++) {
        TEST_ASSERT_EQUAL(data->movies[m]->id, movie_data->movies[m]->id);
        TEST_ASSERT_EQUAL(data->movies[m]->date, movie_data->movies[m]->date);
        TEST_ASSERT_EQUAL(data->movies[m]->nb_ratings, movie_data->movies[m]->nb_ratings);
        TEST_ASSERT_EQUAL_STRING(data->movies[m]->title, movie_data->movies[m]->title);
        for (uint r = 0; r < data->movies[m]->nb_ratings; r++) {
            TEST_ASSERT_EQUAL(data->movies[m]->ratings[r].customer_id_lsb, movie_data->movies[m]->ratings[r].customer_id_lsb);
            TEST_ASSERT_EQUAL(data->movies[m]->ratings[r].customer_id_msb, movie_data->movies[m]->ratings[r].customer_id_msb);
            TEST_ASSERT_EQUAL(data->movies[m]->ratings[r].date, movie_data->movies[m]->ratings[r].date);
            TEST_ASSERT_EQUAL(data->movies[m]->ratings[r].score, movie_data->movies[m]->ratings[r].score);
        }
    }

    uint test1 = stats->movies[8].average;

    args.folder = "data/";
    args.limit = days_from_epoch(2005, 6, 25); 
    args.movie_id = 25;
    args.min = 20;
    args.nb_bad_reviewers = 2;
    args.bad_reviewers = calloc(2, sizeof(ulong));
    args.bad_reviewers[0] = 1329923;
    args.bad_reviewers[1] = 2472537;

    free_stats(stats);
    stats = malloc(sizeof(Stats));
    free_movie_data(data);
    data = calculate_movies_stats(stats, &args, movie_data, user_data);

    uint test2 = stats->movies[8].average;
    TEST_ASSERT_NOT_EQUAL(test1, test2);

    free_stats(stats);
    free_movie_data(data);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(stats_test);
    return UNITY_END();
}