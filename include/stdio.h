//
// Created by Andrew Bowie on 1/13/23.
//

#ifndef F_R_I_D_A_Y_STDIO_H
#define F_R_I_D_A_Y_STDIO_H

#include "stddef.h"

/**
 * @file stdio.h
 * @brief Contains useful functions for standard IO
 */

/**
 * @brief Reads a string of input from the standard input source.
 * @param str_buf the buffer to store the string in.
 * @param buf_len the amount of bytes to read. (The buffer should be at least one byte longer)
 * @return a pointer to the read array.
 */
char *gets(char *str_buf, size_t buf_len);

/**
 * @brief Prints a null-terminated string to standard output.
 * @param str the string.
 */
void print(const char *str);

/**
 * @brief Prints the string with formatting to standard outpu.
 * @param str the string to print.
 * @param ... the formatting objects.
 * @return 0 if successful, -1 if there was a formatting error.
 */
int printf(const char *str, ...);

/**
 * @brief Prints a null-terminated string, then a new line, to standard output.
 * @param str the string.
 */
void println(const char *str);

#endif //F_R_I_D_A_Y_STDIO_H
