#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "parser.h"
#include "hashmap.h"

typedef unsigned int uint;
typedef unsigned long ulong;

/** 
 * @brief Contains all arguments given by the user, already parsed.
*/
typedef struct Arguments {
    /*@{*/
    const char* folder; /**< Directory where put file corresponding to results. */
    uint16_t limit; /**< `-l` option: prohibits taking notes into account if their date is greater than Arguments::limit. */
    uint16_t movie_id; /**< `-s` option: film than the user wants statistics for. */
    uint nb_customer_ids; /**< Length of Arguments::customer_ids. */
    ulong *customer_ids; /**< `-c` option: customers than the user wants to take into account. */
    uint nb_bad_reviewers; /**< Length of Arguments::bad_reviewers. */
    ulong *bad_reviewers; /**< `-b` option: customers than the user does not want to take into account. */
    uint min; /**< `-e` option: to take into account only customers with Arguments::min ratings at least. */
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
 * @brief Compute the similarity between two movies.
 * @param data The data structure.
 * @param movie1 The first movie.
 * @param movie2 The second movie.
 * @return The similarity between the two movies.
 */
double mse_correlation(Movie *movie1, Movie *movie2, Hashmap *ratings);

/**
 * @brief Create a similarity matrix.
 * @param data The data structure.
 * @return The similarity matrix.
 */
float *create_similarity_matrix(MovieData *data);

/**
 * @brief Write the similarity matrix in a csv file.
 * 
 * @param stats Stats containing the similarity matrix.
 * @param filename Name of the csv fiile.
 */
void write_similarity_matrix_to_csv(Stats *stats, char *filename);

// ========== Functions to calculate statistics from the fulldata ==========

/**
 * @brief Free a stat structure.
 * @param stats Structure to free.
 */
void free_stats(Stats *stats);

/**
 * @return `True` if the customer `id` must be taken into account.
 *         `False` otherwise.
 */
bool is_requested(Arguments *args, ulong id);

/**
 * @return `True` if the customer `id` must not be taken into account.
 *         `False` otherwise.
 */
bool is_a_bad_reviewer(Arguments *args, ulong id);

/**
 * @brief A succession of conditions to determinate if the given rating must be ignored or not.
 * 
 * @param args Arguments to determinate if the rating must be ignored.
 * @param user_data Data of all user. Quick access to the number of ratings per user.
 * @param movie_src Movie rated.
 * @param c_id id of the customer author of the rating.
 * @param r index of the rating in the `movie_src->ratings` array.
 * @return `True` if the rating must be ignored.
 * @return `False` otherwise.
 */
bool ignored_rating(Arguments* args, UserData* user_data, Movie* movie_src, ulong c_id, uint r);

/**
 * @brief Calculate all statistics for all movies, respecting arguments.
 * 
 * @param stats 
 * @param args 
 * @param movie_data 
 * @param user_data 
 * @return The partial data
 */
MovieData *calculate_movies_stats(Stats* stats, Arguments* args, MovieData* movie_data, UserData* user_data);

/**
 * @brief Calculate all statistics for all requested user, respecting arguments.
 * 
 * @param stats 
 * @param args 
 * @param user_data 
 */
void calculate_users_stats(Stats* stats, Arguments* args, UserData* user_data);

/**
 * @brief Create a text file containing stats for the movie given by `-s` option.
 * 
 * @param stats Stats of all movies.
 * @param args Arguments to know which movie need to be convert.
 */
void one_movie_stats(Stats* stats, Arguments* args);

/**
 * @brief Main function to collect statistic from the binary file, respectings given arguments.
 * @note This function also make a new binary file with desired data.
 * 
 * @param movie_data Data extracted from the binary file
 * @param user_data User oriented data
 * @param args Arguments given by the user.
 * @return A `Stats*` structure containing all requested statistics.
 */
Stats *read_stats_from_data(MovieData *movie_data, UserData *user_data, Arguments *args);


