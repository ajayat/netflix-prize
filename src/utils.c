#include <stdlib.h>
#include <string.h>

#include "utils.h"

bool is_power_of_two(ulong x)
{
    return (x != 0) && ((x & (x - 1)) == 0);
}

int min(int a, int b)
{
    return (a < b) ? a : b;
}

uint get_size(FILE *file)
{
    fseek(file, 0L, SEEK_END);
    uint size = ftell(file);
    rewind(file);
    return size;
}

uint days_from_epoch(uint year, uint month, uint day)
{
    // Shift the calendar to start on March 1st
    int m = (month + 9) % 12;
    int y = year - m/10;
    // Number of days between 1st March and 1st <month m>
    uint month_days = (m*306 + 5)/10;
    // Number of days between 1st March 1 and 1st March <year y> excluding leap years
    uint year_days = 365*y + y/4 - y/100 + y/400;
    uint total = year_days + month_days + day - 1;
    return total - EPOCH;
}

char *strdup(const char *str)
{
    size_t str_length = strlen(str) + 1;
    char *dup = malloc(str_length * sizeof(char));
    if (dup != NULL)
        strncpy(dup, str, str_length);
    return dup;
}

inline float get_similarity(float *sim, uint i, uint j)
{
    if (i == j)
        return 0.;
    if (j < i)
        return sim[i * (i - 1) / 2 + j];
    else
        return sim[j * (j - 1) / 2 + i];
}

int dichotomic_search(char **array, uint length, char *string)
{
    int s = -1;
    int a = 0;
    int b = length - 1;
    uint m = 0;

    while (a <= b) {
        m = (a + b) / 2;
        s = strcmp(array[m], string);
        if (s == 0) 
            return m;
        if (s < 0) 
            a = m + 1;
        else 
            b = m - 1;
    }
    return -1;
}
