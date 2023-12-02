#pragma once

#include <stdio.h>
#include <stdint.h>

/**
 * @brief Contains all information about a rating.
 */
typedef struct Rating {
    /*@{*/
    uint16_t customer_id_msb; /**< The 16 most significant bits of the customer identifier. */
    uint8_t customer_id_lsb; /**< The 8 less significant bits of the customer identifier. */
    uint8_t score; /**< Rating given by the customer (integer from 1 to 5). */
    uint16_t date; /**< Date of the rating (stocked as number of days since Epoch). */
    /*@}*/
} Rating;

/**
 * @brief Contains all information about a movie.
 */
typedef struct Movie {
    /*@{*/
    char *title; /**< Title of the movie. */
    Rating *ratings; /**< Table of all ratings for this movie. */
    uint32_t nb_ratings; /**< Number of ratings for thie movie. */
    uint16_t id; /**< Identifier of the movie. */
    uint16_t date; /**< Date of the movie publication. */
    /*@}*/
} Movie;

/**
 * @brief Parse a file containing the titles of movies.
 * @param mv_titles The array of titles.
 * @param titles_file The file containing the titles.
 * @return The number of movies read.
 */
unsigned int parse_movie_titles(Movie *movies, FILE *titles_file);

/**
 * @brief Parse a file containing ratings of a movie.
 * @param ratings The array of ratings.
 * @param mv_file The movie file containing the ratings.
 * @return The number of ratings read.
 */
unsigned int parse_movie(Rating *ratings, FILE *mv_file);

/**
 * @brief Write data structure to a binary file.
 * @param bin_file The file where the data will be written.
 */
void write_to_file(FILE *bin_file);

/**
 * @brief Read data structure from a file.
 * @param bin_file The file where the data will be read.
 * @return The data read from the file.
 */
Movie *read_from_file(FILE* bin_file);