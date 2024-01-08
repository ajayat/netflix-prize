#include <stdlib.h>
#include <string.h>

#include "utils.h"

bool is_power_of_two(ulong x)
{
    return (x != 0) && ((x & (x - 1)) == 0);
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