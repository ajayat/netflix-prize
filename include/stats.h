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
    unsigned int nb_customer_ids; /**< Length of Arguments::customer_ids. */
    unsigned long *customer_ids; /**< `-c` option: customers than the user wants to take into account. */
    unsigned int nb_bad_reviewers; /**< Length of Arguments::bad_reviewers. */
    unsigned int *bad_reviewers; /**< `-b` option: customers than the user does not want to take into account. */
    unsigned int min; /**< `-e` option: to take into account only customers with Arguments::min ratings at least. */
    bool time; /**< True to give the executive time of the algorithm. */
    /*@}*/
} Arguments;

/**
 * @brief Contains all stats about a movie.
 * 
 */
typedef struct Movie_stats {
    float average;
    uint8_t min;
    uint8_t max;
} Movie_stats;

/**
 * @brief Contains all stats about all movies.
 * 
 */
typedef struct Stats {
    Movie_stats *movies;
    int nb_movies;
} Stats;

/**
 * @brief Free a stat structure.
 * 
 * @param stats Structure to free.
 */
void free_stats(Stats *stats);

/**
 * @brief Count the number of ratings per customer in order to know if they are an elite or not.
 * 
 * @param file Binary file used to count.
 * @param elite Array with the number of ratings per customers.
 * @return `True` if all work during the process.
 * @return `False` otherwise.
 */
void count_ratings(Data *data, unsigned int *elite);

/**
 * @brief 
 * 
 * @param args 
 * @param id 
 * @return `True` if the customer must be taken into account.
 * @return `False` otherwise.
 */
bool is_requested(Arguments *args, unsigned long id);

/**
 * @brief 
 * 
 * @param args 
 * @param id 
 * @return `True` if the customer must be taken into account.
 * @return `False` otherwise.
 */
bool not_a_bad_reviewer(Arguments *args, unsigned long id);

/**
 * @brief Main function to collect statistic from the binary file, respectings given arguments.
 * @note This function also make a new binary file with dessired data.
 * 
 * @param fulldata Data extracted from the binary file
 * @param args Arguments given by the user.
 * @return A `Stats*` structure containing all requested statistics.
 */
Stats *read_stats_from_data(Data *fulladata, Arguments *args);