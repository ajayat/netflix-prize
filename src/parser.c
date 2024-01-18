#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <argp.h>

#include "parser.h"
#include "utils.h"

#define LENGTH_MAX_TITLE 120
#define MAX_NUMBER_MOVIES 17771
#define MAX_NUMBER_RATINGS 250000
#define MAX_USER_ID 2649430

// ========== Functions to parse arguments==========

void free_args(Arguments *args)
{
    if (args->nb_customer_ids > 0)
        free(args->customer_ids);
    if (args->nb_bad_reviewers > 0)
        free(args->bad_reviewers);
}

ulong* parse_ids(char *arg, uint *n)
{
    char* end = NULL;
    ulong* ids = calloc(1, sizeof(ulong));
    ulong id;
    while ((id = strtoul(arg, &end, 10)) != 0) {
        if (is_power_of_two(*n))
            ids = realloc(ids, 2 * (*n) * sizeof(ulong));

        ids[(*n)++] = id;
        arg = end + 1;
    }
    return ids;
}

error_t parse_opt(int key, char *arg, struct argp_state *state)
{
    Arguments *args = state->input;
    switch (key) {
    case 'f':
        args->folder = arg;
        return 0;
    case 'l':
        args->limit = strtoul(arg, NULL, 10);
        return 0;
    case 's':
        args->movie_id = strtoul(arg, NULL, 10);
        return 0;
    case 'c':
        args->customer_ids = parse_ids(arg, &args->nb_customer_ids);
        return 0;
    case 'b':
        args->bad_reviewers = parse_ids(arg, &args->nb_bad_reviewers);
        return 0;
    case 'e':
        args->min = strtoul(arg, NULL, 10);
        return 0;
    case 't':
        args->time = true;
        return 0;
    case 'r':
        args->likes_file = arg;
        return 0;
    case 'n':
        args->nb_recommandations = strtoul(arg, NULL, 10);
        return 0;
    case ARGP_KEY_ARG:
        return 0;
    default:
        return ARGP_ERR_UNKNOWN;
    }
}

// ========== Functions to parse the data ==========

ulong get_customer_id(MovieRating rating)
{
    return (((uint32_t)rating.customer_id_msb) << 8) + (uint32_t)rating.customer_id_lsb;
}

void free_movie_data(MovieData *data)
{
    for (uint i = 0; i < data->nb_movies; i++) {
        free(data->movies[i]->title);
        free(data->movies[i]->ratings);
        free(data->movies[i]);
    }
    free(data->movies);
    free(data);
}

