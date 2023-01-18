#ifndef F_R_I_D_A_Y_SET_TIME_H
#define F_R_I_D_A_Y_SET_TIME_H

int get_time();

int get_index(int a);

bool set_time_clock(unsigned int a, unsigned int b, unsigned int c);

bool set_date_clock(unsigned int a, unsigned int b, unsigned int c);

unsigned char decimalToBCD(unsigned int first, unsigned int second);

bool isValidTimeOrDate(const char* date, unsigned int buf[], int buf_len, char c);

int getDaysInMonth(int month, int year);
#endif