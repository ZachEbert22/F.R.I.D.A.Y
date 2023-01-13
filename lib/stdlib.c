#include <stdlib.h>
#include <ctype.h>
#include <stddef.h>

int atoi(const char *s)
{
	int res = 0;
	char sign = ' ';

	while (isspace(*s)) {
		s++;
	}

	if (*s == '-' || *s == '+') {
		sign = *s;
		s++;
	}

	while ('0' <= *s && *s <= '9') {
		res = res * 10 + (*s - '0');
		s++;

	}

	if (sign == '-') {
		res = res * -1;
	}

	return res;
}

double atod(const char *s)
{
	double res = 0.0;
	char sign = ' ';
        int length = 0;
	double decimal = 0.0;

	while (isspace(*s)) {
		s++;
	}

	if (*s == '-' || *s == '+') {
		sign = *s;
		s++;
	}

	while ('0' <= *s && *s <= '9') {
		res = res * 10 + (*s - '0');
		s++;

	}

	
        if(*s == '.'){
                s++;
        }
        length = strlen(s);

        while ('0' <= *s && *s <= '9') {
		decimal = decimal * 10 + (*s - '0');
		s++;
	}
        if (sign == '-') {
		res = res * -1;
                decimal = decimal * -1;
	}
        for(int i = 0; i < length; i++){
                decimal = decimal/10;
        }

	return res+decimal;
}

char *itoa(int i, char *str_buf, int buf_len)
{
        if(buf_len == 0)
                return NULL;

        int num_pos = 0;

        //Check for a sign.
        if(i < 0)
        {
                str_buf[0] = '-';
                i *= -1;
                num_pos = 1;
        }

        if(buf_len <= num_pos)
                return NULL;

        //Loop through the number, removing a single digit at a time.
        char swap[10] = {0};
        int num_index = 0;
        while(i > 0)
        {
                int digit = i % 10;

                swap[num_index++] = (char) (digit + '0');

                i /= 10;
        }

        //If this is the case, we can't store the string.
        if(num_index + num_pos >= buf_len)
                return NULL;

        //Put all chars from swap into the buffer.
        for (int index = num_index - 1; index >= 0; --index)
        {
                str_buf[num_pos++] = swap[index];
        }
        return str_buf;
}
