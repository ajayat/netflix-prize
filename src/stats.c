#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>

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

static double mse_correlation(Movie *movie1, Movie *movie2, Hashmap *ratings)
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
    ulong size = (ulong)data->nb_movies * (data->nb_movies - 1) / 2;
    float *sim = calloc(size, sizeof(float));

    for (uint i = 1; i < data->nb_movies; i++) 
    {
        printf("\n\033[A\033[2K");  // Clear the line
        printf("Compute similarity matrix %.2lf%%", i*(i+1) / (size*2.) * 100);
        Movie *movie1 = data->movies[i];
        Hashmap *ratings = hashmap_create(movie1->nb_ratings);

        for (uint r = 0; r < movie1->nb_ratings; r++)
            hashmap_insert(ratings, get_customer_id(movie1->ratings[r]), r);

        for (uint j = 0; j < i; j++) {
            uint x = i * (i - 1) / 2 + j;
            sim[x] = (float)mse_correlation(movie1, data->movies[j], ratings);
        }
        hashmap_free(ratings);
    }
    puts("\nDone!");  // Information for the user
    return sim;
}

void write_similarity_matrix(Stats *stats, char *filename)
{
    FILE *bin = fopen(filename, "wb");
    if (bin == NULL)
        return perror("The file for the similarity matrix can't be opened.");
    
    ulong size = (ulong)stats->nb_movies * (stats->nb_movies - 1) / 2;
    fwrite(&size, sizeof(size), 1, bin);
    fwrite(stats->similarity, sizeof(float), size, bin);
    fclose(bin);
}

float *read_similarity_matrix(char *filename)
{
    FILE *bin = fopen(filename, "rb");
    if (bin == NULL) 
        return NULL;
    // puts("Reading similarity matrix...");  // Information for the user
    ulong size = 0;
    if (!fread(&size, sizeof(size), 1, bin))
        goto read_error;
    // Read the matrix
    float *sim = malloc(size * sizeof(float));
    if (!fread(sim, sizeof(float), size, bin)) {
        free(sim);
        goto read_error;
    }
    fclose(bin);
    return sim;

read_error:
    fclose(bin);
    return NULL;
}

// ========== Functions to calculate statistics from the fulldata ==========

void free_stats(Stats *stats)
{
    if (stats == NULL)
        return;
    free(stats->similarity);
    free(stats->movies);
    for (uint i = 0; i < MAX_USER_ID; i++)
        hashmap_free(stats->users[i].frequency);

    free(stats->users);
    free(stats);
}

static bool is_requested(Arguments *args, ulong id)
{
    for (uint c = 0; c < args->nb_customer_ids; c++) {
        if (args->customer_ids[c] == id)
            return true;
    }
    return false;
}

static bool is_a_bad_reviewer(Arguments *args, ulong id)
{
    for (uint b=0; b < args->nb_bad_reviewers; b++) {
        if (args->bad_reviewers[b] == id)
            return true;
    }
    return false;
}

static uint *number_of_ratings(MovieData *data)
{
    uint *ratings_count = calloc(MAX_USER_ID, sizeof(uint));

    for (uint m = 0; m < data->nb_movies; m++) {
        Movie *movie = data->movies[m];
        for (uint r = 0; r < movie->nb_ratings; r++)
            ratings_count[get_customer_id(movie->ratings[r]) - 1]++;
    }
    return ratings_count;
}

bool ignored_rating(Arguments *args, MovieRating rating, ulong c_id, uint c_nb_ratings)
{
    if (rating.date >= args->limit // opt -l
        || c_nb_ratings < args->min // opt -e
        || (args->nb_customer_ids && !is_requested(args, c_id)) // opt -c
        || (args->nb_bad_reviewers && is_a_bad_reviewer(args, c_id))) // opt -b
        return true;
    return false;
}

