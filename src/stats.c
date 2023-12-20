#include <string.h>
#include <stdlib.h>
#include "stats.h"
#include "utils.h"

#define MAX_NUMBER_RATINGS 250000

void free_stats(Stats *stats)
{
    if (stats != NULL) {
        if (stats->movies != NULL) 
            free(stats->movies);
        free(stats);
    }
    return;
}

void count_ratings(Data *data, unsigned int *elite)
{
    for (unsigned int m=0 ; m<data->nb_movies ; m++) {
        for (unsigned int r=0 ; r<data->movies[m]->nb_ratings ; r++) {
            elite[get_customer_id(data->movies[m]->ratings[r])]++;
        }
    }
}

bool is_requested(Arguments *args, unsigned long id)
{
    for (unsigned int c=0 ; c<args->nb_customer_ids ; c++) {
        if (args->customer_ids[c] == id)
            return true;
    }
    return false;
}

bool not_a_bad_reviewer(Arguments *args, unsigned long id)
{
    for (unsigned int b=0 ; b<args->nb_bad_reviewers ; b++) {
        if (args->bad_reviewers[b] == id)
            return false;
    }
    return true;
}

Stats *read_stats_from_data(Data *fulldata, Arguments *args)
{
    unsigned int elite[2649429] = {0};
    if (args->min>1)
        count_ratings(fulldata, elite);

    // Allocate memory for stats
    Stats *stats = malloc(sizeof(Stats));
    stats->movies = malloc(fulldata->nb_movies * sizeof(Movie));
    // Allocate memory for partial data
    Data *data = malloc(sizeof(Data));
    data->nb_movies = fulldata->nb_movies;
    data->movies = malloc(data->nb_movies*sizeof(Movie));

    unsigned long c_id;
    int title_length;
    for (unsigned int m=0 ; m<data->nb_movies ; m++) {
        // Copy of unchanged caracteristics
        data->movies[m]->id = fulldata->movies[m]->id;
        data->movies[m]->date = fulldata->movies[m]->date;
        title_length = strlen(fulldata->movies[m]->title) + 1;
        data->movies[m]->title = malloc(title_length * sizeof(char));
        strncpy(data->movies[m]->title, fulldata->movies[m]->title, title_length);
        // Ratings treatment
        data->movies[m]->nb_ratings = 0;
        data->movies[m]->ratings = malloc(MAX_NUMBER_RATINGS * sizeof(Rating));
        for (uint32_t r=0 ; r<fulldata->movies[m]->nb_ratings ; r++) {
            c_id = get_customer_id(fulldata->movies[m]->ratings[r]);
            if (fulldata->movies[m]->ratings[r].date < args->limit // opt -l
                && elite[c_id] > args->min // opt -e
                && (args->nb_customer_ids==0 || is_requested(args, c_id)) // opt -c
                && (args->nb_bad_reviewers==0 || not_a_bad_reviewer(args, c_id))) // opt -b
                {
                memcpy(&(data->movies[m]->ratings[data->movies[m]->nb_ratings]), &(fulldata->movies[m]->ratings[r]), sizeof(Rating));
                data->movies[m]->nb_ratings++;
                stats->movies[m].average += (float)(fulldata->movies[m]->ratings[r].score);
                if (fulldata->movies[m]->ratings[r].score > stats->movies[m].max)
                    stats->movies[m].max = fulldata->movies[m]->ratings[r].score;
                if (fulldata->movies[m]->ratings[r].score < stats->movies[m].min)
                    stats->movies[m].min = fulldata->movies[m]->ratings[r].score;
            }
        }
        stats->movies[m].average /= (float)(data->movies[m]->nb_ratings);
    }
    FILE *databin = fopen("/data/data.bin","r");
    write_to_file(databin, data);
    if (fclose(databin) == EOF)
        perror("data.bin can't be closed.");
    if (args->movie_id != 0) {
        FILE *one_movie = fopen("NNNNNNN","r"); // ! Nom personnalisé selon le titre du film (ou sont id ?).
        fwrite(&(stats->movies[args->movie_id-1].average),sizeof(float),1,one_movie);
        fwrite(&(stats->movies[args->movie_id-1].min),sizeof(uint8_t),1,one_movie);
        fwrite(&(stats->movies[args->movie_id-1].max),sizeof(uint8_t),1,one_movie);
        if (fclose(one_movie) == EOF)
            perror("The file can't be closed."); // ! Modifier avec nom du fichier
    }
    free_data(data);
    return stats;
}
