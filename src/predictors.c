#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "parser.h"
#include "predictors.h"
#include "stats.h"
#include "utils.h"

static float *similarity_matrix = NULL;
static uint movie_id = 0;

static int compare_ratings(const void *a, const void *b)
{
    UserRating *r1 = (UserRating *)a;
    UserRating *r2 = (UserRating *)b;
    float s1 = get_similarity(similarity_matrix, movie_id-1, r1->movie_id-1);
    float s2 = get_similarity(similarity_matrix, movie_id-1, r2->movie_id-1);
    return s2 - s1;
}

static int compare_scores(const void *a, const void *b)
{
    Score *d1 = (Score *)a;
    Score *d2 = (Score *)b;
    double diff = d2->score - d1->score;
    if (diff > 0) return 1;
    else if (diff < 0) return -1;
    else return 0;
}

UserRating *knn_ratings(Stats *stats, User *u, uint i, uint k)
{
    similarity_matrix = stats->similarity;
    movie_id = i;
    qsort(u->ratings, u->nb_ratings, sizeof(UserRating), compare_ratings);

    UserRating *ratings = calloc(k, sizeof(UserRating));
    for (uint j = 0; j < k; j++)
        ratings[j] = u->ratings[j];
    return ratings;
}

double knn_predictor(Stats *stats, User *user, uint movie_id)
{
    uint k = 2000;  // number of nearest neighbors
    double tau = 0.0;  // time decay
    double scale = 35.;  // scale of the logistic function
    double offset = -19;  // penalize low similarities

    double score = 0, sum_weights = 0;
    UserRating *nearest_ratings;
    
    if (user->nb_ratings > k)
        nearest_ratings = knn_ratings(stats, user, movie_id, k);
    else
        nearest_ratings = user->ratings;

    double s, time_factor, weight;
    for (int i = 0; i < min(k, user->nb_ratings); i++)
    {
        UserRating rating = nearest_ratings[i];
        s = get_similarity(stats->similarity, movie_id-1, rating.movie_id-1);
        int delta = rating.date - stats->movies[movie_id-1].date;
        time_factor = 1 / (1 + tau * abs(delta));
        weight = logistic(scale * s * time_factor + offset, 1, 0);
        score += weight * nearest_ratings[i].score;
        sum_weights += weight;
    }
    if (user->nb_ratings > k)
        free(nearest_ratings);
    return score / sum_weights;
}

static double proximity(Stats *stats, uint i, ulong *ids, uint m)
{
    double distance = 0;
    double sum_weights = 0;
    for (uint j = 0; j < m; j++) {
        if (i == ids[j]-1)
            return 0.;  // This movie is already liked.
        double s = get_similarity(stats->similarity, i, ids[j]-1);
        double weight = exp(1.8 * s);
        distance += weight * s;
        sum_weights += weight;
    }
    return distance / sum_weights;
}

uint *knn_movies(Stats *stats, ulong *ids, uint n, uint k, double p)
{
    Score scores[stats->nb_movies];
    uint *movies = calloc(k, sizeof(uint));

    for (uint i = 0; i < stats->nb_movies; i++) {
        double distance = proximity(stats, i, ids, n);  // between 0 and 1
        double popularity = (double)stats->movies[i].nb_ratings / stats->nb_users;
        double quality = stats->movies[i].average / 5 * popularity;
        scores[i].movie_id = i + 1;
        scores[i].score = (1 - p) * quality + p * distance;
    }
    qsort(scores, stats->nb_movies, sizeof(Score), compare_scores);

    for (uint i = 0; i < k; i++)
        movies[i] = scores[i].movie_id;
    return movies;
}

// ====================== Probe prediction =====================

static void parse_probe(FILE *out, char *buffer, Stats *stats, UserData *data)
{
    char *begin = buffer, *end = NULL;
    ulong movie_id = 0, id = 0;
    while ((id = strtoul(begin, &end, 10)) != 0)
    {
        begin = end + 1;
        if (end[0] == ':') {
            fprintf(out, "%lu:\n", id);
            movie_id = id;
            continue;
        }
        User *user = data->users[id-1];
        for (uint r = 0; r < user->nb_ratings; r++)
        {
            if (user->ratings[r].movie_id != movie_id)
                continue;
            double predicted = knn_predictor(stats, user, movie_id);
            fprintf(out, "%lu,%u,%lf\n", id, user->ratings[r].score, predicted);
            break;
        }
    }
}

void write_probe_predictions(char *filename, Stats *stats, UserData *data)
{
    puts("Parsing the probe.txt file...");

    FILE* probe_file = fopen("data/probe.txt", "r");
    if (probe_file == NULL) {
        perror("Could not open data/probe.txt file.");
        return;
    }
    FILE* probe_prediction = fopen(filename, "w");
    if (probe_prediction == NULL) {
        perror("Could not open probe prediction file");
        fclose(probe_file);
        return;
    }
    uint size = get_size(probe_file);
    char *buffer = calloc((size + 1), sizeof(char));
    if (fread(buffer, 1, size, probe_file) != 0)
        parse_probe(probe_prediction, buffer, stats, data);
    
    fclose(probe_prediction);
    fclose(probe_file);
    free(buffer);
}

double rmse_probe_calculation(char* filename)
{
    FILE* probe_file = fopen(filename, "r");
    double s1, s2, diff;
    double n = 0;
    double rmse = 0;

    char line[7];
    while (fgets(line, 7, probe_file) != NULL) {
        while (fscanf(probe_file, "%*[0-9],%lf,%lf\n", &s1, &s2) == 2) {
            diff = s1 - s2;
            rmse += diff * diff;
            n += 1.0;
        }
    }
    fclose(probe_file);
    return (n == 0) ? 0 : sqrt(rmse/n);
}