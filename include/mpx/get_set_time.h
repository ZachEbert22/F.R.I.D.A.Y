#ifndef F_R_I_D_A_Y_SET_TIME_H
#define F_R_I_D_A_Y_SET_TIME_H

/**
 * @brief Sets the timezone hour offset.
 * @param offset the hour offset.
 */
void set_timezone(int offset);

/**
 * @brief Prints the time and date of the system.
 * @return 0 if successful, negative if not.
 */
int print_time();

/**
 * @brief Sets the time of the system clock to the provided values.
 * @param hr the hours, in BCD.
 * @param min the minutes, in BCD.
 * @param sec the seconds, in BCD.
 * @return true if the time was changed, false if the values were invalid.
 */
bool set_time_clock(unsigned int hr, unsigned int min, unsigned int sec);

/**
 * @brief Sets the date of the system clock to the provided values.
 * @param month the month, in BCD.
 * @param day the day, in BCD.
 * @param year the year, in BCD.
 * @return true if the time was changed, false if the values were invalid.
 */
bool set_date_clock(unsigned int month, unsigned int day, unsigned int year);

/**
 * @brief Converts the given decimal number to BCD.
 * @param decimal the number to convert.
 * @return the converted number.
 */
unsigned char decimal_to_bcd(unsigned int decimal);

/**
 * @brief Checks if the given array of time values is validly defined.
 *        All strings in the array must be valid, positive, 2 digit numbers.
 * @param word_len the length of 2nd dimension of the array.
 * @param buf the array.
 * @param buff_len the length of the 1st dimension of the array.
 * @return if the provided array is valid.
 */
bool is_valid_date_or_time(int word_len, char buf[][word_len], int buff_len);

/**
 * @brief Gets the amount of days in the provided month and returns it in BCD.
 * @param month the month of the year, in BCD.
 * @param year the year, in BCD. (Used for leap years)
 * @return the amount of days in the month, in BCD.
 */
unsigned int get_days_in_month(int month, int year);
#endif