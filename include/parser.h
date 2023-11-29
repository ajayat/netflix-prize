#pragma once

#include <stdio.h>

typedef struct Rating {
    int user_id;
    int rating;
    int date;
} Rating;

/**
 * Return an array of movies titles.
 * 
 * @param filename The name of the file containing the titles.
 * @param length The number of movies.
 * @return An array of titles.
 * @note The array must be freed by the caller.
 */
char **parse_movies_titles(char *filename, int *length);

/**
 * Return an array of ratings.
 * 
 * @param filename The name of the file containing the ratings.
 * @param length The number of ratings.
 * @return An array of ratings.
 * @note The array must be freed by the caller.
 */
Rating *parse_movie(const char *filename, int *length);

/**
 *  Write data structure to a file.
 * 
 * @param filename The name of the output file where the data will be written.
 * @param data The data to write.
 * @param size The size of the data.
 * @note The data is usually a struct or an array.
 */
void write_to_file(const char *filename, void *data, size_t size);

/**
 * Read data structure from a file.
 * 
 * @param filename The name of the input file where the data will be read.
 * @param size The size of the data.
 * @return The data read from the file.
 * @note The data is usually a struct or an array.
 */
void *read_from_file(const char *filename, size_t *size);
