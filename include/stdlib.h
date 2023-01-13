#ifndef MPX_STDLIB_H
#define MPX_STDLIB_H

/**
 @file stdlib.h
 @brief A subset of standard C library functions.
*/

/**
 Convert an ASCII string to an integer
 @param s A NUL-terminated string
 @return The value of the string converted to an integer
*/
int atoi(const char *s);

/**
 * @brief Convert a signed integer to a string
 * @param i the integer to convert
 * @param str_buf the buffer to store the integer in
 * @param buf_len the string buffer length
 * @return the created string from the integer
 */
char *itoa(int i, char *str_buf, int buf_len);

#endif
