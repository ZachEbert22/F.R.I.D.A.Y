//
// Created by Andrew Bowie on 1/18/23.
//

#include "commands.h"
#include "string.h"
#include "stdio.h"
#include "sys_req.h"
#include "mpx/comhand.h"
#include "mpx/serial.h"
#include "mpx/clock.h"
#include "stdlib.h"

#define CMD_HELP_LABEL "help"
#define CMD_VERSION_LABEL "version"
#define CMD_SHUTDOWN_LABEL "shutdown"
#define CMD_GET_TIME_LABEL "get-time"
#define CMD_SET_TIMEZONE_LABEL "set-timezone"
#define CMD_SET_TIME_LABEL "set-time"
#define CMD_SET_DATE_LABEL "set-date"

/**
 * @brief Checks if the given command matches the label.
 * @param comm the command.
 * @param label the label.
 * @return if the command matches the label.
 */
bool matches_cmd(const char *comm, const char *label)
{
    //Create a copy.
    size_t comm_len = strlen(comm);
    char comm_cpy[comm_len + 1];
    memcpy(comm_cpy, comm, comm_len + 1);

    //Token the command.
    char *str_token = strtok(comm_cpy, " ");
    return strcicmp(str_token, label) == 0;
}


bool cmd_version(const char *comm)
{
    //The command's label.
    const char *label = CMD_VERSION_LABEL;

    //Check if it matched.
    if (!matches_cmd(comm, label))
        return false;

    println("Module: R1");
    println(__DATE__);
    println(__TIME__);
    return true;
}

bool cmd_shutdown(const char *comm)
{
    const char *label = CMD_SHUTDOWN_LABEL;

    if (!matches_cmd(comm, label))
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
    if (!matches_cmd(comm, label))
        return false;

    print_time();
    //println(__TIME__);
    return true;
}

bool cmd_set_date(const char *comm)
{
    const char *label = CMD_SET_DATE_LABEL;
    // Means that it did not start with label therefore it is not a valid input
    if (!matches_cmd(comm, label))
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
    unsigned char month_dec = atoi(date_array[0]);
    unsigned char day_dec = atoi(date_array[1]);
    unsigned char year_dec = atoi(date_array[2]);

    //Get the current time.
    int current_time[7] = {0};
    get_time(current_time);

    //Update the hours and minutes.
    current_time[0] = year_dec;
    current_time[1] = month_dec;
    current_time[2] = day_dec;
    adj_timezone(current_time,
                 -get_clock_timezone()->tz_hour_offset,
                 -get_clock_timezone()->tz_minute_offset);
    year_dec = current_time[0];
    month_dec = current_time[1];
    day_dec = current_time[2];

    unsigned int month = decimal_to_bcd(month_dec);
    unsigned int day = decimal_to_bcd(day_dec);
    unsigned int year = decimal_to_bcd(year_dec);
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

    if (day < 0x01 | day > get_days_in_month((int) month, (int) year))
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
    if (!matches_cmd(comm, label))
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
    unsigned char hour_dec = atoi(time_array[0]);
    unsigned char minute_dec = atoi(time_array[1]);
    unsigned char second_dec = atoi(time_array[2]);

    //Get the current time.
    int current_time[7] = {0};
    get_time(current_time);

    //Update the hours and minutes.
    current_time[4] = hour_dec;
    current_time[5] = minute_dec;
    adj_timezone(current_time,
                 -get_clock_timezone()->tz_hour_offset,
                 -get_clock_timezone()->tz_minute_offset);
    hour_dec = current_time[4];
    minute_dec = current_time[5];

    unsigned char hour = decimal_to_bcd(hour_dec);
    unsigned char minute = decimal_to_bcd(minute_dec);
    unsigned char second = decimal_to_bcd(second_dec);
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
        {.str_label = CMD_HELP_LABEL,
                .help_message = "The '%s' command gives information about specific aspects of the system."},
        {.str_label = CMD_VERSION_LABEL,
                .help_message = "The '%s' command gives you the version of the OS and the date it was compiled."},
        {.str_label = CMD_SHUTDOWN_LABEL,
                .help_message = "The '%s' command prompts the user to shut down the OS."},
        {.str_label = CMD_GET_TIME_LABEL,
                .help_message = "The '%s' command gets the current system time in the OS."},
        {.str_label = CMD_SET_TIME_LABEL,
                .help_message = "The '%s' command allows the use to set the time on the system.\nThe time should follow the format HH:mm:SS."},
        {.str_label = CMD_SET_DATE_LABEL,
                .help_message = "The '%s' command allows the use to set the date on the system.\nThe date should follow the format MM/DD/YY."},
        {.str_label = CMD_SET_TIMEZONE_LABEL,
                .help_message = "The '%s' command allows the user to set the timezone for the system.\nOnly a select set of American timezones are provided."}
};

bool cmd_help(const char *comm)
{
    //The command's label.
    const char *label = CMD_HELP_LABEL;

    //Check if it matched.
    if (!matches_cmd(comm, label))
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

bool cmd_set_tz(const char *comm)
{
    const char *label = CMD_SET_TIMEZONE_LABEL;
    if (!matches_cmd(comm, label))
    {
        return false;
    }

    //Create a copy.
    size_t str_len = strlen(comm);
    char comm_cpy[str_len + 1];
    memcpy(comm_cpy, comm, str_len + 1);

    char tz_buf[10] = {0};
    char *tz_token = strtok(comm_cpy, " ");

    //Advance the token forward.
    tz_token = strtok(NULL, " ");
    if(tz_token != NULL)
    {
        memcpy(tz_buf, tz_token, 9);
    }
    else
    {
        //Prompt the user again.
        println("What time zone do you want to be in?");

        //Iterate over the timezone.
        const time_zone_t **all_tzs = get_all_timezones();
        int printed = 0;
        while(all_tzs[printed] != NULL)
        {
            const time_zone_t *tz_ptr = all_tzs[printed];
            printf("=> %s (%s)\n", tz_ptr->tz_label, tz_ptr->tz_longformat);
            printed++;
        }
        set_cli_history(0);
        print(": ");
        sys_req(READ, COM1, tz_buf, 9);
        set_cli_history(1);
    }

    //Check it against all other timezones.
    const time_zone_t *tz_ptr = get_timezone(tz_buf);
    if(tz_ptr == NULL)
    {
        printf("Timezone '%s' not recognized!\n", tz_buf);
        return true;
    }

    set_timezone(tz_ptr);
    printf("Set the timezone to '%s'!\n", tz_buf);
    return true;
}