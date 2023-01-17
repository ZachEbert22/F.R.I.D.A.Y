//
// Created by Andrew Bowie on 1/13/23.
//

#include <sys_req.h>
#include "stdio.h"
#include "string.h"
#include "stdbool.h"
#include "mpx/comhand.h"

#define CMD_PROMPT ">> "

/**
 * The shutdown command, used to handle when a shutdown happens.
 * @param comm the command string.
 * @return true if the command was handled, false if not.
 */
bool shutdown(const char *comm)
{
        //The command's label.
        const char *label = "shutdown";

        //Check if it matched.
        int cmp = strcmp(comm, label);
        if(cmp != 0)
                return false;

        println("Shutting down...");
        return true;
}

/**
 * @brief An array of pointers to command functions. These functions
 * will return true if the command was handled by them, or false if not.
 */
bool (*comm_funcs[])(const char *comm) = {
        &shutdown
};

void comhand(void)
{
        printf("Welcome to MPX. Please select an option \n 1)Help\n 2)Set Time\n 3)Get Time\n Enter Choice:    ");
        for(;;)
        {
                //100 + 1 for the null terminator.
                char buf[101] = {0};

                print(CMD_PROMPT);
                sys_req(READ, COM1, buf, 100);
                println(buf);

                //Handle all functions.
                int comm_func_count = sizeof (comm_funcs) /
                        sizeof (comm_funcs[0]);
                for (int i = 0; i < comm_func_count; ++i)
                {
                        bool result = comm_funcs[i](buf);
                        if(result)
                                break;
                }
        }
}
