#include <stdio.h>
#include <stdlib.h>

#include "parser.h"

int main(void)
{
    FILE *bin_file = fopen("data/data.bin", "wb");
    if (bin_file == NULL) {
        fprintf(stderr, "Error: could not open file %s\n", "data.bin");
        return 1;
    }
    write_to_file(bin_file);
    fclose(bin_file);

    FILE *bin_file2 = fopen("data/data.bin", "rb");
    if (bin_file2 == NULL) {
        fprintf(stderr, "Error: could not open file %s\n", "data.bin");
        return 1;
    }
    Movie* movies = read_from_file(bin_file2);
    fclose(bin_file2);
    // print some info about the movies
    for (unsigned int i = 0; i < 17770; i++)
        printf("%u: %s\n", movies[i].id, movies[i].title);
    
    // free memory
    for (unsigned int i = 0; i < 17770; i++) {
        free(movies[i].ratings);
        free(movies[i].title);
    }
    free(movies);
    return 0;
}
