#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "parser.h"
#include "predictors.h"
#include "stats.h"
#include "utils.h"

UserRating *knn_ratings(Stats *stats, User *u, uint i, uint k)
{
    return NULL;
}

double knn_predictor(Stats *stats, User *user, Movie *movie)
{
    uint k = 80;
    double tau = 0.01;
    double scale = 4;
    double offset = -5;

    double score = 0;
    double sum_weights = 0;
    UserRating *nearest_ratings;
    if (k > stats->nb_movies)
        nearest_ratings = knn_ratings(stats, user, movie->id-1, k);
    else
        nearest_ratings = user->ratings;

    for (uint i = 0; i < k; i++) {
        UserRating rating = nearest_ratings[i];
        double s = get_similarity(stats->similarity, movie->id-1, rating.movie_id-1);
        uint delta = abs(rating.date - movie->date);
        double time_factor = 1 / (1 + tau * delta);
        double weight = logistic(scale * s * time_factor + offset, 1, 0);
        score += weight * user->ratings[rating.movie_id-1].score;
        sum_weights += weight;
    }
    return score / sum_weights;
}

static double proximity(Stats *stats, uint i, uint *ids, uint m)
{
    double distance = 0;
    double sum_weights = 0;
    for (uint j = 0; j < m; j++) {
        double s = get_similarity(stats->similarity, i, ids[j]-1);
        double weight = exp(2 * s);
        distance += weight * s;
        sum_weights += weight;
    }
    return distance / sum_weights;
}

uint *knn_movies(Stats *stats, uint *ids, uint m, uint k)
{
    double a = 0.5;
    double b = 0.5;

    for (uint i = 0; i < stats->nb_movies; i++) {
        double distance = proximity(stats, i, ids, m);  // between 0 and 1
        double popularity = stats->movies[i].nb_ratings / stats->nb_users;
        double score = a * stats->movies[i].average * popularity + b * distance;
    }
    return NULL;
}
