#pragma once

#include <stdint.h>

typedef struct Rating {
    uint32_t customer_id;
    uint8_t score;
    uint16_t date;
} Rating;

typedef struct Movie {
    char *title;
    Rating *ratings;
    uint32_t nb_ratings;
    uint16_t id;
    uint16_t date;
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