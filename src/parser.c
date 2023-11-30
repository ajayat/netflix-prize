#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "parser.h"
#include "utils.h"

#define LENGTH_MAX_TITLE 100
#define INITIAL_NUMBER_RATINGS 1000


char **parse_movies_titles(char *filename, unsigned short *nb_movies)
{
    // Opening of the file containing movies titles
    FILE *mv_titles = fopen(filename, "r");
    if (mv_titles == NULL)
        exit(EXIT_FAILURE);
    
    // Allocation of the dynamic table which will contain movies titles
    char **titles = malloc(2*sizeof(char*));
    if (titles == NULL)
        exit(EXIT_FAILURE);

    titles[1] = malloc(LENGTH_MAX_TITLE*sizeof(char));
    if (titles[1] == NULL)
        exit(EXIT_FAILURE);
    
    // Allocation of the title buffer
    char *title = malloc(LENGTH_MAX_TITLE*sizeof(char));
    if (title == NULL)
        exit(EXIT_FAILURE);

    unsigned short id;
    *nb_movies = 1;
    while(fscanf(mv_titles, "%hu%*c%*d%*c%s\n", id, title) != EOF) {
        if (id > *nb_movies) {
            realloc(titles, (id+1)*sizeof(char*));
            for (int t=*nb_movies+1 ; t<id+1 ; t++)
                titles[t] = malloc(LENGTH_MAX_TITLE*sizeof(char));
            *nb_movies = id;
        }
        strncpy(titles[id], title, LENGTH_MAX_TITLE);
    }
    return titles ;
}


Rating *parse_movie(const char *filename, unsigned int *nb_ratings)
{
    // Opening of the file containing ratings of the treated movie
    FILE *mv_ratings = fopen(filename, "r");
    if (mv_ratings == NULL)
        exit(EXIT_FAILURE);

    // Allocation of the dynamic table which will contain ratings
    Rating *ratings = malloc(INITIAL_NUMBER_RATINGS*sizeof(Rating));
    if (ratings == NULL)
        exit(EXIT_FAILURE);

    int size = INITIAL_NUMBER_RATINGS; // current size of the table ratings
    char score;
    unsigned int c_id;
    unsigned int y, m, d;
    *nb_ratings = 0;

    while(fscanf(mv_ratings, "%u%*c%1[1-5]%*c%4hu%*c%2hu%*c%2hu%*[^\n]\n", &c_id, &score, &y, &m, &d) == 5) {
        if (*nb_ratings >= size) {
            realloc(ratings, 2*size*sizeof(Rating));
            size *= 2;
        }
        ratings[*nb_ratings].customer_id = (uint32_t)c_id;
        ratings[*nb_ratings].score = (uint8_t)score;
        ratings[*nb_ratings].date = (uint16_t)days_from_epoch(y,m,d);
        *nb_ratings++;
    }

    return ratings ;
}