void compute_stats(Stats *stats, MovieData *data, Arguments *args)
{
    uint *ratings_count = number_of_ratings(data);

    stats->nb_movies = data->nb_movies;
    for (uint m = 0u; m < data->nb_movies; m++)
    {
        printf("\n\033[A\033[2K");  // Clear the line
        printf("Compute statistics: %.0f%%", (float)m / data->nb_movies * 100);
        Movie *movie = data->movies[m];
        // Initialize statistics for this movie
        stats->movies[m].date = movie->date;
        stats->movies[m].min = UINT8_MAX;

        for (uint r = 0u; r < movie->nb_ratings; r++) 
        {
            MovieRating rating = movie->ratings[r];
            ulong id = get_customer_id(rating);
            if (ignored_rating(args, rating, id, ratings_count[id-1]))
                continue; // Skip the undesired rating.

            // Update movie stats
            stats->movies[m].nb_ratings++;
            stats->movies[m].average += (double)(rating.score);
            if (rating.score > stats->movies[m].max)
                stats->movies[m].max = rating.score;
            if (rating.score < stats->movies[m].min)
                stats->movies[m].min = rating.score;

            // Update user stats
            if (stats->users[id-1].frequency == NULL)
                stats->users[id-1].frequency = hashmap_create(ratings_count[id-1] / 10);
            hashmap_increase(stats->users[id-1].frequency, rating.date, 1);

            stats->users[id-1].average += rating.score;
            stats->users[id-1].nb_ratings++;
        }
        stats->movies[m].average /= (double)(stats->movies[m].nb_ratings);
    }
    free(ratings_count);
    // Compute average for users
    for (ulong u = 0u; u < MAX_USER_ID; u++) {
        if (stats->users[u].nb_ratings > 0) {
            stats->nb_users++;
            stats->users[u].average /= (double)stats->users[u].nb_ratings;
        }
    }
    puts(" ✅");  // Information for the user
}

static void one_movie_stats(Stats* stats, Arguments* args)
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

Stats *read_stats_from_data(MovieData *data, Arguments *args)
{
    // Allocate memory for stats
    Stats *stats = malloc(sizeof(Stats));
    stats->nb_movies = data->nb_movies;
    stats->nb_users = 0;
    stats->movies = calloc(data->nb_movies, sizeof(MovieStats));
    stats->users = calloc(MAX_USER_ID, sizeof(UserStats));
    // Compute stats
    compute_stats(stats, data, args);

    if (args->movie_id != 0) // opt -s
        one_movie_stats(stats, args);
    
    stats->similarity = read_similarity_matrix("data/similarity.bin");
    if (stats->similarity == NULL) {
        stats->similarity = create_similarity_matrix(data);
        write_similarity_matrix(stats, "data/similarity.bin");
    }
    return stats;
}

Stats *read_stats_from_file(char *filepath)
{
    FILE *file = fopen(filepath, "rb");
    if (file == NULL)
        return NULL;
    // Read movies stats
    Stats *stats = malloc(sizeof(Stats));
    stats->movies = NULL;
    stats->users = calloc(MAX_USER_ID, sizeof(UserStats));

    if (!fread(&stats->nb_movies, sizeof(stats->nb_movies), 1, file)
        || !fread(&stats->nb_users, sizeof(stats->nb_users), 1, file))
        goto read_error;
    stats->movies = malloc(stats->nb_movies * sizeof(MovieStats));
    if (!fread(stats->movies, sizeof(MovieStats), stats->nb_movies, file))
        goto read_error;

    // Read users stats
    for (uint i = 0; i < stats->nb_users; i++) {
        uint64_t id;
        if (!fread(&id, sizeof(id), 1, file))
            goto read_error;
        UserStats *user = &stats->users[id-1];
        user->frequency = read_hashmap_from_file(file);
        if (user->frequency == NULL
            || !fread(&user->average, sizeof(user->average), 1, file)
            || !fread(&user->nb_ratings, sizeof(user->nb_ratings), 1, file))
            goto read_error;
    }
    stats->similarity = read_similarity_matrix("data/similarity.bin");
    if (stats->similarity == NULL)
        goto read_error;
    fclose(file);
    return stats;

read_error:
    fclose(file);
    free_stats(stats);
    return NULL;
}

void write_stats_to_file(Stats *stats, char *filepath)
{
    FILE *file = fopen(filepath, "wb");
    if (file == NULL)
        return perror("The stats.bin file can't be opened");
    
    fwrite(&stats->nb_movies, sizeof(stats->nb_movies), 1, file);
    fwrite(&stats->nb_users, sizeof(stats->nb_users), 1, file);
    fwrite(stats->movies, sizeof(MovieStats), stats->nb_movies, file);
    for (uint i = 0; i < MAX_USER_ID; i++) {
        UserStats *user = &stats->users[i];
        if (stats->users[i].nb_ratings == 0)
            continue;
        uint64_t id = i + 1;
        fwrite(&id, sizeof(id), 1, file);
        write_hashmap_to_file(user->frequency, file);
        fwrite(&user->average, sizeof(user->average), 1, file);
        fwrite(&user->nb_ratings, sizeof(stats->users[i].nb_ratings), 1, file);
    }
    fclose(file);
}
