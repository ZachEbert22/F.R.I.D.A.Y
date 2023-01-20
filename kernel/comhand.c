//
// Created by Andrew Bowie on 1/13/23.
//

#include <sys_req.h>
#include "stdio.h"
#include "stdbool.h"
#include "mpx/comhand.h"
#include "commands.h"
#include "string.h"

#define UNKNOWN_CMD_MSG "Unknown command '%s'. Type 'help' for help!"

/**
 * @brief An array of pointers to command functions. These functions
 * will return true if the command was handled by them, or false if not.
 */
bool (*comm_funcs[])(const char *comm) = {
        &cmd_version,
        &cmd_help,
        &cmd_get_time_menu,
        &cmd_shutdown,
        &cmd_set_date,
        &cmd_set_time,
        &cmd_set_tz
};

/// Used to denote if the comm hand should stop.
bool sig_shutdown = false;

void signal_shutdown(void)
{
    sig_shutdown = true;
}

/**
 * @brief Prints a welcome message to the user.
 */
void print_welcome(void)
{
    println("*********      **********      **      ****                  **         **      **");
    println("**             **     **       **      **   **             **  **         **   **");
    println("**             **    **        **      **    **           **     **         ****");
    println("*****          **  **          **      **     **         **       **         **");
    println("**             ****            **      **     **        *************        **");
    println("**             **  **          **      **     **       **           **       ** ");
    println("**             **    **        **      **    **       **             **      **");
    println("**             **     **       **      **   **       **               **     **");
    println("**             **      **      **      ******       **                 **    **");

    println("Welcome to MPX. Please select an option");
    println("=> Help");
    println("=> Set Time");
    println("=> Set Date");
    println("=> Get Time");
    println("=> version");
    println("=> shutdown");
}

void comhand(void)
{
    while (!sig_shutdown)
    {
        //100 + 1 for the null terminator.
        char buf[101] = {0};

        print(CMD_PROMPT);
        sys_req(READ, COM1, buf, 100);

        //Strip whitespace.
        str_strip_whitespace(buf, NULL, 0);

        //Handle all functions.
        int comm_func_count = sizeof(comm_funcs) /
                              sizeof(comm_funcs[0]);

        bool found = false;
        for (int i = 0; i < comm_func_count; ++i)
        {
            bool result = comm_funcs[i](buf);
            if (result)
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            printf(UNKNOWN_CMD_MSG, buf);
            println("");
        }
    }
}