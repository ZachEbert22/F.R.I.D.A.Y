//
// Created by Andrew Bowie on 1/13/23.
//

#include "stdio.h"
#include "stdbool.h"
#include "mpx/comhand.h"
#include "commands.h"
#include "string.h"
#include "cli.h"
#include "print_format.h"

///The message to send to the user if a command hasn't been recognized.
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
        &cmd_set_tz,
        &cmd_clear,
        &cmd_color,
        &cmd_pcb
};

/// Used to denote if the comm hand should stop.
static bool sig_shutdown = false;

void signal_shutdown(void)
{
    sig_shutdown = true;
}

/**
 * @brief Prints a welcome message to the user.
 */
void print_welcome(void)
{
    set_output_color(get_color("red"));
    println("*********      **********      **      ****                  **         **        **");
    println("**             **     **       **      **   **             **  **         **    **");
    println("**             **    **        **      **    **           **     **         ****");
    println("*****          **  **          **      **     **         **       **         **");
    println("**             ****            **      **     **        *************        **");
    println("**             **  **          **      **     **       **           **       ** ");
    println("**             **    **        **      **    **       **             **      **");
    println("**             **     **       **      **   **       **               **     **");
    println("**             **      **      **      ******       **                 **    **");
    set_output_color(get_color("reset"));

    println("Welcome to MPX. Please select an option");
    println("=> help");
    println("=> get-time-date");
    println("=> set-time HH:mm:SS");
    println("=> set-date MM/DD/YY");
    println("=> set-timezone");
    println("=> version");
    println("=> shutdown");
    println("=> color");
    println("=> clear");
}

void comhand(void)
{
    print_welcome();
    sig_shutdown = false;
    while (!sig_shutdown)
    {
        //60 + 1 for the null terminator.
        char buf[61] = {0};

        print(CMD_PROMPT);

        set_cli_history(true);
        gets(buf, 60);
        set_cli_history(false);

        //Strip whitespace.
        str_strip_whitespace(buf, NULL, 0);

        //Handle all functions.
        int comm_func_count = sizeof(comm_funcs) /
                              sizeof(comm_funcs[0]);

        //Loop over all commands and check if it matches.
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

        //If something wasn't found, print the unknown command message.
        if (!found && strlen(buf) > 0)
        {
            printf(UNKNOWN_CMD_MSG, buf);
            println("");
        }
    }
}