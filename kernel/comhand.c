//
// Created by Andrew Bowie on 1/13/23.
//

#include <sys_req.h>
#include "stdio.h"
#include "mpx/comhand.h"

void comhand(void)
{
        for(;;)
        {
                //100 + 1 for the null terminator.
                char buf[101] = {0};

                print(">> ");
                sys_req(READ, COM1, buf, 100);

                sys_req(WRITE, COM1, "You wrote: ", 10);
                sys_req(WRITE, COM1, buf, 100);
                sys_req(WRITE, COM1, "\n", 1);
        }
        return;
}
