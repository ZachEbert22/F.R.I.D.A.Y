//
// Created by Andrew Bowie on 1/13/23.
//

#include "stdio.h"
#include "stdarg.h"
#include "sys_req.h"
#include "stdlib.h"
#include "string.h"

void print(const char *s)
{
        int str_len = (int) strlen(s);

        sys_req(WRITE, COM1, s, str_len);
}

int printf(const char *s, ...)
{
        va_list va;
        va_start(va, s);

        //Get the string length and try to format it.
        int str_len = (int) strlen(s);

        //Keep track of arguments.
        char *argument_arr[100] = {};

        //Loop through the string, formatting each.
        int arguments_expected = 0;
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
                                        return -1;

                                i = j;

                                //Format the argument.
                                char *str = va_arg(va, char *);
                                argument_arr[arguments_expected++] =
                                        str;

                                int arg_len = (int) strlen(str);
                                net_str_len += arg_len;
                                found_any = 1;
                                break;
                        }
                        else if(f_code == 'd')
                        {
                                //Multi args not supported.
                                if(arg_count > 0)
                                        return -1;

                                i = j;

                                //Convert the argument.
                                int num = va_arg(va, int);
                                char buf[20] = {0};
                                itoa(num, buf, 20);
                                argument_arr[arguments_expected++] = buf;

                                int arg_len = (int) strlen(buf);
                                net_str_len += arg_len;
                                found_any = 1;
                                break;
                        }
                        else if(f_code == '%')
                        {
                                //Multiple arguments not supported.
                                if(arg_count > 0)
                                        return -1;

                                i = j;
                                net_str_len++;

                                argument_arr[arguments_expected++] =
                                        "%";
                                found_any = 1;
                                break;
                        }

                        //If the argument was improperly defined, return.
                        if(arg_count >= 5)
                                return -1;

                        arguments[arg_count++] = f_code;
                }

                if(!found_any)
                        return -1;
        }
        va_end(va);

        //Now, build the actual string.
        char new_str[net_str_len + 1];
        int arg_num = 0;
        int str_ind = 0;
        for (int i = 0; i < str_len; ++i)
        {
                char at = s[i];

                //If we've found the formatting symbol, try to format.
                if(at != '%')
                {
                        new_str[str_ind++] = at;
                        continue;
                }

                //Find the appropriate formatting
                for (int j = i + 1; j < str_len; ++j)
                {
                        char f_code = s[j];
                        if(f_code == 's' || f_code == 'd' || f_code == '%')
                        {
                                i = j;

                                //Insert the argument.
                                char *arg = argument_arr[arg_num++];
                                int arg_len = (int) strlen(arg);
                                for (int k = 0; k < arg_len; ++k)
                                {
                                        new_str[str_ind++] = arg[k];
                                }
                                break;
                        }
                }
        }

        new_str[net_str_len] = '\0';
        print(new_str);
        return 0;
}

void println(const char *s)
{
        int str_len = (int) strlen(s);

        sys_req(WRITE, COM1, s, str_len);
        sys_req(WRITE, COM1, "\n", 1);
}