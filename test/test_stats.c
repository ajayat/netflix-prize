#include "unity.h"
#include "stats.h"
#include "parser.h"
#include "utils.h"

#include <stdlib.h>

#define MAX_USER_ID 2649430

void setUp(void) {}
void tearDown(void) {}

void test_stats(void)
{
     Arguments args;
    // defaults values
    args.limit = INT16_MAX;
    args.min = 0;
    args.time = false;
    args.nb_customer_ids = 0;
    args.nb_bad_reviewers = 0;

    MovieData *movie_data = read_movie_data_from_file("data/movie_data.bin");
    UserData *user_data = read_user_data_from_file("data/user_data.bin");

    Stats *stats = read_stats_from_data(movie_data, user_data, &args);
    MovieData *data = read_movie_data_from_file("data/data.bin");

    TEST_ASSERT_EQUAL(data->nb_movies, movie_data->nb_movies);
    for (uint m = 0; m < data->nb_movies; m++) 
    {
        Movie *movie1 = data->movies[m];
        Movie *movie2 = movie_data->movies[m];
        TEST_ASSERT_EQUAL(movie1->id, movie2->id);
        TEST_ASSERT_EQUAL(movie1->date, movie2->date);
        TEST_ASSERT_EQUAL(movie1->nb_ratings, movie2->nb_ratings);
        TEST_ASSERT_EQUAL_STRING(movie1->title, movie2->title);

        for (uint r = 0; r < movie1->nb_ratings; r++) {
            MovieRating rating1 = movie1->ratings[r];
            MovieRating rating2 = movie2->ratings[r];
            TEST_ASSERT_EQUAL(get_customer_id(rating1), get_customer_id(rating2));
            TEST_ASSERT_EQUAL(rating1.date, rating2.date);
            TEST_ASSERT_EQUAL(rating1.score, rating2.score);
        }
    }
    double test1 = stats->movies[8].average;
    free_movie_data(data);
    free_stats(stats);

    // Test (2) with options
    args.limit = days_from_epoch(2005, 6, 25); 
    args.min = 20;
    args.bad_reviewers = parse_ids("1329923 2472537", &args.nb_bad_reviewers);

    stats = read_stats_from_data(movie_data, user_data, &args);
    double test2 = stats->movies[8].average;
    TEST_ASSERT_NOT_EQUAL(test1, test2);

    free_stats(stats);
    free_user_data(user_data);
    free_movie_data(data);
    free_movie_data(movie_data);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_stats);
    return UNITY_END();
}