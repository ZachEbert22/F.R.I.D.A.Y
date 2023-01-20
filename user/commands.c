//
// Created by Andrew Bowie on 1/18/23.
//

#include "commands.h"
#include "string.h"
#include "stdio.h"
#include "sys_req.h"
#include "mpx/comhand.h"
#include "mpx/serial.h"
#include "mpx/get_set_time.h"
#include "stdlib.h"

#define CMD_HELP_LABEL "help"
#define CMD_VERSION_LABEL "version"
#define CMD_SHUTDOWN_LABEL "shutdown"
#define CMD_GET_TIME_LABEL "get-time"
#define CMD_SET_TIME_LABEL "set-time"
#define CMD_SET_DATE_LABEL "set-date"

bool cmd_version(const char *comm)
{
    //The command's label.
    const char *label = CMD_VERSION_LABEL;

    //Check if it matched.
    int cmp = strcicmp(comm, label);
    if (cmp != 0)
        return false;

    println("Module: R1");
    println(__DATE__);
    println(__TIME__);
    return true;
}

bool cmd_shutdown(const char *comm)
{
    const char *label = CMD_SHUTDOWN_LABEL;

    int cmp = strcicmp(comm, label);
    if (cmp != 0)
        return false;

    print("Are you sure you want to shutdown? (y/N): ");
    char confirm_buf[6] = {0};
    set_cli_history(0);
    sys_req(READ, COM1, confirm_buf, 5);
    set_cli_history(1);

    //Check confirmation.
    if (strcicmp(confirm_buf, "y") == 0 ||
        strcicmp(confirm_buf, "yes") == 0)
    {
        signal_shutdown();
    }
    return true;
}

bool cmd_get_time_menu(const char *comm)
{
    //The command's label.
    const char *label = CMD_GET_TIME_LABEL;

    //Check if it matched.
    int cmp = strcicmp(comm, label);
    if (cmp != 0)
        return false;

    get_time();
    //println(__TIME__);
    return true;
}

bool cmd_set_date(const char *comm)
{
    const char *label = "set-date";
    // Means that it did not start with label therefore it is not a valid input
    if (ci_starts_with(comm, label) == 0)
    {
        return false;
    }
    size_t comm_strlen = strlen(comm);
    //Get the time.
    char comm_cpy[comm_strlen + 1];
    memcpy(comm_cpy, comm, comm_strlen + 1);
    char *date_token = strtok(comm_cpy, " ");
    date_token = strtok(NULL, " ");

    // Date is provided
    if (date_token == NULL)
    {
        println("Date must be provided. MM/DD/YY");
        return true;
    }
    // buffer to save numbers
    char date_array[3][3] = {0};
    str_strip_whitespace(date_token, NULL, 0);
    // if part after set time is not valid with form hh:mm:ss returns with invalid date
    if (split(date_token, '/', 3, date_array, 3) || !is_valid_date_or_time(3, date_array, 3))
    {
        printf("Invalid date! You entered: %s, expecting format: MM/DD/YY!\n", date_token);
        return true;
    }
    // sets the time and returns whether is was sucessful
    unsigned int month = decimal_to_bcd(atoi(date_array[0]));
    unsigned int day = decimal_to_bcd(atoi(date_array[1]));
    unsigned int year = decimal_to_bcd(atoi(date_array[2]));
    if (month < 0x01 | month > 0x12)
    {
        println("Month is out of range 1-12!");
        return true;
    }

    if (year < 0x00 | year > 0x99)
    {
        println("Year is out of range 0-100!");
        return true;
    }

    if (day < 0x01 | day > get_days_in_month((int) month, year))
    {
        println("Day is out of range for that month!");
        return true;
    }
    if (!set_date_clock(month, day, year))
    {
        println("Failed to set date, please try again!");
        return true;
    }

    printf("Set the date to: %s\n", date_token);
    return true;
}


