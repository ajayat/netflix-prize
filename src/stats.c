#include "stats.h"
#include "utils.h"
#include "parser.h"

#define MAX_NUMBER_RATINGS 250000

void free_stats(Stats *stats)
{
    if (stats != NULL) {
        if (stats->movies != NULL) {
            for (int m=0 ; m<stats->nb_movies ; m++)
                free(stats->movies[m]);
            free(stats->movies);
        }
        free(stats);
    }
    return;
}

bool count_ratings(Data *data, unsigned int *elite)
{
    for (int m=0 ; m<data->nb_movies ; m++) {
        for (int r=0 ; r<data->movies[m]->nb_ratings ; r++) {
            elite[get_customer_id(data->movies[m]->ratings[r])]++;
        }
    }
}

bool is_requested(Arguments *args, unsigned long id)
{
    for (int c=0 ; c<args->nb_customer_ids ; c++) {
        if (args->customer_ids[c] == id)
            return true;
    }
    return false;
}

bool not_a_bad_reviewer(Arguments *args, unsigned long id)
{
    for (int b=0 ; b<args->nb_bad_reviewers ; b++) {
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
    for (int m=0 ; m<data->nb_movies ; m++) {
        data->movies[m]->id = fulldata->movies[m]->id;
        data->movies[m]->date = fulldata->movies[m]->date;
        title_length = strlen(fulldata->movies[m]->title) + 1;
        data->movies[m]->title = malloc(title_length * sizeof(char));
        strncpy(data->movies[m]->title, fulldata->movies[m]->title, title_length);
        data->movies[m]->nb_ratings = 0;
        data->movies[m]->ratings = malloc(MAX_NUMBER_RATINGS * sizeof(Rating));
        for (int r=0 ; r<fulldata->movies[m]->nb_ratings ; r++) {
            c_id = get_customer_id(fulldata->movies[m]->ratings[r]);
            if (fulldata->movies[m]->ratings[r].date < args->limit
                && elite[c_id] > args->min
                && (args->nb_customer_ids==0 || is_requested(args, c_id))
                && (args->nb_bad_reviewers==0 || not_a_bad_reviewer(args, c_id)))
                {
                memcpy(data->movies[m]->ratings[data->movies[m]->nb_ratings], fulldata->movies[m]->ratings[r], sizeof(Rating));
                data->movies[m]->nb_ratings++;
                stats->movies[m].r_average += (float)(fulldata->movies[m]->ratings[r].score);
                if (fulldata->movies[m]->ratings[r].score > stats->movies[m].r_max)
                    stats->movies[m].r_max = fulldata->movies[m]->ratings[r].score;
                if (fulldata->movies[m]->ratings[r].score < stats->movies[m].r_min)
                    stats->movies[m].r_min = fulldata->movies[m]->ratings[r].score;
            }
        }
        stats->movies[m].r_average /= (float)(data->movies[m]->nb_ratings);
    }
    return stats;
}
