#ifndef F_R_I_D_A_Y_SET_TIME_H
#define F_R_I_D_A_Y_SET_TIME_H

int get_time();

int get_index(int a);

bool set_time_clock(unsigned int a, unsigned int b, unsigned int c);

bool set_date_clock(unsigned int a, unsigned int b, unsigned int c);

unsigned char decimal_to_bcd(unsigned int decimal);

bool is_valid_date_or_time(int word_len,char buf[][word_len], int buff_len);

unsigned int get_days_in_month(int month, int year);
#endif