#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"

char **parse_movies_titles(char *filename, int *nb_movies)
{
    // Opening of the file containing movies titles
    FILE *movies_titles = fopen(filename, "r");
    if (movies_titles == NULL) 
        exit(EXIT_FAILURE) ;

    // Allocation of the dynamic table which will contain movies titles
    char **titles = malloc(2*sizeof(char*)) ;
    if (titles == NULL) 
        exit(EXIT_FAILURE) ;

    titles[0] = malloc(LENGTH_MAX_TITLE*sizeof(char)) ;
    titles[1] = malloc(LENGTH_MAX_TITLE*sizeof(char)) ;
    if (titles[0] == NULL || titles[1] == NULL) 
        exit(EXIT_FAILURE) ;

    // Allocation of the title buffer
    char *title = malloc(LENGTH_MAX_TITLE*sizeof(char)) ;
    if (title == NULL) 
        exit(EXIT_FAILURE) ;

    int id ;
    *nb_movies = 1 ;
    while(fscanf(movies_titles, "%d%*c%*d%*c%s\n", id, title) != EOF) {
        if (id > *nb_movies) {
            realloc(titles, (id+1)*sizeof(char*)) ;
            for (int t=*nb_movies+1 ; t<id+1 ; t++)
                titles[t] = malloc(LENGTH_MAX_TITLE*sizeof(char)) ;
            *nb_movies = id ;
        }
        strncpy(titles[id], title, LENGTH_MAX_TITLE) ;
    }
    return titles ;
}