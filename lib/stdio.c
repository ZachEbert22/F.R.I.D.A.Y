//
// Created by Andrew Bowie on 1/13/23.
//

#include "stdio.h"
#include "sys_req.h"
#include "string.h"

void print(const char *s)
{
        int str_len = (int) strlen(s);

        sys_req(WRITE, COM1, s, str_len);
}

void println(const char *s)
{
        int str_len = (int) strlen(s);

        sys_req(WRITE, COM1, s, str_len);
        sys_req(WRITE, COM1, "\n", 1);
}