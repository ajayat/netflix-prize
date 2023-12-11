#include <errno.h>

#include "unity.h"
#include "parser.h"

void setUp(void) {}
void tearDown(void) {}

/**
 * @brief Test about the ability to write correctly in the binary file.
 * 
 */
void test_write_to_file(void)
{
    FILE *file = fopen("data/data.bin", "wb");
        if (ferror(file)) {
            perror("data/data.bin");
            return 1;
        }
        Data* data = write_to_file(file);
    fclose(file);
    free_data(data); // Free memory
}

void test_read_from_file(void)
{
    FILE *file = fopen("data/data.bin", "rb");
    if (file == NULL) {
        fprintf(stderr, "Error: could not open file %s\n", "data.bin");
        return 1;
    }
    Data *data = read_from_file(file);
    fclose(file);

    // print some info about the movies
    for (unsigned int i = 0; i < 17770; i++) {
        Movie *movie = data->movies[i];
        printf("%u: %s\n", movie->id, movie->title);
        printf("    - %u ratings\n", movie->nb_ratings);
        printf("    - date : %u\n", movie->date/365 + 1900);
        Rating rating1 = movie->ratings[0];
        printf("    - Rating 1:\n");
        printf("         - customer id: %lu\n", get_customer_id(rating1));
        printf("         - note: %u\n", rating1.score);
        printf("         - date: %u\n", rating1.date/365 + 1900);
    }
    free_data(data); // Free memory
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_write_to_file);
    RUN_TEST(test_read_from_file);
    return UNITY_END();
}