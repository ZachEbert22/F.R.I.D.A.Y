//
// Created by Andrew Bowie on 1/13/23.
//

#include "stdio.h"
#include "stdarg.h"
#include "sys_req.h"
#include "stdlib.h"
#include "string.h"
#include "memory.h"

#define PRINTF_BUF_LEN 250

char *gets(char *str_buf, size_t buf_len)
{
    sys_req(READ, COM1, str_buf, buf_len);
    return str_buf;
}

char getc(void)
{
    char buf[2] = {0};
    sys_req(READ, COM1, buf, 1);
    return buf[0];
}

char pollc(void)
{
    int rc = sys_req(READ, COM1, NULL, 1);
    return (char) rc;
}

void print(const char *s)
{
        int str_len = (int) strlen(s);

        sys_req(WRITE, COM1, s, str_len);
}

int printf(const char *s, ...)
{
        char buffer[PRINTF_BUF_LEN] = {0};

        //Format the string.
        va_list va;
        va_start(va, s);
        char *result = vsprintf(s, buffer, PRINTF_BUF_LEN, va);
        va_end(va);
        if(result == NULL)
                return -1;

        print(result);
        return 0;
}

void clearscr(void)
{
    //The 'Clear Screen' action
    char clear_screen[5] = {
            27,
            '[',
            '2',
            'J',
            0
    };
    //The 'reset cursor' action
    char reset_cursor[7] = {
            27,
            '[',
            '1',
            ';',
            '1',
            'H',
            0
    };
    print(clear_screen);
    print(reset_cursor);
}

void println(const char *s)
{
        int str_len = (int) strlen(s);

        sys_req(WRITE, COM1, s, str_len);
        sys_req(WRITE, COM1, "\n", 1);
}