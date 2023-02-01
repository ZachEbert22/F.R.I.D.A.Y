//
// Created by Andrew Bowie on 1/18/23.
//

#include "commands.h"
#include "string.h"
#include "stdio.h"
#include "sys_req.h"
#include "bomb_catcher.h"
#include "mpx/comhand.h"
#include "mpx/clock.h"
#include "stdlib.h"
#include "color.h"
#include "cli.h"

#define CMD_HELP_LABEL "help"
#define CMD_VERSION_LABEL "version"
#define CMD_SHUTDOWN_LABEL "shutdown"
#define CMD_GET_TIME_LABEL "get-time-date"
#define CMD_SET_TIMEZONE_LABEL "set-timezone"
#define CMD_SET_TIME_LABEL "set-time"
#define CMD_SET_DATE_LABEL "set-date"
#define CMD_CLEAR_LABEL "clear"
#define CMD_COLOR_LABEL "color"

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
    gets(confirm_buf, 5, false);
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
        println("Date must be provided. enter 'set-date MM/DD/YY'");
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
        println("Time value must be provided. Try 'set-time HH:mm:SS'");
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

    //Do some error checking
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

///Used to store information on a specific label of the 'help' command.
struct help_info
{
    /**
     * @brief The label of the command for the help message.
     */
    char *str_label;
    /**
     * @brief The help message to send for this struct.
     */
    char *help_message;
};

/**
 * @brief An array of all help info messages.
 */
struct help_info help_messages[] = {
        {.str_label = CMD_HELP_LABEL,
                .help_message = "The '%s' command gives information about specific aspects of the system.\nIf you need further help, type 'help'"},
        {.str_label = CMD_VERSION_LABEL,
                .help_message = "The '%s' command gives you the version of the OS and the date it was compiled.\nType 'version', to get the version!"},
        {.str_label = CMD_SHUTDOWN_LABEL,
                .help_message = "The '%s' command prompts the user to shut down the OS.\nType 'shutdown' to turn off the machine!"},
        {.str_label = CMD_GET_TIME_LABEL,
                .help_message = "The '%s' command gets the current system time in the OS.\nType 'get-time' to get the time!"},
        {.str_label = CMD_SET_TIME_LABEL,
                .help_message = "The '%s' command allows the use to set the time on the system.\nThe time should follow the format HH:mm:SS.\nTo start changing the time, enter 'set-time'"},
        {.str_label = CMD_SET_DATE_LABEL,
                .help_message = "The '%s' command allows the use to set the date on the system.\nThe date should follow the format MM/DD/YY.\nTo start changing the date, enter 'set-date'"},
        {.str_label = CMD_SET_TIMEZONE_LABEL,
                .help_message = "The '%s' command allows the user to set the timezone for the system.\nMost North American and European Time Zones Are provided\nTo fix the timezone, type 'set-timezone'"},
        {.str_label = CMD_CLEAR_LABEL,
                .help_message = "The '%s' command clears the screen.\nto clear your terminal, enter 'clear'"},
        {.str_label = CMD_COLOR_LABEL,
                .help_message = "The '%s' command sets the color of text output.\nto change your color, enter 'color'"}
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

    char *spl_token = strtok(comm_cpy, split_label); //First Time
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
    //All the help function, and the possible functions that are associated to it
    println("If You want to set the Time for the OS, enter 'help set-time'");
    println("If You want to set the Time for the OS, enter 'help set-date'");
    println("if You want to get the Time for the OS, enter 'help get-time'");
    println("If you want to know what the help does, enter 'help help'");
    println("if you need help setting the timezone enter 'help set-timezone'");
    println("If you want to get the version history, enter 'help version'");
    println("If you help to shutdown, enter 'help shutdown' down below");
    println("if you wnt to know what the command color does, enter 'help color'");
    println("If you want to know what clear does, enter 'help clear'");
    //println("If you need help in a class, dont use Stack Overflow");
    //println("Hope this helps!");
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
    if(tz_token == NULL)
    {
        //Prompt the user again.
        println("What time zone do you want to be in?");

        //Iterate over the timezone.
        const time_zone_t **all_tzs = get_all_timezones();
        int printed = 0;
        while(all_tzs[printed] != NULL)
        {
            const time_zone_t *tz_ptr = all_tzs[printed];
            printf("=> %s (%s - %s)\n", tz_ptr->tz_label, tz_ptr->tz_longformat, tz_ptr->tz_city);
            printed++;
        }
        set_cli_history(0);
        print(": ");
        gets(tz_buf, 9, false);
        set_cli_history(1);
    }
    else
    {
        strcpy(tz_buf, tz_token, 9);
    }

    //Check it against all other timezones.
    const time_zone_t *tz_ptr = get_timezone(tz_buf);
    if(tz_ptr == NULL)
    {
        printf("Timezone '%s' not recognized!\n", tz_buf);
        return true;
    }
    

    set_timezone(tz_ptr);
    printf("Set the timezone to '%s'!\n", tz_ptr->tz_longformat);
    return true;
}

bool cmd_clear(const char *comm)
{
    //Check if the label matches.
    if(!matches_cmd(comm, CMD_CLEAR_LABEL))
        return false;

    clearscr();
    return true;
}

bool cmd_color(const char *comm)
{
    if(!matches_cmd(comm, CMD_COLOR_LABEL))
        return false;

    size_t comm_len = strlen(comm);
    char comm_cpy[comm_len + 1];
    memcpy(comm_cpy, comm, comm_len + 1);
    char *token = strtok(comm_cpy, " ");
    //Advance the token
    token = strtok(NULL, " ");

    char input[15] = {0};
    if(token == NULL)
    {
        //Print all the available colors.
        const color_t **colors = get_colors();
        const color_t *current_color = get_output_color();

        int index = 0;
        while(colors[index] != NULL)
        {
            print("=> ");

            //Print the color.
            set_output_color(colors[index]);
            print(colors[index]->color_label);
            set_output_color(current_color);

            println("");
            index++;
        }
        print(": ");

        gets(input, 14, false);
    }
    else
    {
        strcpy(input, token, 14);
    }

    const color_t *color = get_color(input);
    if(color == NULL)
    {
        printf("The color '%s' is not defined!\n", input);
        return true;
    }

    set_output_color(color);
    printf("Set the color to '%s'!\n", color->color_label);
    return true;
}