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
    uint k = 80;
    double tau = 0.01;
    double scale = 4;
    double offset = -5;

    double score = 0;
    double sum_weights = 0;
    UserRating *nearest_ratings;
    
    if (user->nb_ratings > k)
        nearest_ratings = knn_ratings(stats, user, movie_id, k);
    else
        nearest_ratings = user->ratings;

    for (int i = 0; i < min(k, user->nb_ratings); i++) {
        UserRating rating = nearest_ratings[i];
        double s = get_similarity(stats->similarity, movie_id-1, rating.movie_id-1);
        uint delta = abs(rating.date - stats->movies[movie_id].date);
        double time_factor = 1 / (1 + tau * delta);
        double weight = logistic(scale * s * time_factor + offset, 1, 0);
        score += weight * nearest_ratings[i].score;
        sum_weights += weight;
    }
    if (k > stats->nb_movies)
        free(nearest_ratings);
    return score / sum_weights;
}

static double proximity(Stats *stats, uint i, uint *ids, uint m)
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

uint *knn_movies(Stats *stats, uint *ids, uint n, uint k, double p)
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

void parse_probe(char *filename, Stats *stats, MovieData *data)
{
    UserData *user_data = to_user_oriented(data);

    FILE* probe_file = fopen(filename, "r");
    if (probe_file == NULL) {
        perror("Could not open probe file.");
        return;
    }
    FILE* probe_prediction = fopen("data/probe_predictions.txt", "w");
    if (probe_prediction == NULL) {
        perror("Could not open probe prediction file.");
        return;
    }
    Movie *movie = data->movies[0];
    char c;
    ulong id;
    float m = 0;
    while  (fscanf(probe_file, "%lu%c\n", &id, &c) != EOF)
    {
        if (c == ':') {
            m += 1.0;
            printf("\n\033[A\033[2K");  // Clear the line
            printf("Parsing probe file: %.0lf%%", m / PROBE_MOVIES_COUNT * 100);
            fprintf(probe_prediction, "%lu:\n", id);
            movie = data->movies[id-1];
            continue;
        }
        uint8_t score = 0;
        for (uint r = 0; r < movie->nb_ratings; r++) {
            if (get_customer_id(movie->ratings[r]) == id) {
                score = movie->ratings[r].score;
                break;
            }
        }
        double predicted_score = knn_predictor(stats, user_data->users[id-1], movie->id);
        fprintf(probe_prediction, "%lu,%u,%lf\n", id, score, predicted_score);

    }
    puts(" ✅");  // Information for the user
    fclose(probe_prediction);
    fclose(probe_file);
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