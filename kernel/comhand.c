//
// Created by Andrew Bowie on 1/13/23.
//

#include <sys_req.h>
#include "comhand.h"

void comhand(void)
{
        for(;;)
        {
                char buf[100] = {0};

                sys_req(READ, COM1, buf, 100);

                sys_req(WRITE, COM1, "You wrote: ", 10);
                sys_req(WRITE, COM1, buf, 100);
                sys_req(WRITE, COM1, "\n", 1);
        }
        return;
}
