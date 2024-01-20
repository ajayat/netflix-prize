#pragma once

#include <stdio.h>
#include <stdbool.h>

#define EPOCH 729695 // 1st January 1998

typedef unsigned int uint;
typedef unsigned long ulong;

/**
 * @brief Return true if the given number is a power of two.
 * 
 * @param x Given number.
 * @return true if it is a power of two.
 * @return false otherwise.
 */
bool is_power_of_two(ulong x);

/**
 * @brief Return the minimal value between `a` and `b`.
 * 
 * @param a First value to compare.
 * @param b Second value to compare.
 * @return `int` The minimum of values.
 */
int min(int a, int b);

/**
 * @brief Get the size in byte of a file.
 * @param file The file to get the size from.
 * @return The size of the file.
 */
uint get_size(FILE *file);

/**
 * @brief Get the number of days between a gven date and Epoch.
 * @note Epoch is the 1st January 1889.
 */
uint days_from_epoch(uint year, uint month, uint day);

/**
 * @brief Duplicate a string.
 * @param str The string to duplicate.
 * @return The duplicated string.
 * 
 * @note This function is not part of the C standard library before C23.
*/
char *strdup(const char *str);

/**
 * @brief Get the similarity between two movies.
*/
float get_similarity(float *similarity_matrix, uint i, uint j);

/**
 * @brief Find the index of a given string in a sorted array.
 * @param array The sorted array.
 * @param length The length of the array.
 * @param string The string to find.
 * @return The index of the string in the array, -1 if it is not found.
*/
int dichotomic_search(char **array, uint length, char *string);

/**
 * @brief Get the filepath of a file in the current working directory.
 * @param folder The name of the folder containing the file.
 * @param filename The name of the file.
*/
char *get_filepath(const char *folder, char *filename);
