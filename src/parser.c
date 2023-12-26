#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "parser.h"
#include "utils.h"

#define EPOCH_YEAR 1889
#define LENGTH_MAX_TITLE 120
#define MAX_NUMBER_MOVIES 17771
#define MAX_NUMBER_RATINGS 250000
#define MAX_USER_ID 2649430

u_long get_customer_id(MovieRating rating)
{
    return (((uint32_t)rating.customer_id_msb) << 8) + (uint32_t)rating.customer_id_lsb;
}

void free_movie_data(MovieData *data)
{
    for (u_int i = 0; i < data->nb_movies; i++) {
        free(data->movies[i]->title);
        free(data->movies[i]->ratings);
        free(data->movies[i]);
    }
    free(data->movies);
    free(data);
}

void free_user_data(UserData *data)
{
    for (u_int i = 0; i < data->nb_users; i++) {
        if (data->users[i] != NULL) {
            free(data->users[i]->ratings);
            free(data->users[i]);
        }
    }
    free(data->users);
    free(data);
}

int parse_titles(MovieData *data, FILE *titles_file)
{
    unsigned short id;
    u_int year_int;
    char year[5];
    char title[LENGTH_MAX_TITLE];
    u_int m = 0;

    while (fscanf(titles_file, "%hu%*c%4c%*c%119[^\n]\n", &id, year, title) != EOF) {
        data->movies[m] = calloc(1, sizeof(Movie));
        data->movies[m]->id = (uint16_t)id;
        data->movies[m]->title = strdup(title);
        if ((year_int = strtoul(year, NULL, 10)) == 0)
            year_int = EPOCH_YEAR;
        data->movies[m]->date = days_from_epoch(year_int, 1, 1);
        m++;
    }
    data->nb_movies = m;
    return 0;
}

int parse_ratings(Movie *movie, FILE *mv_file)
{
    MovieRating *ratings = malloc(MAX_NUMBER_RATINGS * sizeof(MovieRating));
    movie->ratings = ratings;
    if (ratings == NULL) {
        fprintf(stderr, "Error: could not allocate memory for ratings\n");
        return 1;
    }
    u_int size = get_size(mv_file);
    char buffer[size+1];
    if (fread(buffer, sizeof(char), size, mv_file) == 0)
        return 1;

    u_int r = 0;  // Number of ratings
    u_int id;  // Customer id
    u_int y, m, d;
    char *begin = NULL, *end = NULL;
    strtoul(buffer, &begin, 10);  // Skip the first line

    while ((id = strtoull(begin + 1, &end, 10)) != 0) 
    {
        ratings[r].customer_id_msb = (uint16_t)(id >> 8);
        ratings[r].customer_id_lsb = (uint8_t)(id & 255);
        ratings[r].score = (uint8_t)atoi(end + 1);
        y = strtoul(end + 3, &end, 10);
        m = strtoul(end + 1, &end, 10);
        d = strtoul(end + 1, &end, 10);
        ratings[r].date = (uint16_t)days_from_epoch(y, m, d);
        begin = end;
        r++;
    }
    if (r > 0)
        movie->ratings = realloc(ratings, r * sizeof(MovieRating));
    movie->nb_ratings = r;
    return 0;
}

MovieData *parse(void)
{
    FILE *titles_file = fopen("data/movie_titles.txt", "r");
    if (titles_file == NULL) {
        fprintf(stderr, "Error: could not open file %s\n", "data/movie_titles.txt");
        exit(EXIT_FAILURE);
    }
    // Allocate memory for data
    MovieData *data = malloc(sizeof(MovieData));
    data->movies = malloc(MAX_NUMBER_MOVIES * sizeof(Movie*));
    parse_titles(data, titles_file);
    fclose(titles_file);

    for (u_int i = 0; i < data->nb_movies; i++)
    {
        Movie *movie = data->movies[i];
        // printf("Processing movie %d\n", movie->id);  // Information for the user
        // Open the movie file
        char mv_filename[40];
        snprintf(mv_filename, 40, "data/training_set/mv_%07u.txt", movie->id);
        FILE *mv_file = fopen(mv_filename, "r");
        if (mv_file == NULL) {
            fprintf(stderr, "Error: could not open file %s\n", mv_filename);
            continue; // Ignore this movie
        }
        parse_ratings(movie, mv_file);
    }
    return data;
}

