#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "parser.h"

/** 
 * @brief Contains all arguments given by the user, already parsed.
*/
typedef struct Arguments {
    /*@{*/
    const char* folder; /**< Directory where put file corresponding to results. */
    uint16_t limit; /**< `-l` option: prohibits taking notes into account if their date is greater than Arguments::limit. */
    uint16_t movie_id; /**< `-s` option: film than the user wants statistics for. */
    u_int nb_customer_ids; /**< Length of Arguments::customer_ids. */
    u_long *customer_ids; /**< `-c` option: customers than the user wants to take into account. */
    u_int nb_bad_reviewers; /**< Length of Arguments::bad_reviewers. */
    u_int *bad_reviewers; /**< `-b` option: customers than the user does not want to take into account. */
    u_int min; /**< `-e` option: to take into account only customers with Arguments::min ratings at least. */
    bool time; /**< True to give the executive time of the algorithm. */
    /*@}*/
} Arguments;

/**
 * @brief Contains all stats about a movie.
 */
typedef struct MovieStats {
    double average; /**< Average of its ratings. */
    uint8_t min; /** Minimum of its ratings. */
    uint8_t max; /**< Maximum of its ratings. */
    uint32_t nb_ratings; /**< Number of ratings. */
} MovieStats;

/**
 * @brief Contains all stats about a user.
 */
typedef struct UserStats {
    int *frequency;
    double average;
    uint32_t nb_ratings;
} UserStats;

/**
 * @brief Contains all stats about all movies.
 */
typedef struct Stats {
    double **similarity;
    MovieStats *movies;
    UserStats *users;
    int nb_movies;
    int nb_users;
} Stats;

/**
 * @brief Free a stat structure.
 * @param stats Structure to free.
 */
void free_stats(Stats *stats);

/**
 * @return `True` if the customer must be taken into account.
 *         `False` otherwise.
 */
bool is_requested(Arguments *args, u_long id);

/**
 * @return `True` if the customer must be taken into account.
 *         `False` otherwise.
 */
bool not_a_bad_reviewer(Arguments *args, u_long id);

/**
 * @brief Compute logistic function.
*/
double logistic(double x, double a, double b);

/**
 * @brief Reduce the value when n is low.
 * @param alpha The shrink factor.
*/
double shrink(double value, u_int n, double alpha);

/**
 * @brief Main function to collect statistic from the binary file, respectings given arguments.
 * @note This function also make a new binary file with dessired data.
 * 
 * @param fulldata Data extracted from the binary file
 * @param args Arguments given by the user.
 * @return A `Stats*` structure containing all requested statistics.
 */
Stats *read_stats_from_data(MovieData *fulldata, Arguments *args);

/**
 * @brief Create a similarity matrix.
 * @param data The data structure.
 * @return The similarity matrix.
 */
double **create_similarity_matrix(MovieData *data);

/**
 * @brief Compute the similarity between two movies.
 * @param data The data structure.
 * @param movie1 The first movie.
 * @param movie2 The second movie.
 * @return The similarity between the two movies.
 */
double mse_correlation(MovieData *data, Movie *movie1, Movie *movie2);