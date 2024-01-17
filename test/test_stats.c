#include "unity.h"
#include "stats.h"
#include "parser.h"
#include "utils.h"

#include <stdlib.h>

#define MAX_USER_ID 2649430

void setUp(void) {}
void tearDown(void) {}

void test_stats(void) {
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

    FILE *mv_file = fopen("data/movie_data.bin", "rb");
    MovieData *movie_data = read_movie_data_from_file(mv_file);
    fclose(mv_file);

    FILE *user_file = fopen("data/user_data.bin", "rb");
    UserData *user_data = read_user_data_from_file(user_file);
    fclose(user_file);

    Stats *stats = malloc(sizeof(Stats));
    stats->nb_movies = movie_data->nb_movies;
    stats->nb_users = user_data->nb_users;
    stats->movies = calloc(movie_data->nb_movies, sizeof(MovieStats));
    stats->users = calloc(MAX_USER_ID, sizeof(UserStats));
    stats->similarity = NULL;

    MovieData *data = calculate_movies_stats(stats, &args, movie_data, user_data);

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
    stats->nb_movies = movie_data->nb_movies;
    stats->nb_users = user_data->nb_users;
    stats->movies = calloc(movie_data->nb_movies, sizeof(MovieStats));
    stats->users = calloc(MAX_USER_ID, sizeof(UserStats));
    stats->similarity = NULL;
    free_movie_data(data);
    data = calculate_movies_stats(stats, &args, movie_data, user_data);

    uint test2 = stats->movies[8].average;
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