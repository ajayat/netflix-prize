#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "parser.h"
#include "predictors.h"
#include "stats.h"

UserRating *knn_ratings(Stats *stats, User *u, u_int i, u_int k)
{
    return NULL;
}

double knn_predictor(Stats *stats, User *user, Movie *movie)
{
    u_int k = 80;
    double tau = 0.01;
    double scale = 4;
    double offset = -5;

    double score = 0;
    double sum_weights = 0;
    UserRating *nearest_ratings;
    if (k > stats->nb_movies)
        nearest_ratings = knn_ratings(stats, user, movie->id, k);
    else
        nearest_ratings = user->ratings;

    for (u_int i = 0; i < k; i++) {
        UserRating rating = nearest_ratings[i];
        double s = stats->similarity[movie->id][rating.movie_id];
        u_int delta = abs(rating.date - movie->date);
        double time_factor = 1 / (1 + tau * delta);
        double weight = logistic(scale * s * time_factor + offset, 1, 0);
        score += weight * user->ratings[rating.movie_id].score;
        sum_weights += weight;
    }
    return score / sum_weights;
}

static double proximity(Stats *stats, u_int i, u_int *ids, u_int m)
{
    double distance = 0;
    double sum_weights = 0;
    for (u_int j = 0; j < m; j++) {
        double s = stats->similarity[i][ids[j]];
        double weight = exp(2 * s);
        distance += weight * s;
        sum_weights += weight;
    }
    return distance / sum_weights;
}

u_int *knn_movies(Stats *stats, u_int *ids, u_int m, u_int k)
{
    double a = 0;
    double b = 1;

    for (u_int i = 0; i < stats->nb_movies; i++) {
        double distance = proximity(stats, i, ids, m);
        double popularity = stats->movies[i].nb_ratings / stats->nb_users;
        double score = a * stats->movies[i].average * popularity + b * distance;
    }
    return NULL;
}
