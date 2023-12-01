#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "utils.h"

#define LENGTH_MAX_TITLE 100
#define MAX_NUMBER_MOVIES 17771
#define MAX_NUMBER_RATINGS 250000

unsigned int parse_movie_titles(Movie *movies, FILE *titles_file)
{
    unsigned short id;
    unsigned int year;
    // Allocation of the title buffer
    char title[LENGTH_MAX_TITLE];
    unsigned int nb_movies = 0;
    int r;
    while ((r = fscanf(titles_file, "%hu%*c%4d%*c%99[^\n]\n", &id, &year, title)) != EOF) {
        if (r != 3)
            fscanf(titles_file, "%*[^\n]\n");  // Skip the line
        movies[nb_movies].id = (uint16_t)id;
        movies[nb_movies].title = strdup(title);
        movies[nb_movies].date = (uint16_t)days_from_epoch(year, 1, 1);
        nb_movies++;
    }
    return nb_movies;
}

unsigned int parse_movie(Rating *ratings, FILE *mv_file)
{
    unsigned short score;
    unsigned int customer_id;
    unsigned int y, m, d;
    unsigned int nb_ratings = 0;

    int r;
    while ((r = fscanf(mv_file, "%u%*c%1hu[1-5]%*c%4u%*c%2u%*c%2u%*[^\n]\n",
                       &customer_id, &score, &y, &m, &d)) != EOF) {
        if (r != 5)
            fscanf(mv_file, "%*[^\n]\n");  // Skip the line

        ratings[nb_ratings].customer_id = (uint32_t)customer_id;
        ratings[nb_ratings].score = (uint8_t)score;
        ratings[nb_ratings].date = (uint16_t)days_from_epoch(y, m, d);
        nb_ratings++;
    }
    return nb_ratings;
}

void write_to_file(FILE *bin_file)
{
    FILE *titles_file = fopen("data/movie_titles.txt", "r");
    if (titles_file == NULL) {
        fprintf(stderr, "Error: could not open file %s\n", "data/movie_titles.txt");
        exit(EXIT_FAILURE);
    }
    Movie movies[MAX_NUMBER_MOVIES] = {0};
    unsigned int nb_movies = parse_movie_titles(movies, titles_file);
    fclose(titles_file);

    fwrite(&nb_movies, sizeof(uint32_t), 1, bin_file);

    for (unsigned int i = 0; i < nb_movies; i++)
    {
        printf("Processing movie %d\n", i);
        char mv_filename[50];
        sprintf(mv_filename, "data/training_set/mv_%07u.txt", movies[i].id);
        FILE *mv_file = fopen(mv_filename, "r");
        if (mv_file == NULL) {
            fprintf(stderr, "Error: could not open file %s\n", mv_filename);
            for (unsigned int j = 0; j < nb_movies; j++)
                free(movies[j].title);
            exit(EXIT_FAILURE);
        }
        Rating ratings[MAX_NUMBER_RATINGS];
        uint8_t title_length = strlen(movies[i].title) + 1;
        unsigned int nb_ratings = parse_movie(ratings, mv_file);

        fwrite(&movies[i].id, sizeof(uint16_t), 1, bin_file);
        fwrite(&title_length, sizeof(uint8_t), 1, bin_file);
        fwrite(movies[i].title, sizeof(char), title_length, bin_file);
        fwrite(&movies[i].date, sizeof(uint16_t), 1, bin_file);
        fwrite(&nb_ratings, sizeof(uint32_t), 1, bin_file);
        fwrite(ratings, sizeof(Rating), nb_ratings, bin_file);

        fclose(mv_file);
    }
}

Movie *read_from_file(FILE* bin_file)
{
    uint32_t nb_movies;
    fread(&nb_movies, sizeof(uint32_t), 1, bin_file);
    Movie *movies = malloc(nb_movies * sizeof(Movie));

    for (unsigned int i = 0; i < nb_movies; i++)
    {
        fread(&movies[i].id, sizeof(uint16_t), 1, bin_file);
        uint8_t title_length;
        fread(&title_length, sizeof(uint8_t), 1, bin_file);
        movies[i].title = malloc(title_length * sizeof(char));
        fread(movies[i].title, sizeof(char), title_length, bin_file);
        fread(&movies[i].date, sizeof(uint16_t), 1, bin_file);
        uint32_t nb_ratings;
        fread(&nb_ratings, sizeof(uint32_t), 1, bin_file);
        movies[i].nb_ratings = nb_ratings;
        movies[i].ratings = malloc(nb_ratings * sizeof(Rating));
        fread(movies[i].ratings, sizeof(Rating), nb_ratings, bin_file);
    }
    return movies;
}