#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "hashmap.h"
#include "stats.h"
#include "utils.h"
#include "hashmap.h"

#define MAX_USER_ID 2649430

// ========== Functions to build the similarity matrix ==========

double logistic(double x, double a, double b)
{
    return 1 / (1 + exp(-a * (x - b)));
}

double shrink(double value, uint n, double alpha)
{
    return value * (double)n / (n + alpha);
}

double mse_correlation(Movie *movie1, Movie *movie2, Hashmap *ratings)
{
    // Calculate MSE between score given by same user
    double sum = 0, diff;
    uint nb_ratings = 0;
    uint r1, customer_id;

    for (uint r2 = 0; r2 < movie2->nb_ratings; r2++) {
        customer_id = get_customer_id(movie2->ratings[r2]);
        if ((r1 = hashmap_get(ratings, customer_id)) == EMPTY)
            continue;
        diff = movie1->ratings[r1].score - movie2->ratings[r2].score;
        sum += diff * diff;
        nb_ratings++;
    }
    double c = (nb_ratings > 0) ? (double)nb_ratings / (nb_ratings + sum) : 0;
    return shrink(c, nb_ratings, 400);
}

float *create_similarity_matrix(MovieData *data)
{
    float *sim = calloc(data->nb_movies * data->nb_movies, sizeof(float));
    uint x;

    for (uint i = 0; i < data->nb_movies; i++) {
        printf("\n\033[A\033[2K");  // Clear the line
        printf("Compute similarity matrix %.2lf%c", 
                100 * i * (i+1) / (double)(data->nb_movies * data->nb_movies), '%');
        Movie *movie1 = data->movies[i];
        Hashmap *ratings = hashmap_create(movie1->nb_ratings);

        for (uint r = 0; r < movie1->nb_ratings; r++)
            hashmap_insert(ratings, get_customer_id(movie1->ratings[r]), r);

        for (uint j = 0; j <= i; j++) {
            x = i * data->nb_movies + j;
            sim[x] = (float)mse_correlation(movie1, data->movies[j], ratings);
            sim[j * data->nb_movies + i] = sim[x];
        }
        hashmap_free(ratings);
    }
    puts("\nDone!");  // Information for the user
    return sim;
}

void write_similarity_matrix_to_csv(Stats *stats, char *filename)
{
    FILE *csv = fopen(filename, "w");
    for (uint i = 0; i < stats->nb_movies; i++) {
        for (uint j = 0; j < stats->nb_movies; j++)
            fprintf(csv, "%f;", stats->similarity[i*stats->nb_movies + j]);
        fprintf(csv, "\n");
    }
    fclose(csv);
}

// ========== Functions to calculate statistics from the fulldata ==========

void free_stats(Stats *stats)
{
    if (stats == NULL)
        return;
    free(stats->similarity);
    free(stats->movies);
    free(stats);
}

bool is_requested(Arguments *args, ulong id)
{
    for (uint c = 0; c < args->nb_customer_ids; c++) {
        if (args->customer_ids[c] == id)
            return true;
    }
    return false;
}

bool is_a_bad_reviewer(Arguments *args, ulong id)
{
    for (uint b=0; b < args->nb_bad_reviewers; b++) {
        if (args->bad_reviewers[b] == id)
            return true;
    }
    return false;
}

bool ignored_rating(Arguments* args, UserData* user_data, Movie* movie_src, ulong c_id, uint r)
{
    if (movie_src->ratings[r].date >= args->limit // opt -l
        || user_data->users[c_id-1]->nb_ratings < args->min // opt -e
        || (args->nb_customer_ids && !is_requested(args, c_id)) // opt -c
        || (args->nb_bad_reviewers && is_a_bad_reviewer(args, c_id))) // opt -b
        return true;
    return false;
}

