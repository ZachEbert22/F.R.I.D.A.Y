#include <string.h>
#include "stdarg.h"
#include "stdlib.h"
#include "stdio.h"

void *memcpy(void * restrict s1, const void * restrict s2, size_t n)
{
	unsigned char *dst = s1;
	const unsigned char *src = s2;
	for (size_t i = 0; i < n; i++) {
		dst[i] = src[i];
	}
	return s1;
}

void *memset(void *s, int c, size_t n)
{
	unsigned char *p = s;
	for (size_t i = 0; i < n; i++) {
		p[i] = (unsigned char)c;
	}
	return s;
}

int strcmp(const char *s1, const char *s2)
{

	// Remarks:
	// 1) If we made it to the end of both strings (i. e. our pointer points to a
	//    '\0' character), the function will return 0
	// 2) If we didn't make it to the end of both strings, the function will
	//    return the difference of the characters at the first index of
	//    indifference.
	while ((*s1) && (*s1 == *s2)) {
		++s1;
		++s2;
	}
	return (*(unsigned char *)s1 - *(unsigned char *)s2);
}

size_t strlen(const char *s)
{
	size_t len = 0;
	while (*s++) {
		len++;
	}
	return len;
}

char *strtok(char * restrict s1, const char * restrict s2)
{
	static char *tok_tmp = NULL;
	const char *p = s2;

	//new string
	if (s1 != NULL) {
		tok_tmp = s1;
	}
	//old string cont'd
	else {
		if (tok_tmp == NULL) {
			return NULL;
		}
		s1 = tok_tmp;
	}

	//skip leading s2 characters
	while (*p && *s1) {
		if (*s1 == *p) {
			++s1;
			p = s2;
			continue;
		}
		++p;
	}

	//no more to parse
	if (!*s1) {
		return (tok_tmp = NULL);
	}
	//skip non-s2 characters
	tok_tmp = s1;
	while (*tok_tmp) {
		p = s2;
		while (*p) {
			if (*tok_tmp == *p++) {
				*tok_tmp++ = '\0';
				return s1;
			}
		}
		++tok_tmp;
	}

	//end of string
	tok_tmp = NULL;
	return s1;
}

char *sprintf(const char *s, char *str, size_t buf_len, ...)
{
	va_list va;
	va_start(va, buf_len);
	char *result = vsprintf(s, str, buf_len, va);
	va_end(va);

	return result;
}

char *vsprintf(const char *s, char *str, size_t buf_len, va_list va)
{
	//Copy the arguments for the 2nd pass.
	va_list copy;
	va_copy(copy, va);

	//Get the string length and try to format it.
	int str_len = (int) strlen(s);

	//Loop through the string, formatting each.
	int net_str_len = 0;
	for (int i = 0; i < str_len; ++i)
	{
		char at = s[i];

		//If we've found the formatting symbol, try to format.
		if(at != '%')
		{
			net_str_len++;
			continue;
		}

		//Find the appropriate formatting
		char arguments[5] = {0};
		int arg_count = 0;
		int found_any = 0;
		for (int j = i + 1; j < str_len; ++j)
		{
			char f_code = s[j];
			if(f_code == 's')
			{
				//Arguments not supported for strings.
				if(arg_count > 0)
					return NULL;

				i = j;

				int arg_len = (int) strlen(va_arg(va, char *));
				net_str_len += arg_len;
				found_any = 1;
				break;
			}
			else if(f_code == 'd')
			{
				//Multi args not supported.
				if(arg_count > 0)
					return NULL;

				i = j;

				//Convert the argument.
				int num = va_arg(va, int);
				char buf[12] = {0};
				itoa(num, buf, 12);

				int arg_len = (int) strlen(buf);
				net_str_len += arg_len;
				found_any = 1;
				break;
			}
			else if(f_code == 'c')
			{
				//Multi args not supported.
				if(arg_count > 0)
					return NULL;

				i = j;
				net_str_len++;
				va_arg(va, int);
				found_any = 1;
				break;
			}
			else if(f_code == '%')
			{
				//Multiple arguments not supported.
				if(arg_count > 0)
					return NULL;

				i = j;
				net_str_len++;
				found_any = 1;
				break;
			}

			//If the argument was improperly defined, return.
			if(arg_count >= 5)
				return NULL;

			arguments[arg_count++] = f_code;
		}

		if(!found_any)
			return NULL;
	}

	//Check the string buffer length.
	if((int) buf_len < net_str_len + 1)
		return NULL;

	//Now, build the actual string.
	//The second loop is used to avoid using dynamically allocated
	//memory. If freeing memory was possible, it would be easier
	//to build the strings in one loop and place them in the 2nd.
	int str_ind = 0;
	for (int i = 0; i < str_len; ++i)
	{
		char at = s[i];

		//If we've found the formatting symbol, try to format.
		if(at != '%')
		{
			str[str_ind++] = at;
			continue;
		}

		//Find the appropriate formatting
		char arguments[5] = {0};
		int arg_count = 0;
		for (int j = i + 1; j < str_len; ++j)
		{
			char f_code = s[j];
			if(f_code == 's')
			{
				//Arguments not supported for strings.
				if(arg_count > 0)
					return NULL;

				i = j;

				char *arg = va_arg(copy, char *);

				int len = (int) strlen(arg);
				for (int k = 0; k < len; ++k)
				{
					str[str_ind++] = arg[k];
				}
				break;
			}
			else if(f_code == 'd')
			{
				//Multi args not supported.
				if(arg_count > 0)
					return NULL;

				i = j;

				//Convert the argument.
				int num = va_arg(copy, int);
				char buf[12] = {0};
				itoa(num, buf, 12);

				int len = (int) strlen(buf);
				for (int k = 0; k < len; ++k)
				{
					str[str_ind++] = buf[k];
				}
				break;
			}
			else if(f_code == 'c')
			{
				//Multi args not supported.
				if(arg_count > 0)
					return NULL;

				i = j;
				char val = va_arg(copy, int);

				str[str_ind++] = val;
				break;
			}
			else if(f_code == '%')
			{
				//Multiple arguments not supported.
				if(arg_count > 0)
					return NULL;

				i = j;
				str[str_ind++] = '%';
				break;
			}

			//If the argument was improperly defined, return.
			if(arg_count >= 5)
				return NULL;

			arguments[arg_count++] = f_code;
		}
	}

	str[net_str_len] = '\0';
	return str;
}
