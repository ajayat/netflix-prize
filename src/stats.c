#include <string.h>
#include <stdlib.h>

#include "stats.h"
#include "utils.h"

#define MAX_NUMBER_RATINGS 250000
#define MAX_CUSTOMER_ID 2649430

void free_stats(Stats *stats)
{
    if (stats == NULL)
        return;
    if (stats->movies != NULL) 
        free(stats->movies);
    free(stats);
}

void count_ratings(Data *data, unsigned int *ratings)
{
    for (unsigned int m = 0; m < data->nb_movies; m++) {
        for (unsigned int r = 0; r < data->movies[m]->nb_ratings; r++) {
            unsigned long id = get_customer_id(data->movies[m]->ratings[r]);
            ratings[id]++;
        }
    }
}

bool is_requested(Arguments *args, unsigned long id)
{
    for (unsigned int c = 0; c < args->nb_customer_ids; c++) {
        if (args->customer_ids[c] == id)
            return true;
    }
    return false;
}

bool is_a_bad_reviewer(Arguments *args, unsigned long id)
{
    for (unsigned int b=0; b<args->nb_bad_reviewers; b++) {
        if (args->bad_reviewers[b] == id)
            return true;
    }
    return false;
}

Stats *read_stats_from_data(Data *fulldata, Arguments *args)
{
    unsigned int ratings_count[MAX_CUSTOMER_ID] = {0};
    if (args->min > 0)
        count_ratings(fulldata, ratings_count);

    // Allocate memory for stats
    Stats *stats = malloc(sizeof(Stats));
    stats->movies = calloc(fulldata->nb_movies, sizeof(MovieStats));
    // Allocate memory for partial data
    Data *data = malloc(sizeof(Data));
    data->nb_movies = fulldata->nb_movies;
    data->movies = malloc(data->nb_movies * sizeof(Movie*));

    unsigned long c_id;
    for (unsigned int m = 0; m < data->nb_movies; m++)
    {
        Movie *movie = fulldata->movies[m];
        Movie *movie_dst = data->movies[m] = malloc(sizeof(Movie));
        // Copy of unchanged caracteristics
        movie_dst->id = movie->id;
        movie_dst->date = movie->date;
        movie_dst->title = strdup(movie->title);
        // Ratings treatment
        movie_dst->nb_ratings = 0;
        unsigned long r_dst = 0;
        movie_dst->ratings = malloc(MAX_NUMBER_RATINGS * sizeof(Rating));
        
        for (unsigned int r = 0; r < movie->nb_ratings ; r++) 
        {
            c_id = get_customer_id(movie->ratings[r]);

            if (movie->ratings[r].date >= args->limit // opt -l
                || ratings_count[c_id] < args->min // opt -e
                || args->nb_customer_ids && !is_requested(args, c_id) // opt -c
                || args->nb_bad_reviewers && is_a_bad_reviewer(args, c_id)) // opt -b
                continue;
            // Copy ratings
            memcpy(&movie_dst->ratings[r_dst++], &movie->ratings[r], sizeof(Rating));
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
    FILE *databin = fopen("data/data.bin", "r");
    write_to_file(databin, data);
    if (fclose(databin) == EOF)
        perror("data.bin can't be closed.");

    if (args->movie_id != 0) {
        char mv_filename[30];
        snprintf(mv_filename, 30, "data/stats_mv_%07u.csv", args->movie_id);
        FILE *one_movie = fopen(mv_filename, "w");

        MovieStats mv_stats = stats->movies[args->movie_id - 1];
        fprintf(one_movie, "average; min; max\n");
        fprintf(one_movie, "%lf; %ui; %ui\n", 
                mv_stats.average, mv_stats.min, mv_stats.max);

        if (fclose(one_movie) == EOF)
            perror("The file can't be closed."); // ! Modifier avec nom du fichier
    }
    free_data(data);
    return stats;
}
