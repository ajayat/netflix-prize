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
    uint size = data->nb_movies * (data->nb_movies - 1) / 2;
    float *sim = calloc(size, sizeof(float));

    for (uint i = 1; i < data->nb_movies; i++) 
    {
        printf("\n\033[A\033[2K");  // Clear the line
        printf("Compute similarity matrix %.2lf%c", i*(i+1) / (size*2.) * 100, '%');
        Movie *movie1 = data->movies[i];
        Hashmap *ratings = hashmap_create(movie1->nb_ratings);

        for (uint r = 0; r < movie1->nb_ratings; r++)
            hashmap_insert(ratings, get_customer_id(movie1->ratings[r]), r);

        for (uint j = 0; j < i; j++) {
            uint x = get_similarity(sim, i, j);
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
    
    uint size = stats->nb_movies * (stats->nb_movies - 1) / 2;
    fwrite(&size, sizeof(uint), 1, bin);
    fwrite(stats->similarity, sizeof(float), size, bin);
    fclose(bin);
}

float *read_similarity_matrix(char *filename)
{
    FILE *bin = fopen(filename, "rb");
    if (bin == NULL) 
        return NULL;
    puts("Reading similarity matrix...");  // Information for the user
    uint size;
    if (!fread(&size, sizeof(uint), 1, bin))
        goto read_error;
    // Read the matrix
    float *sim = malloc(size * sizeof(float));
    if (!fread(sim, sizeof(float), size, bin)) {
        free(sim);
        goto read_error;
    }
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
    free(stats->users);
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
    return data;
}

void calculate_users_stats(Stats* stats, Arguments* args, UserData* user_data)
{
    for (uint u = 0; u < MAX_USER_ID; u++) 
    {
        User* user = user_data->users[u];
        if (user == NULL)
            continue;
        
        ulong c_id = (ulong)user->id;
        if (is_a_bad_reviewer(args, c_id) || !is_requested(args, c_id) || user->nb_ratings < args->min)
            continue;

        Hashmap *h = stats->users[c_id-1].frequency = hashmap_create(user->nb_ratings/10);
        for (uint r = 0; r < user->nb_ratings; r++) {
            if (user->ratings[r].date >= args->limit)
                continue;
            hashmap_increase(h, user->ratings[r].date, 1);
            stats->users[c_id-1].average += user->ratings[r].score;
            stats->users[c_id-1].nb_ratings++;
        }
        stats->users[c_id-1].average /= (double)stats->users[c_id-1].nb_ratings;
    }
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

    puts("Reading statistics:");  // Information for the user
    puts("    ~ for movies");
    MovieData *data = calculate_movies_stats(stats, args, movie_data, user_data);
    puts("    ~ for users");
    calculate_users_stats(stats, args, user_data);

    FILE *databin = fopen("data/data.bin", "wb");
    write_to_file(databin, data);
    if (fclose(databin) == EOF)
        perror("data.bin can't be closed.");

    if (args->movie_id != 0) // opt -s
        one_movie_stats(stats, args);
    
    stats->similarity = read_similarity_matrix("data/similarity.bin");
    if (stats->similarity == NULL) {
        stats->similarity = create_similarity_matrix(data);
        write_similarity_matrix(stats, "data/similarity.bin");
    }
    free_movie_data(data);
    return stats;
}