bool cmd_set_time(const char *comm)
{
    const char *label = CMD_SET_TIME_LABEL;
    // Means that it did not start with label therefore it is not a valid input
    if (ci_starts_with(comm, label) == 0)
    {
        return false;
    }
    size_t comm_strlen = strlen(comm);
    //Get the time.
    char comm_cpy[comm_strlen + 1];
    memcpy(comm_cpy, comm, comm_strlen + 1);
    char *time_token = strtok(comm_cpy, " ");
    time_token = strtok(NULL, " ");

    // Date is provided
    if (time_token == NULL)
    {
        println("Time value must be provided. HH:mm:SS");
        return true;
    }
    str_strip_whitespace(time_token, NULL, 0);
    // buffer to save numbers
    char time_array[3][3] = {0};
    // if part after set time is not valid with form hh:mm:ss returns with invalid date
    if (split(time_token, ':', 3, time_array, 3) < 0 ||
            !is_valid_date_or_time(3, time_array, 3))
    {
        printf("Invalid time. You entered: %s, expecting format: HH:mm:SS\n", time_token);
        return true;
    }
    // sets the time and returns whether it was successful
    unsigned char hour = decimal_to_bcd(atoi(time_array[0]));
    unsigned char minute = decimal_to_bcd(atoi(time_array[1]));
    unsigned char second = decimal_to_bcd(atoi(time_array[2]));
    if (hour < 0x00 | hour > 0x23)
    {
        println("Hour is out of range 0-23!");
        return true;
    }
    if (minute < 0x00 | minute > 0x59)
    {
        println("Minutes is out of range 0-59!");
        return true;
    }
    if (second < 0x00 | second > 0x59)
    {
        println("Seconds is out of range 0-59!");
        return true;
    }
    if (!set_time_clock(hour, minute, second))
    {
        println("Failed to set time, please try again!");
        return true;
    }

    printf("Set the time to: %s\n", time_token);
    return true;
}

/**
 * Used to store information on a specific label of the 'help' command.
 */
struct help_info
{
    /**
     * The label of the command for the help message.
     */
    char *str_label;
    /**
     * The help message to send for this.
     */
    char *help_message;
};

/**
 * An array of all help info messages.
 */
struct help_info help_messages[] = {
        {.str_label = CMD_HELP_LABEL, "The '%s' command gives information about specific aspects of the system."},
        {.str_label = CMD_VERSION_LABEL,
                "The '%s' command gives you the version of the OS and the date it was compiled."},
        {.str_label = CMD_SHUTDOWN_LABEL, "The '%s' command prompts the user to shut down the OS."},
        {.str_label = CMD_GET_TIME_LABEL, "The '%s' command gets the current system time in the OS."},
        {.str_label = CMD_SET_TIME_LABEL,
                "The '%s' command allows the use to set the time on the system.\nThe time should follow the format HH:mm:SS."},
        {.str_label = CMD_SET_DATE_LABEL,
                "The '%s' command allows the use to set the date on the system.\nThe date should follow the format MM/DD/YY."}
};

bool cmd_help(const char *comm)
{
    //The command's label.
    const char *label = CMD_HELP_LABEL;

    //Check if it matched.
    int cmp = ci_starts_with(comm, label);
    if (!cmp)
        return false;

    const char *split_label = " ";
    size_t str_len = strlen(comm);

    //Create a copy.
    char comm_cpy[str_len + 1];
    memcpy(comm_cpy, comm, str_len + 1);

    char *spl_token = strtok(comm_cpy, split_label);
    //Bump the token forward.
    int help_m_len = sizeof(help_messages) / sizeof(help_messages[0]);
    if ((spl_token = strtok(NULL, split_label)) != NULL)
    {
        //Try to find help for the specific command.
        for (int i = 0; i < help_m_len; ++i)
        {
            char *help_label = help_messages[i].str_label;

            //Check if the label matches.
            if (strcicmp(help_label, spl_token) != 0)
                continue;

            printf(help_messages[i].help_message, spl_token);
            println("");
            return true;
        }

        //At this point, we didn't find any valid strings.
        printf("Couldn't find any help for '%s'!\n", spl_token);
        return true;
    }

    println("If You want to set the Time for the OS, enter 'Set Time ##:##:##'");
    println("If You want to set the Time for the OS, enter 'Set Date ##/##/##'");
    println("if You want to get the Time for the OS, enter 'Get Time'");
    println("If you need this screen reprompted, re-enter 'help'");
    println("If you want to get the version history, enter 'Version'");
    println("If you want to shutdown, enter 'shutdown' down below");
    println("If you need help in your actual class, dont use Stack Overflow");
    println("Hope this helps!");

    return true;
}