#include <string.h>
#include <stdlib.h>

#include "stats.h"
#include "utils.h"

void free_stats(Stats *stats)
{
    if (stats == NULL)
        return;
    if (stats->movies != NULL) 
        free(stats->movies);
    free(stats);
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

Stats *read_stats_from_data(MovieData *movie_data, Arguments *args)
{
    UserData *user_data = to_user_oriented(movie_data);
    // Allocate memory for stats
    Stats *stats = malloc(sizeof(Stats));
    stats->movies = calloc(movie_data->nb_movies, sizeof(MovieStats));
    // Allocate memory for partial data
    MovieData *data = malloc(sizeof(MovieData));
    data->nb_movies = movie_data->nb_movies;
    data->movies = malloc(data->nb_movies * sizeof(Movie*));

    unsigned long c_id;
    for (unsigned int m = 0; m < data->nb_movies; m++)
    {
        Movie *movie = movie_data->movies[m];
        Movie *movie_dst = data->movies[m] = malloc(sizeof(Movie));
        // Copy of unchanged caracteristics
        movie_dst->id = movie->id;
        movie_dst->date = movie->date;
        movie_dst->title = strdup(movie->title);
        // Ratings treatment
        movie_dst->nb_ratings = 0;
        unsigned long r_dst = 0;
        movie_dst->ratings = malloc(movie->nb_ratings * sizeof(Rating));
        
        for (unsigned int r = 0; r < movie->nb_ratings ; r++) 
        {
            c_id = get_customer_id(movie->ratings[r]);

            if (movie->ratings[r].date >= args->limit // opt -l
                || user_data->users[c_id]->nb_ratings < args->min // opt -e
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
    free_movie_data(data);
    free_user_data(user_data);
    return stats;
}
