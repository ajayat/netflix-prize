#pragma once

#include "stats.h"
#include "parser.h"

typedef struct Score {
    uint movie_id;
    double score;
} Score;

/**
 * @brief Predict the score of a movie for a given user.
*/
double knn_predictor(Stats *stats, User *user, Movie *movie);

/** 
 * @brief Get recommandations from liked movies.
 * @param stats The stats.
 * @param ids The ids of the movies liked by the user.
 * @param n The number of movies liked by the user.
 * @param k The number of recommandations.
 * @return The ids of the k recommandations.
*/
uint *knn_movies(Stats *stats, uint *ids, uint n, uint k);