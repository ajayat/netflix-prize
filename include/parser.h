#pragma once

#include <argp.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

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
    char* likes_file; /**< `-r` option: file where to get liked movies. */
    uint nb_recommandations; /**< `-n` option: number of recommandations to give. */
    /*@}*/
} Arguments;

/**
 * @brief Parse an argument given by the user.
*/
error_t parse_opt(int key, char *arg, struct argp_state *state);

/**
 * @brief Free memory allocated for Arguments.
*/
void free_args(Arguments *args);

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
    uint nb_movies; /**< Number of movies. (length of Data::movies)*/
    /*@}*/
} MovieData;

typedef struct UserRating {
    /*@{*/
    uint16_t movie_id; /**< Identifier of the movie. */
    uint16_t date; /**< Date of the rating (stocked as number of days since Epoch). */
    uint8_t score; /**< Rating given by the customer (integer from 1 to 5). */
    /*@}*/
} UserRating;

/**
 * @brief Contains all information about a user.
 */
typedef struct User {
    /*@{*/
    UserRating *ratings; /**< Array of all ratings given by the user. */
    uint32_t nb_ratings; /**< Number of ratings given by the user. */
    uint32_t id; /**< Identifier of the user. */
    /*@}*/
} User;

/**
 * @brief User data oriented structure.
 */
typedef struct UserData {
    User **users; /**< Array containing users information sorted by users identifiers. */
    uint nb_users; /**< Number of users. (length of UserData::users)*/
} UserData;

/**
 * @brief Get the customer id of a rating.
 * @param rating The rating.
 * @return The customer id.
 */
ulong get_customer_id(MovieRating rating);

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

/**
 * @brief Parse a file containing movies liked by the user.
 * 
 * @param filename Name of the file containing liked movies.
 * @param movie_data Data of movies to find corresponding identifiers.
 * @param ids Address to an array containing the identifiers of liked movies.
 * @return The number of liked movies.
 * 
 * @note The array of identifiers must be freed by the caller.
 */
uint parse_likes(const char *filename, MovieData *movie_data, uint **ids);
