#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "hashmap.h"
#include "stats.h"
#include "utils.h"
#include "hashmap.h"

#define MAX_USER_ID 2649430

void free_stats(Stats *stats)
{
    if (stats == NULL)
        return;
    if (stats->similarity != NULL) {
        for (uint i = 0; i < stats->nb_movies; i++)
            free(stats->similarity[i]);
        free(stats->similarity);
    }
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

double logistic(double x, double a, double b)
{
    return 1 / (1 + exp(-a * (x - b)));
}

double shrink(double value, uint n, double alpha)
{
    return value * (double)n / (n + alpha);
}

double mse_correlation(Movie *movie1, Movie *movie2)
{
    // if (movie1->id == 175)
    //     printf("Movie 2 : %d\n", movie2->id);
    Movie *min_movie, *max_movie;
    if (movie1->nb_ratings < movie2->nb_ratings) {
        min_movie = movie1;
        max_movie = movie2;
    } else {
        min_movie = movie2;
        max_movie = movie1;
    }
    Hashmap *h = hashmap_create(min_movie->nb_ratings);
    for (uint i = 0; i < min_movie->nb_ratings; i++)
        hashmap_insert(h, get_customer_id(min_movie->ratings[i]), i);

    // Calculate MSE between score given by same user
    double sum = 0;
    uint nb_ratings = 0;
    uint r2, customer_id;

    for (uint r1 = 0; r1 < max_movie->nb_ratings; r1++) {
        customer_id = get_customer_id(max_movie->ratings[r1]);
        if ((r2 = hashmap_get(h, customer_id)) == EMPTY)
            continue;
        sum += pow(movie1->ratings[r1].score - movie2->ratings[r2].score, 2);
        nb_ratings++;
    }
    hashmap_free(h);
    double c = (double)nb_ratings / (1.0 + sum);
    return shrink(c, nb_ratings, 400);
}

double **create_similarity_matrix(MovieData *data)
{
    double **sim = malloc(data->nb_movies * sizeof(double*));
    for (uint i = 0; i < data->nb_movies; i++) {
        sim[i] = calloc(data->nb_movies, sizeof(double));
        assert(sim[i] != NULL);
    }
    for (uint i = 0; i < data->nb_movies; i++) {
        printf("Compute similarity of movie : %u\n", i + 1);
        for (uint j = 0; j < i; j++) {
            sim[i][j] = mse_correlation(data->movies[i], data->movies[j]);
            sim[j][i] = sim[i][j];
        }
    }
    return sim;
}

void write_matrix_to_csv(double **matrix, uint size, char *filename)
{
    FILE *csv = fopen(filename, "w");
    for (uint i = 0; i < size; i++) {
        for (uint j = 0; j < size; j++)
            fprintf(csv, "%lf;", matrix[i][j]);
        fprintf(csv, "\n");
    }
    fclose(csv);
}

Stats *read_stats_from_data(MovieData *movie_data, Arguments *args)
{
    UserData *user_data = to_user_oriented(movie_data);
    // Allocate memory for stats
    Stats *stats = malloc(sizeof(Stats));
    stats->nb_movies = movie_data->nb_movies;
    stats->nb_users = user_data->nb_users;
    stats->movies = calloc(movie_data->nb_movies, sizeof(MovieStats));
    // Allocate memory for partial data
    MovieData *data = malloc(sizeof(MovieData));
    data->nb_movies = movie_data->nb_movies;
    data->movies = malloc(data->nb_movies * sizeof(Movie*));

    ulong c_id;
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
        
        for (uint r = 0; r < movie_src->nb_ratings ; r++) 
        {
            c_id = get_customer_id(movie_src->ratings[r]);

            if (movie_src->ratings[r].date >= args->limit // opt -l
                || user_data->users[c_id]->nb_ratings < args->min // opt -e
                || (args->nb_customer_ids && !is_requested(args, c_id)) // opt -c
                || (args->nb_bad_reviewers && is_a_bad_reviewer(args, c_id))) // opt -b
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
    // Free memory
    free_user_data(user_data);

    FILE *databin = fopen("data/data.bin", "wb");
    write_to_file(databin, data);
    free_movie_data(data);

    if (fclose(databin) == EOF)
        perror("data.bin can't be closed.");

    if (args->movie_id != 0) {
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
    stats->similarity = create_similarity_matrix(data);
    write_matrix_to_csv(stats->similarity, 100, "data/similarity.csv");
    return stats;
}
