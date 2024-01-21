#pragma once

#include "stats.h"
#include "parser.h"

#define PROBE_MOVIES_COUNT 16938

typedef struct Score {
    uint movie_id;
    double score;
} Score;

/**
 * @brief Predict the score of a movie for a given user.
 * @param stats The stats.
 * @param user The user.
 * @param movie_id The movie id.
*/
double knn_predictor(Stats *stats, User *user, uint movie_id);

/** 
 * @brief Get recommandations from liked movies.
 * @param stats The stats.
 * @param ids The ids of the movies liked by the user.
 * @param n The number of movies liked by the user.
 * @param k The number of recommandations.
 * @param p The percentage of personnalized recommandations.
 * @return The ids of the k recommandations.
*/
uint *knn_movies(Stats *stats, ulong *ids, uint n, uint k, double p);

/**
 * @brief Predict the score for users in the probe file.
 * @param filename The file where predictions will be written.
 * @param stats The stats.
 * @param data The data.
*/
void write_probe_predictions(char *filename, Stats *stats, UserData *data);

/**
 * @brief Calculate the RMSE of our algorithm to predict ratings, using the `probe_prediction.txt` file.
 * 
 * @param filename The file containing real scores and prediected scores.
 * @return `double` The RMSE value.
 */
double rmse_probe_calculation(char* filename);