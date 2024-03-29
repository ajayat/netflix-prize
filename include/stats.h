#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "parser.h"
#include "hashmap.h"

typedef unsigned int uint;
typedef unsigned long ulong;

/**
 * @brief Contains all stats about a movie.
 */
typedef struct MovieStats {
    uint32_t nb_ratings; /**< Number of ratings. */
    uint16_t date; /**< Date in number of days. */
    float average; /**< Average of its ratings. */
    uint8_t min; /** Minimum of its ratings. */
    uint8_t max; /**< Maximum of its ratings. */
} MovieStats;

/**
 * @brief Contains all stats about a user.
 */
typedef struct UserStats {
    Hashmap *frequency; /**< Number of ratings per day. */
    double average; /**< Average of his ratings. */
    uint32_t nb_ratings; /**< Number of all his ratings. */
} UserStats;

/**
 * @brief Contains all stats about all movies.
 */
typedef struct Stats {
    float *similarity;
    MovieStats *movies;
    UserStats *users;
    uint nb_movies;
    uint nb_users;
} Stats;

// ========== Functions to build the similarity matrix ==========

/**
 * @brief Compute logistic function.
 */
double logistic(double x, double a, double b);

/**
 * @brief Reduce the value when n is low.
 * @param alpha The shrink factor.
 */
double shrink(double value, uint n, double alpha);

/**
 * @brief Create a similarity matrix.
 * @param data The data structure.
 * @return The similarity matrix.
 */
float *create_similarity_matrix(MovieData *data);

/**
 * @brief Write the similarity matrix in a binary file.
 * 
 * @param stats Stats containing the similarity matrix.
 * @param filename Name of the binary file.
 */
void write_similarity_matrix(Stats *stats, char *filename);

/**
 * @brief Read the similarity matrix from a binary file.
 * 
 * @param filename Name of the binary file.
 * @return The similarity matrix.
 */
float *read_similarity_matrix(char *filename);


// ========== Functions to calculate statistics from the fulldata ==========

/**
 * @brief Free a stat structure.
 * @param stats Structure to free.
 */
void free_stats(Stats *stats);

/**
 * @brief A succession of conditions to determinate if the given rating must be ignored or not.
 * 
 * @param args Arguments to determinate if the rating must be ignored.
 * @param rating Rating to test.
 * @param c_id Customer id of the rating.
 * @param c_nb_ratings Number of ratings of the customer.
 */
bool ignored_rating(Arguments *args, MovieRating rating, ulong c_id, uint c_nb_ratings);

/**
 * @brief Main function to collect statistic from the binary file, respectings given arguments.
 * @note This function also make a new binary file with desired data.
 * 
 * @param data Data extracted from the binary file
 * @param args Arguments given by the user.
 * @return A `Stats*` structure containing all requested statistics.
 */
Stats *read_stats_from_data(MovieData *data, Arguments *args);

/**
 * @brief Read the stats from a binary file.
*/
Stats *read_stats_from_file(char *filepath);

/**
 * @brief Write the stats in a binary file.
 * 
 * @note The similarity matrix is not written.
*/
void write_stats_to_file(Stats *stats, char *filepath);
