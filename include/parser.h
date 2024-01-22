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
    bool force; /**< `-f` option: to force recompute stats. */
    const char* directory; /**< `-d` option: Directory where put file corresponding to results. */
    uint16_t limit; /**< `-l` option: prohibits taking notes into account if their date is greater than Arguments::limit. */
    uint16_t movie_id; /**< `-s` option: film than the user wants statistics for. */
    uint nb_customer_ids; /**< Length of Arguments::customer_ids. */
    ulong *customer_ids; /**< `-c` option: customers than the user wants to take into account. */
    uint nb_bad_reviewers; /**< Length of Arguments::bad_reviewers. */
    ulong *bad_reviewers; /**< `-b` option: customers than the user does not want to take into account. */
    uint min; /**< `-e` option: to take into account only customers with Arguments::min ratings at least. */
    bool time; /**< True to give the executive time of the algorithm. */
    char* likes; /**< `-r` option: file with movies titles or movies ids of liked movies. */
    uint n; /**< `-n` option: number of recommandations to give. */
    float percent; /**< `-p` option: percentage of personnalized recommandations */  
    /*@}*/
} Arguments;

/**
 * @brief Parse a string containing a list of ids.
 * @param arg List of given Arguments.
 * @param n Pointer to an  `uint` to stock the length of the ids array.
 * @return `ulong*`: An array containing parsed identifiers.
*/
ulong* parse_ids(const char *arg, uint *n);

/**
 * @brief Parse an argument given by the user.
 * @param key The key of the Arguments we are parsing.
 * @param arg List of given Arguments.
 * @param state List of options and their description in a argp_state structure.
*/
error_t parse_opt(int key, char *arg, struct argp_state *state);

/**
 * @brief Free memory allocated for Arguments.
 * @param args The Arguments structure to free.
*/
void free_args(Arguments *args);

/**
 * @brief Contains all information about a rating (movie-oriented).
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
 * @note Index of a movie in MovieData::movies corresponds to Movie::id - 1.
 */
typedef struct MovieData {
    /*@{*/
    Movie **movies; /**< Array containing movies information sorted by movies identifiers. */
    uint16_t nb_movies; /**< Number of movies. (length of MovieData::movies)*/
    /*@}*/
} MovieData;

/**
 * @brief Contain all information about a rating (user-oriented).
 */
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
 * @note Index of a user in UserData::users corresponds to User::id - 1.
 */
typedef struct UserData {
    User **users; /**< Array containing users information sorted by users identifiers. */
    uint nb_users; /**< Number of users. (length of UserData::users)*/
} UserData;

/**
 * @brief Get the customer id of a given MovieRating.
 * @param rating The given MovieRating.
 * @return The customer id.
 */
ulong get_customer_id(MovieRating rating);

/**
 * @brief Free the memory allocated for the MovieData \p data.
 * @param data The MovieData to free.
 */
void free_movie_data(MovieData *data);

/**
 * @brief Free the memory allocated for the UserData \p data.
 * @param data The UserData to free.
 */
void free_user_data(UserData *data);

/**
 * @brief Parse the file \p titles_file containing the titles of movies.
 * @param data The MovieData to complete with titles.
 * @param titles_file The file containing the titles.
 * @return `0` if the parsing was successful, `1` otherwise.
 */
int parse_titles(MovieData *data, FILE *titles_file);

/**
 * @brief Parse a file containing ratings of a movie.
 * @param movie The Movie to which the ratings will be added.
 * @param mv_file The movie file containing the ratings.
 * @return `0` if the parsing was successful, `1` otherwise.
 */
int parse_ratings(Movie *movie, FILE *mv_file);

/**
 * @brief Parse the training_set.
 * @return `MovieData*`: A pointer to the generated MovieData structure.
*/
MovieData *parse(void);

/**
 * @brief Write data structure to a binary file.
 * @param filepath The file where the MovieData will be written.
 * @param data The MovieData structure.
 */
void write_movie_data_to_file(char *filepath, MovieData *data);

/**
 * @brief Read a MovieData structure from a file.
 * @param filepath The file where the data will be read.
 * @return `MovieData*`: A pointer to the MovieData structure read.
 */
MovieData *read_movie_data_from_file(char *filepath);

/**
 * @brief Write a UserData structure to a binary file.
 * @param filepath Complete path to the file where to write.
 * @param data The UserData to write.
*/
void write_user_data_to_file(char *filepath, UserData *data);

/**
 * @brief Read a UserData structure from a file.
 * @param filepath Complete path to the file to read.
 * @return `UserData*`: The UserData structure read.
*/
UserData *read_user_data_from_file(char *filepath);

/**
 * @brief Convert a movie oriented /p data (MovieData) to a user oriented data (UserData).
 * @param data The movie oriented data.
 * @return `UserData*`: A pointer to the generated user oriented data.
 */
UserData *to_user_oriented(MovieData *data);

/**
 * @brief Parse a file containing movies liked by the user.
 * @note The array of identifiers must be freed by the caller.
 * @param filename Name of the file containing liked movies.
 * @param data DataMovie to find corresponding Movie::id.
 * @param ids Address to an array containing the identifiers of liked movies.
 * @return The number of liked movies.
 */
uint parse_likes(const char *filename, MovieData *data, ulong **ids);