void write_to_file(FILE *file, MovieData *data)
{
    // Write the number of movies
    fwrite(&data->nb_movies, sizeof(uint32_t), 1, file);

    for (u_int i = 0; i < data->nb_movies; i++)
    {
        Movie *movie = data->movies[i];
        uint8_t title_length = strlen(movie->title) + 1;
        // Write informations about the movie
        fwrite(&movie->id, sizeof(uint16_t), 1, file);
        fwrite(&title_length, sizeof(uint8_t), 1, file);
        fwrite(movie->title, sizeof(char), title_length, file);
        fwrite(&movie->date, sizeof(uint16_t), 1, file);
        fwrite(&movie->nb_ratings, sizeof(uint32_t), 1, file);
        fwrite(movie->ratings, sizeof(MovieRating), movie->nb_ratings, file);
    }
}

MovieData *read_from_file(FILE* file)
{
    // Allocate memory for data
    MovieData *data = malloc(sizeof(MovieData));
    data->nb_movies = 0;
    u_int nb_movies;
    if (!fread(&nb_movies, sizeof(uint32_t), 1, file)) {
        free(data);
        return NULL;
    }
    data->movies = malloc(nb_movies * sizeof(Movie*));
    for (u_int i = 0; i < nb_movies; i++)
    {
        Movie *movie = calloc(1, sizeof(Movie));
        data->movies[i] = movie;
        // Read informations about the movie
        uint8_t title_length;
        if (!fread(&movie->id, sizeof(uint16_t), 1, file) ||
            !fread(&title_length, sizeof(uint8_t), 1, file))
            goto read_error;
        movie->title = malloc(title_length * sizeof(char));
        if (fread(movie->title, sizeof(char), title_length, file) != title_length ||
            !fread(&movie->date, sizeof(uint16_t), 1, file) ||
            !fread(&movie->nb_ratings, sizeof(uint32_t), 1, file))
            goto read_error;
        movie->ratings = malloc(movie->nb_ratings * sizeof(MovieRating));
        if (!fread(movie->ratings, sizeof(MovieRating), movie->nb_ratings, file))
            goto read_error;
        data->nb_movies++;
    }
    return data;

read_error:
    free_movie_data(data);
    return NULL;
}

static bool is_power_of_two(u_long x)
{
    return (x != 0) && ((x & (x - 1)) == 0);
}

UserData *to_user_oriented(MovieData *data)
{
    UserData *user_data = malloc(sizeof(UserData));
    user_data->nb_users = 0;
    user_data->users = calloc(MAX_USER_ID, sizeof(User*));

    for (u_int i = 0; i < data->nb_movies; i++) 
    {
        Movie *movie = data->movies[i];
        for (u_int r = 0; r < movie->nb_ratings; r++) 
        {
            MovieRating rating = movie->ratings[r];
            u_long id = get_customer_id(rating);
            if (user_data->users[id] == NULL) {
                User *user = user_data->users[id] = malloc(sizeof(User));
                user->id = id;
                user->nb_ratings = 0;
                user->ratings = malloc(sizeof(UserRating));
                user_data->nb_users++;
            }
            User *user = user_data->users[id];
            if (is_power_of_two(user->nb_ratings))
                user->ratings = realloc(user->ratings, 
                                        2 * user->nb_ratings * sizeof(UserRating));
            // Add the rating to the user
            user->ratings[user->nb_ratings].movie_id = movie->id;
            user->ratings[user->nb_ratings].date = rating.date;
            user->ratings[user->nb_ratings].score = rating.score;
            user->nb_ratings++;
        }
    }
    return user_data;
}
