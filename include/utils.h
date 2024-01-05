#pragma once

#include <stdio.h>

#define EPOCH 729695 // 1st January 1998

typedef unsigned int u_int;

/**
 * @brief Get the size in byte of a file.
 * @param file The file to get the size from.
 * @return The size of the file.
 */
u_int get_size(FILE *file);

/**
 * @brief Get the number of days between a gven date and Epoch.
 * @note Epoch is the 1st January 1889.
 */
u_int days_from_epoch(u_int year, u_int month, u_int day);

/**
 * @brief Duplicate a string.
 * @param str The string to duplicate.
 * @return The duplicated string.
 * 
 * @note This function is not part of the C standard library before C23.
*/
char *strdup(const char *str);
