#pragma once

#include <stdio.h>
#include <stdint.h>

typedef unsigned int u_int;
typedef unsigned long u_long;

/**
 * @brief Contains all information about a rating.
 */
typedef struct MovieRating {
    /*@{*/
    uint16_t customer_id_msb; /**< The 16 most significant bits of the customer identifier. */
    uint8_t customer_id_lsb; /**< The 8 less significant bits of the customer identifier. */
    uint8_t score; /**< Rating given by the customer (integer from 1 to 5). */
    uint16_t date; /**< Date of the rating (stocked as number of days since Epoch). */
    /*@}*/
} MovieRating;

/**
 * @brief Contains all information about a movie.
 */
typedef struct Movie {
    /*@{*/
    char *title; /**< Title of the movie. */
    MovieRating *ratings; /**< Array of all ratings for this movie. */
    uint32_t nb_ratings; /**< Number of ratings for thie movie. (length of Movie::ratings) */
    uint16_t id; /**< Identifier of the movie. */
    uint16_t date; /**< Date of the movie publication. */
    /*@}*/
} Movie;

/**
 * @brief Contains all information contained in the training_set.
 */
typedef struct MovieData {
    /*@{*/
    Movie **movies; /**< Array containing movies information sorted by movies identifiers. */
    u_int nb_movies; /**< Number of movies. (length of Data::movies)*/
    /*@}*/
} MovieData;

typedef struct UserRating {
    /*@{*/
    uint16_t movie_id; /**< Identifier of the movie. */
    uint8_t score; /**< Rating given by the customer (integer from 1 to 5). */
    uint16_t date; /**< Date of the rating (stocked as number of days since Epoch). */
    /*@}*/
} UserRating;

/**
 * @brief Contains all information about a user.
 */
typedef struct User {
    /*@{*/
    uint16_t id; /**< Identifier of the user. */
    uint32_t nb_ratings; /**< Number of ratings given by the user. */
    UserRating *ratings; /**< Array of all ratings given by the user. */
    /*@}*/
} User;

/**
 * @brief User data oriented structure.
 */
typedef struct UserData {
    User **users; /**< Array containing users information sorted by users identifiers. */
    u_int nb_users; /**< Number of users. (length of UserData::users)*/
} UserData;

/**
 * @brief Get the customer id of a rating.
 * @param rating The rating.
 * @return The customer id.
 */
u_long get_customer_id(MovieRating rating);

/**
 * @brief Free the memory allocated for the movies.
 * @param data The data to free.
 */
void free_movie_data(MovieData *data);

void free_user_data(UserData *data);

/**
 * @brief Parse a file containing the titles of movies.
 * @param movies The movies array.
 * @param titles_file The file containing the titles.
 * @return 0 if the parsing was successful, 1 otherwise.
 */
int parse_titles(MovieData *data, FILE *titles_file);

/**
 * @brief Parse a file containing ratings of a movie.
 * @param movie The movie to which the ratings will be added.
 * @param mv_file The movie file containing the ratings.
 * @return 0 if the parsing was successful, 1 otherwise.
 */
int parse_ratings(Movie *movie, FILE *mv_file);

/**
 * @brief Parse the training_set.
 * @return The data structure.
*/
MovieData *parse(void);
/**
 * @brief Write data structure to a binary file.
 * @param file The file where the data will be written.
 * @param data The data structure.
 */
void write_to_file(FILE *file, MovieData *data);

/**
 * @brief Read data structure from a file.
 * @param file The file where the data will be read.
 * @return The data structure.
 */
MovieData *read_from_file(FILE* file);

/**
 * @brief Convert a movie oriented data to a user oriented data.
 * @param data The movie oriented data.
 * @return The user oriented data.
 */
UserData *to_user_oriented(MovieData *data);