void free_user_data(UserData *data)
{
    for (uint i = 0; i < MAX_USER_ID; i++) {
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
    char year[5];
    char title[LENGTH_MAX_TITLE];
    uint16_t m = 0;

    while (fscanf(titles_file, "%hu%*c%4c%*c%119[^\n]\n", &id, year, title) != EOF) {
        data->movies[m] = calloc(1, sizeof(Movie));
        data->movies[m]->id = (uint16_t)id;
        data->movies[m]->title = strdup(title);
        data->movies[m]->date = strtoul(year, NULL, 10);
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
    uint size = get_size(mv_file);
    char buffer[size+1];
    if (fread(buffer, sizeof(char), size, mv_file) == 0)
        return 1;

    uint r = 0;  // Number of ratings
    ulong id;  // Customer id
    uint y, m, d;
    char *begin = NULL, *end = NULL;
    strtoul(buffer, &begin, 10);  // Skip the first line

    while ((id = strtoul(begin + 1, &end, 10)) != 0) 
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

    for (uint i = 0; i < data->nb_movies; i++)
    {
        Movie *movie = data->movies[i];
        printf("\n\033[A\033[2K");  // Clear the line
        printf("Processing movie %u/%u", movie->id, data->nb_movies);
        // Open the movie file
        char mv_filename[40];
        snprintf(mv_filename, 40, "data/training_set/mv_%07u.txt", movie->id);
        FILE *mv_file = fopen(mv_filename, "r");
        if (mv_file == NULL) {
            fprintf(stderr, "Error: could not open file %s\n", mv_filename);
            continue; // Ignore this movie
        }
        parse_ratings(movie, mv_file);
        if (fclose(mv_file) == EOF)
            perror("A movie file can't be closed.");
    }
    puts(" ✅");  // Information for the user
    return data;
}

void write_movie_data_to_file(char *filepath, MovieData *data)
{
    FILE *file = fopen(filepath, "wb");
    if (file == NULL) {
        fprintf(stderr, "Error: could not open file %s\n", filepath);
        return;
    }
    // Write the number of movies
    fwrite(&data->nb_movies, sizeof(uint16_t), 1, file);

    for (uint i = 0; i < data->nb_movies; i++)
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
    fclose(file);
}

MovieData *read_movie_data_from_file(char* filepath)
{
    FILE *file = fopen(filepath, "rb");
    if (file == NULL) {
        fprintf(stderr, "Error: could not open file %s\n", filepath);
        return NULL;
    }
    // Allocate memory for data
    MovieData *data = malloc(sizeof(MovieData));
    data->nb_movies = 0u;
    uint16_t nb_movies;
    if (!fread(&nb_movies, sizeof(uint16_t), 1, file)) {
        free(data);
        return NULL;
    }
    data->movies = malloc(nb_movies * sizeof(Movie*));
    for (uint i = 0; i < nb_movies; i++)
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
    fclose(file);
    return data;

read_error:
    free_movie_data(data);
    return NULL;
}

UserData *to_user_oriented(MovieData *data)
{
    UserData *user_data = malloc(sizeof(UserData));
    user_data->nb_users = 0;
    user_data->users = calloc(MAX_USER_ID, sizeof(User*));

    for (uint i = 0; i < data->nb_movies; i++) 
    {
        Movie *movie = data->movies[i];
        printf("\n\033[A\033[2K");  // Clear the line
        printf("Extracting user data %u/%u", movie->id, data->nb_movies);

        for (uint r = 0; r < movie->nb_ratings; r++)
        {
            MovieRating rating = movie->ratings[r];
            ulong id = get_customer_id(rating);
            if (user_data->users[id-1] == NULL) {
                User *user = user_data->users[id-1] = malloc(sizeof(User));
                user->id = id;
                user->nb_ratings = 0;
                user->ratings = malloc(sizeof(UserRating));
                user_data->nb_users++;
            }
            User *user = user_data->users[id-1];
            if (user->nb_ratings > 0 && is_power_of_two(user->nb_ratings))
                user->ratings = (UserRating *)realloc(user->ratings, 
                                    2 * user->nb_ratings * sizeof(UserRating));
            // Add the rating to the user
            user->ratings[user->nb_ratings].movie_id = movie->id;
            user->ratings[user->nb_ratings].date = rating.date;
            user->ratings[user->nb_ratings].score = rating.score;
            user->nb_ratings++;
        }
    }
    puts(" ✅");  // Information for the user
    return user_data;
}

void write_user_data_to_file(char *filepath, UserData *data)
{
    FILE *file = fopen(filepath, "wb");
    if (file == NULL) {
        fprintf(stderr, "Error: could not open file %s\n", filepath);
        return;
    }
    fwrite(&data->nb_users, sizeof(uint), 1, file);
    for (uint i = 0; i < MAX_USER_ID; i++)
    {
        User *user = data->users[i];
        if (user == NULL)
            continue;
        fwrite(&user->id, sizeof(uint32_t), 1, file);
        fwrite(&user->nb_ratings, sizeof(uint32_t), 1, file);
        fwrite(user->ratings, sizeof(UserRating), user->nb_ratings, file);
    }
    fclose(file);
}

UserData *read_user_data_from_file(char *filepath)
{
    FILE *file = fopen(filepath, "rb");
    if (file == NULL) {
        fprintf(stderr, "Error: could not open file %s\n", filepath);
        return NULL;
    }
    UserData *data = malloc(sizeof(UserData));
    if (!fread(&data->nb_users, sizeof(uint32_t), 1, file)) {
        free(data);
        return NULL;
    }
    data->users = calloc(MAX_USER_ID, sizeof(User*));
    for (uint i = 0; i < data->nb_users; i++)
    {
        User *user = calloc(1, sizeof(User));
        if (!fread(&user->id, sizeof(uint32_t), 1, file)) {
            free(user);
            goto read_error;
        }
        data->users[user->id-1] = user;
        if (!fread(&user->nb_ratings, sizeof(uint32_t), 1, file))
            goto read_error;
        user->ratings = malloc(user->nb_ratings * sizeof(UserRating));
        if (!fread(user->ratings, sizeof(UserRating), user->nb_ratings, file))
            goto read_error;        
    }
    fclose(file);
    return data;

read_error:
    free_user_data(data);
    return NULL;
}

// ================ Arguments ========================

static int compare_strings(const void *a, const void *b)
{
    return strncmp(*(const char **)a, *(const char **)b, LENGTH_MAX_TITLE);
}

static uint remove_duplicates(char **titles, uint n)
{
    uint nb_titles = 0;
    for (uint i = 0; i < n-1; i++) {
        if (strncmp(titles[i], titles[i+1], LENGTH_MAX_TITLE) != 0)
            titles[nb_titles++] = titles[i];
        else    
            free(titles[i]);  // Delete duplicate
    }
    titles[nb_titles++] = titles[n-1];
    return nb_titles;
}

uint parse_likes(const char *filename, MovieData *movie_data, uint **ids)
{
    FILE* likes_file = fopen(filename, "r");
    if (likes_file == NULL) 
        return 0;
    uint n = 0;
    char **titles = calloc(1, sizeof(char*));

    // Get the titles list
    char title[LENGTH_MAX_TITLE];

    while(fgets(title, LENGTH_MAX_TITLE, likes_file) != NULL) {
        if (n > 0 && is_power_of_two(n))
            titles = realloc(titles, 2 * n * sizeof(char*));

        title[strcspn(title, "\n")] = '\0';  // Remove the trailing '\n'
        titles[n++] = strdup(title);
    }
    fclose(likes_file);
    if (n == 0) goto end; // No movie in the file.

    // Sort the title list.
    qsort(titles, n, sizeof(char*), compare_strings);
    n = remove_duplicates(titles, n);

    // Get the corresponding identifiers
    *ids = calloc(n, sizeof(uint));
    uint c = n;

    for (uint m = 0; m < movie_data->nb_movies; m++)
    {
        int i = dichotomic_search(titles, n, movie_data->movies[m]->title);
        if (i != -1) {
            (*ids)[i] = movie_data->movies[m]->id;
            c--;
        }
        if (c == 0) break;  // All titles have been found.
    }
end:
    for (uint i = 0; i < n; i++)
        free(titles[i]);
    free(titles);
    return n;
}
