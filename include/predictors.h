#pragma once

#include "stats.h"
#include "parser.h"

#define PROBE_MOVIES_COUNT 16938

/**
 * @brief Find the \k nearest ratings of the user \p u for the movie \i using the Stats::similarity matrix.
 * @param stats The Stats used in the calculation.
 * @param u The user whose score is to be predicted.
 * @param i The movie identifier of the movie for which the score is to be predicted.
 * @param k Desired number of UserRating.
 * @return `UserRating*`: An array of the \p k nearest UserRating
 */
UserRating *knn_ratings(Stats *stats, User *u, uint i, uint k);

/**
 * @brief Predict the score of a movie for a given \p user.
 * @param stats The Stats used in the calculation.
 * @param user The user whose score is to be predicted.
 * @param movie_id The movie identifier of the movie for which the score is to be predicted.
*/
double knn_predictor(Stats *stats, User *user, uint movie_id);

/** 
 * @brief Get recommandations from liked movies.
 * @param stats The Stats used in the calculation.
 * @param ids The identifiers of the movies liked by the user.
 * @param n The number of movies liked by the user.
 * @param k The desired number of recommandations.
 * @param p The percentage of personnalized recommandations.
 * @return The ids of the k recommandations.
*/
uint *knn_movies(Stats *stats, ulong *ids, uint n, uint k, double p);

// ====================== Probe prediction =====================

/**
 * @brief Predict the score for each user in the probe file.
 * @param filename The file where predictions will be written.
 * @param stats The Stats used in the calculation.
 * @param data The UserData used for information.
*/
void write_probe_predictions(char *filename, Stats *stats, UserData *data);

/**
 * @brief Calculate the RMSE of our algorithm to predict ratings, using the `probe_prediction.txt` file.
 * @param filename The file containing real scores and prediected scores.
 * @return `double`: The RMSE value.
 */
double rmse_probe_calculation(char* filename);