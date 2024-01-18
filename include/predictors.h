#pragma once

#include "stats.h"
#include "parser.h"

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
 * @return The ids of the k recommandations.
*/
uint *knn_movies(Stats *stats, uint *ids, uint n, uint k);

/**
 * @brief Parse the probe.txt file and return a file with the real ratings given by users in the training set.
 * 
 * @param filename Name of the probe file.
 * @param stats All statistics about all movies and all users.
 * @param movie_data Data of all movies, to find ratings.
 * @param user_data Data of all users, to use `knn_predictor`.
 */
void parse_probe(char *filename, Stats *stats, MovieData *movie_data, UserData *user_data);