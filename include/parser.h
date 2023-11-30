#pragma once

typedef struct Rating {
    uint32_t customer_id;
    uint8_t score;
    uint16_t date;
} Rating;

typedef struct Movie {
    Rating *ratings;
    unsigned short nb_ratings;
    char *title;
} Movie;

/**
 * Return an array of movies titles.
 * 
 * @param filename The name of the file containing the titles.
 * @param nb_movies The number of movies.
 * @return An array of titles.
 * @note The array must be freed by the caller.
 */
char **parse_movies_titles(char *filename, unsigned short *nb_movies);

/**
 * Return an array of ratings.
 * 
 * @param filename The name of the file containing the ratings.
 * @param nb_ratings The number of ratings.
 * @return An array of ratings.
 * @note The array must be freed by the caller.
 */
Rating *parse_movie(const char *filename, unsigned int *nb_ratings);

/**
 * Return a parsed structure of movies
 * 
 * @param data_path The name of the path containing all data
 */
Movie *parse(const char data_path);

/**
 *  Write data structure to a file.
 * 
 * @param filename The name of the output file where the data will be written.
 * @param data The data to write.
 * @param size The size of the data.
 * @note The data is usually a struct or an array.
 */
void write_to_file(const char *filename, Movie *data, size_t size);

/**
 * Read data structure from a file.
 * 
 * @param filename The name of the input file where the data will be read.
 * @param size The size of the data.
 * @return The data read from the file.
 * @note The data is usually a struct or an array.
 */
Movie *read_from_file(const char *filename, size_t *size);