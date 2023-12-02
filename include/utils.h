#pragma once

#include <stdint.h>

/**
 * @brief Get the size in byte of a file.
 * @param file The file to get the size from.
 * @return The size of the file.
 */
unsigned int get_size(FILE *file);

/**
 * @brief Get the title of a movie.
 * @param movie_id The id of the movie.
 * @return The title of the movie.
 */
const char *get_title(int movie_id);

/**
 * @brief Get the number of days between a gven date and Epoch.
 * @note Epoch is the 1st of January 1970.
 */
unsigned int days_from_epoch(unsigned int year, unsigned int month, unsigned int day);