MovieData *calculate_movies_stats(Stats* stats, Arguments* args, MovieData* movie_data, UserData* user_data)
{
    // Allocate memory for partial data
    MovieData *data = malloc(sizeof(MovieData));
    data->nb_movies = movie_data->nb_movies;
    data->movies = malloc(data->nb_movies * sizeof(Movie*));

    for (uint m = 0; m < data->nb_movies; m++)
    {
        printf("\n\033[A\033[2K");  // Clear the line
        printf("Compute movies statistics %u/%u", movie_data->movies[m]->id, movie_data->nb_movies);
        Movie *movie_src = movie_data->movies[m];
        Movie *movie_dst = data->movies[m] = malloc(sizeof(Movie));
        // Copy of unchanged caracteristics
        movie_dst->id = movie_src->id;
        movie_dst->date = movie_src->date;
        movie_dst->title = strdup(movie_src->title);
        // Ratings treatment
        movie_dst->nb_ratings = 0;
        ulong r_dst = 0;
        movie_dst->ratings = malloc(movie_src->nb_ratings * sizeof(MovieRating));

        for (uint r = 0; r < movie_src->nb_ratings; r++) 
        {
            ulong c_id = get_customer_id(movie_src->ratings[r]);
            if (ignored_rating(args, user_data, movie_src, c_id, r))
                continue;
            // Copy ratings
            movie_dst->ratings[r_dst++] = movie_src->ratings[r];
            // Update stats
            stats->movies[m].average += (double)movie_src->ratings[r].score;
            if (movie_src->ratings[r].score > stats->movies[m].max)
                stats->movies[m].max = movie_src->ratings[r].score;
            if (movie_src->ratings[r].score < stats->movies[m].min)
                stats->movies[m].min = movie_src->ratings[r].score;
        }
        stats->movies[m].average /= (double)(r_dst);
        movie_dst->nb_ratings = r_dst;
    }
    puts("\nDone!");
    return data;
}

void calculate_users_stats(Stats* stats, Arguments* args, UserData* user_data)
{
    uint count = 0;
    for (uint u = 0; u < MAX_USER_ID; u++)
    {
        User* user = user_data->users[u];
        if (user == NULL)
            continue;
        count++;
        printf("\n\033[A\033[2K");  // Clear the line
        printf("Compute users statistics %u/%u", count, user_data->nb_users);
        ulong c_id = (ulong)user->id;
        if (is_a_bad_reviewer(args, c_id) || !is_requested(args, c_id) || user->nb_ratings < args->min)
            continue;
        Hashmap *h = stats->users[c_id-1].frequency = hashmap_create(user->nb_ratings/10);
        for (int r = 0; r < user->nb_ratings; r++)
        {
            if (user->ratings[r].date >= args->limit)
                continue;

            hashmap_increase(h,user->ratings[r].date,1);
            stats->users[c_id-1].average += user->ratings[r].score;
            stats->users[c_id-1].nb_ratings++;
        }
        stats->users[c_id-1].average /= (double)stats->users[c_id-1].nb_ratings;
    }
    puts("\nDone!");
}

void one_movie_stats(Stats* stats, Arguments* args)
{
    char mv_filename[30];
    snprintf(mv_filename, 30, "data/stats_mv_%07u.csv", args->movie_id);
    FILE *one_movie = fopen(mv_filename, "w");
    MovieStats mv_stats = stats->movies[args->movie_id - 1];
    fprintf(one_movie, "average; min; max\n");
    fprintf(one_movie, "%lf; %u; %u\n", 
            mv_stats.average, mv_stats.min, mv_stats.max);
    if (fclose(one_movie) == EOF)
        perror("The file for one movie can't be closed.");
}

Stats *read_stats_from_data(MovieData *movie_data, UserData *user_data, Arguments *args)
{
    // Allocate memory for stats
    Stats *stats = malloc(sizeof(Stats));
    stats->nb_movies = movie_data->nb_movies;
    stats->nb_users = user_data->nb_users;
    stats->movies = calloc(movie_data->nb_movies, sizeof(MovieStats));
    stats->users = calloc(MAX_USER_ID, sizeof(UserStats));

    puts("Reading statistics...");  // Information for the user
    MovieData *data = calculate_movies_stats(stats, args, movie_data, user_data);
    calculate_users_stats(stats, args, user_data);

    FILE *databin = fopen("data/data.bin", "wb");
    write_to_file(databin, data);
    if (fclose(databin) == EOF)
        perror("data.bin can't be closed.");

    if (args->movie_id != 0) // opt -s
        one_movie_stats(stats, args);
    
    stats->similarity = create_similarity_matrix(data);
    // Free memory
    free_movie_data(data);
    write_similarity_matrix_to_csv(stats, "data/similarity.csv");

    return stats;
}
