//
// Created by Andrew Bowie on 1/13/23.
//

#include <sys_req.h>
#include "stdio.h"
#include "string.h"
#include "stdbool.h"
#include "mpx/comhand.h"
#include "commands.h"

#define CMD_PROMPT ">> "

/**
 * @brief An array of pointers to command functions. These functions
 * will return true if the command was handled by them, or false if not.
 */
bool (*comm_funcs[])(const char *comm) = {
        &version,
        &help,
        &get_time_menu,
        &set_time,
        &set_date
};

void comhand(void)
{
        println("Welcome to MPX. Please select an option");
        println("=> Help");
        println("=> Set Time");
        println("=> Set Date");
        println("=> Get Time");
        println("=> version");
        println("=> shutdown");
        for(;;)
        {
                //100 + 1 for the null terminator.
                char buf[101] = {0};

                print(CMD_PROMPT);
                sys_req(READ, COM1, buf, 100);

                //Handle all functions.
                int comm_func_count = sizeof (comm_funcs) /
                        sizeof (comm_funcs[0]);

                if (strcicmp(buf, "shutdown") == 0){
                        println("Are you sure you want to shutdown? enter y for yes or N for no");
                        memset(buf,'\0', 101);
                        print(CMD_PROMPT);
                        sys_req(READ, COM1, buf, 5);
                        if (strcicmp(buf, "y") == 0 || strcicmp(buf, "yes") == 0){
                                return;
                        }else{
                                continue;
                        }
                }

                for (int i = 0; i < comm_func_count; ++i)
                {
                        bool result = comm_funcs[i](buf);
                        if(result)
                                break;
                }


        }
}
