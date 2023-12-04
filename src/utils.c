#include <stdio.h>

#include "utils.h"

#define EPOCH 689884 // 1st January 1889

unsigned int get_size(FILE *file)
{
    fseek(file, 0L, SEEK_END);
    unsigned int size = ftell(file);
    rewind(file);
    return size;
}

unsigned int days_from_epoch(unsigned int year, unsigned int month, unsigned int day)
{
    // Shift the calendar to start on March 1st
    int m = (month + 9) % 12;
    int y = year - m/10;
    // Number of days between 1st March and 1st <month m>
    unsigned int month_days = (m*306 + 5)/10;
    // Number of days between 1st March 1 and 1st March <year y> excluding leap years
    unsigned int year_days = 365*y + y/4 - y/100 + y/400;
    unsigned int total = year_days + month_days + day - 1;
    return total - EPOCH;
}
