#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "utils.h"

#define LENGTH_MAX_TITLE 120
#define MAX_NUMBER_MOVIES 17771
#define MAX_NUMBER_RATINGS 250000

unsigned long get_customer_id(Rating rating)
{
    return (((uint32_t)rating.customer_id_msb) << 8) + (uint32_t)rating.customer_id_lsb;
}

void free_data(Data *data)
{
    for (unsigned int i = 0; i < data->nb_movies; i++) {
        free(data->movies[i]->title);
        free(data->movies[i]->ratings);
        free(data->movies[i]);
    }
    free(data->movies);
    free(data);
}

int parse_titles(Data *data, FILE *titles_file)
{
    unsigned short id;
    char year[5];
    char title[LENGTH_MAX_TITLE];
    unsigned int m = 0;

    while (fscanf(titles_file, "%hu%*c%4c%*c%119[^\n]\n", &id, year, title) != EOF) {
        data->movies[m] = calloc(1, sizeof(Movie));
        data->movies[m]->id = (uint16_t)id;
        data->movies[m]->title = strdup(title);
        data->movies[m]->date = days_from_epoch(strtoul(year, NULL, 10), 1, 1);
        m++;
    }
    data->nb_movies = m;
    return 0;
}

int parse_ratings(Movie *movie, FILE *mv_file)
{
    Rating *ratings = malloc(MAX_NUMBER_RATINGS * sizeof(Rating));
    if (ratings == NULL) {
        fprintf(stderr, "Error: could not allocate memory for ratings\n");
        return 1;
    }
    unsigned int size = get_size(mv_file);
    char buffer[size+1];
    fread(buffer, sizeof(char), size, mv_file);

    unsigned int r = 0;  // Number of ratings
    unsigned int id;  // Customer id
    unsigned int y, m, d;
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
        ratings = realloc(ratings, r * sizeof(Rating));
    movie->nb_ratings = r;
    movie->ratings = ratings;
    return 0;
}

Data *write_to_file(FILE *file)
{
    FILE *titles_file = fopen("data/movie_titles.txt", "r");
    if (titles_file == NULL) {
        fprintf(stderr, "Error: could not open file %s\n", "data/movie_titles.txt");
        exit(EXIT_FAILURE);
    }
    // Allocate memory for data
    Data *data = malloc(sizeof(Data));
    data->movies = malloc(MAX_NUMBER_MOVIES * sizeof(Movie*));
    parse_titles(data, titles_file);
    fclose(titles_file);
    // Write the number of movies
    fwrite(&data->nb_movies, sizeof(uint32_t), 1, file);

    for (unsigned int i = 0; i < data->nb_movies; i++)
    {
        printf("Processing movie %d\n", i);  // Information for the user
        Movie *movie = data->movies[i];
        // Open the movie file
        char mv_filename[40];
        snprintf(mv_filename, 40, "data/training_set/mv_%07u.txt", movie->id);
        FILE *mv_file = fopen(mv_filename, "r");
        if (mv_file == NULL) {
            fprintf(stderr, "Error: could not open file %s\n", mv_filename);
            continue; // Ignore this movie
        }
        parse_ratings(movie, mv_file);
        uint8_t title_length = strlen(movie->title) + 1;
        // Write informations about the movie
        fwrite(&movie->id, sizeof(uint16_t), 1, file);
        fwrite(&title_length, sizeof(uint8_t), 1, file);
        fwrite(movie->title, sizeof(char), title_length, file);
        fwrite(&movie->date, sizeof(uint16_t), 1, file);
        fwrite(&movie->nb_ratings, sizeof(uint32_t), 1, file);
        fwrite(movie->ratings, sizeof(Rating), movie->nb_ratings, file);
        fclose(mv_file);
    }
    return data;
}

Data *read_from_file(FILE* file)
{
    // Allocate memory for data
    Data *data = malloc(sizeof(Data));
    fread(&data->nb_movies, sizeof(uint32_t), 1, file);
    data->movies = malloc(data->nb_movies * sizeof(Movie*));

    for (unsigned int i = 0; i < data->nb_movies; i++)
    {
        Movie *movie = malloc(sizeof(Movie));
        data->movies[i] = movie;
        // Read informations about the movie
        fread(&movie->id, sizeof(uint16_t), 1, file);
        uint8_t title_length;
        fread(&title_length, sizeof(uint8_t), 1, file);
        movie->title = malloc(title_length * sizeof(char));
        fread(movie->title, sizeof(char), title_length, file);
        fread(&movie->date, sizeof(uint16_t), 1, file);
        fread(&movie->nb_ratings, sizeof(uint32_t), 1, file);
        movie->ratings = malloc(movie->nb_ratings * sizeof(Rating));
        fread(movie->ratings, sizeof(Rating), movie->nb_ratings, file);
    }
    return data;
}