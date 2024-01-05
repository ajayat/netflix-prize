#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "stats.h"
#include "utils.h"

#define MAX_USER_ID 2649430

void free_stats(Stats *stats)
{
    if (stats == NULL)
        return;
    if (stats->movies != NULL) 
        free(stats->movies);
    free(stats);
}

bool is_requested(Arguments *args, u_long id)
{
    for (u_int c = 0; c < args->nb_customer_ids; c++) {
        if (args->customer_ids[c] == id)
            return true;
    }
    return false;
}

bool is_a_bad_reviewer(Arguments *args, u_long id)
{
    for (u_int b=0; b < args->nb_bad_reviewers; b++) {
        if (args->bad_reviewers[b] == id)
            return true;
    }
    return false;
}

double logistic(double x, double a, double b)
{
    return 1 / (1 + exp(-a * (x - b)));
}

double shrink(double value, u_int n, double alpha)
{
    return value * (double)n / (n + alpha);
}

double mse_correlation(Movie *movie1, Movie *movie2)
{
    u_int user_ratings1[MAX_USER_ID] = { -1 };
    u_int user_ratings2[MAX_USER_ID] = { -1 };

    for (u_int i = 0; i < movie1->nb_ratings; i++)
        user_ratings1[get_customer_id(movie1->ratings[i])] = i;
    
    for (u_int i = 0; i < movie2->nb_ratings; i++)
        user_ratings2[get_customer_id(movie2->ratings[i])] = i;

    // Calculate MSE between score given by same user
    double sum = 0;
    u_int nb_ratings = 0;
    int r1, r2;

    for (int i = 0; i < MAX_USER_ID; i++) {
        r1 = user_ratings1[i];
        r2 = user_ratings2[i];
        if (r1 != -1 && r2 != -1) {
            sum += pow(movie1->ratings[r1].score - movie2->ratings[r2].score, 2);
            nb_ratings++;
        }
    }
    double c = (double)nb_ratings / sum;
    return shrink(c, nb_ratings, 100);
}

double **create_similarity_matrix(MovieData *data)
{
    double **sim = malloc(data->nb_movies * sizeof(double*));
    for (u_int i = 0; i < data->nb_movies; i++) {
        sim[i] = calloc(data->nb_movies, sizeof(double));
        assert(sim[i] != NULL);
    }
    for (u_int i = 0; i < data->nb_movies; i++) {
        for (u_int j = 0; j < i + 1; j++) {
            if (i == j) continue;
            sim[i][j] = mse_correlation(data->movies[i], data->movies[j]);
            sim[j][i] = sim[i][j];
        }
    }
    return sim;
}

Stats *read_stats_from_data(MovieData *movie_data, UserData *user_data, Arguments *args)
{
    // Allocate memory for stats
    Stats *stats = malloc(sizeof(Stats));
    stats->movies = calloc(movie_data->nb_movies, sizeof(MovieStats));
    // Allocate memory for partial data
    MovieData *data = malloc(sizeof(MovieData));
    data->nb_movies = movie_data->nb_movies;
    data->movies = malloc(data->nb_movies * sizeof(Movie*));

    u_long c_id;
    for (u_int m = 0; m < data->nb_movies; m++)
    {
        Movie *movie = movie_data->movies[m];
        Movie *movie_dst = data->movies[m] = malloc(sizeof(Movie));
        // Copy of unchanged caracteristics
        movie_dst->id = movie->id;
        movie_dst->date = movie->date;
        movie_dst->title = strdup(movie->title);
        // Ratings treatment
        movie_dst->nb_ratings = 0;
        u_long r_dst = 0;
        movie_dst->ratings = malloc(movie->nb_ratings * sizeof(MovieRating));
        
        for (u_int r = 0; r < movie->nb_ratings; r++) 
        {
            c_id = get_customer_id(movie->ratings[r]);

            if (movie->ratings[r].date >= args->limit // opt -l
                || user_data->users[c_id]->nb_ratings < args->min // opt -e
                || (args->nb_customer_ids && !is_requested(args, c_id)) // opt -c
                || (args->nb_bad_reviewers && is_a_bad_reviewer(args, c_id))) // opt -b
                continue;
            // Copy ratings
            movie_dst->ratings[r_dst++] = movie->ratings[r];
            // Update stats
            stats->movies[m].average += (double)(movie->ratings[r].score);
            if (movie->ratings[r].score > stats->movies[m].max)
                stats->movies[m].max = movie->ratings[r].score;

            if (movie->ratings[r].score < stats->movies[m].min)
                stats->movies[m].min = movie->ratings[r].score;
        }
        stats->movies[m].average /= (double)(r_dst);
        movie_dst->nb_ratings = r_dst;
    }
    FILE *databin = fopen("data/data.bin", "w");
    write_to_file(databin, data);
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
    return stats;
}
