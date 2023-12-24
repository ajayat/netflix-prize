#pragma once

#include <stdio.h>

#define EPOCH 689884 // 1st January 1889

/**
 * @brief Get the size in byte of a file.
 * @param file The file to get the size from.
 * @return The size of the file.
 */
unsigned int get_size(FILE *file);

/**
 * @brief Get the number of days between a gven date and Epoch.
 * @note Epoch is the 1st January 1889.
 */
unsigned int days_from_epoch(unsigned int year, unsigned int month, unsigned int day);

/**
 * @brief Duplicate a string.
 * @param str The string to duplicate.
 * @return The duplicated string.
 * 
 * @note This function is not part of the C standard library before C23.
*/
char *strdup(const char *str);
