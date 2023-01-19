//
// Created by Andrew Bowie on 1/18/23.
//

#include "commands.h"
#include "string.h"
#include "stdio.h"
#include "sys_req.h"
#include "mpx/comhand.h"
#include "mpx/get_set_time.h"
#include "stdlib.h"

bool cmd_version(const char *comm)
{
        //The command's label.
        const char *label = "Version";

        //Check if it matched.
        int cmp = strcicmp(comm, label);
        if (cmp != 0)
                return false;

        println("Module: R1");
        println(__DATE__);
        //println(__TIME__);
        return true;
}

bool cmd_shutdown(const char *comm)
{
        const char *label = "shutdown";

        int cmp = strcicmp(comm, label);
        if (cmp != 0)
                return false;

        println("Are you sure you want to shutdown? enter y for yes or N for no");
        char confirm_buf[6] = {0};
        print(CMD_PROMPT);
        sys_req(READ, COM1, confirm_buf, 5);

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
        const char *label = "get time";

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
        const char *label = "set date ";
        char date[10] = {0};
        split_once_after(comm, label,date,10);
        // Means that it did not start with label therefore it is not a valid input
        if (ci_starts_with(comm, label) == 0)
        {
                return false;
        }
        // Date is provided
        if (strlen(date) == 0)
        {
                println("Date value must be provided");
                return false;
        }
        // buffer to save numbers
        char time_array[3][3] = {0};
        // if part after set time is not valid with form hh:mm:ss returns with invalid date
        if (split(date, '/',3,time_array, 3) || !is_valid_date_or_time(3,time_array,3))
        {
                println("Invalid date");
                return true;
        }
        // sets the time and returns whether is was sucessful
        unsigned int month = decimal_to_bcd(atoi(time_array[0]));
        unsigned int day = decimal_to_bcd(atoi(time_array[1]));
        unsigned int year = decimal_to_bcd(atoi(time_array[2]));
        if (month < 0x01 | month > 0x12)
        {
                println("Month is out of range 1-12");
                return true;
        }
       
        if (year < 0x00 | year > 0x99)
        {
                println("Year is out of range 0-100");
                return true;
        }
        
        if (day < 0x01 | day > get_days_in_month((int) month, year))
        {
                println("Day is out of range for that month");
                return true;
        }
        if (!set_date_clock(month, day, year))
        {
                println("failed to set date");
        }
        return true;
}


bool cmd_set_time(const char *comm)
{
        const char *label = "set time ";
        char date[10] = {0};
        split_once_after(comm, label,date,10);
        // Means that it did not start with label therefore it is not a valid input
        if (ci_starts_with(comm, label) == 0)
        {
                return false;
        }
        // Date is provided
        if (strlen(date) == 0)
        {
                println("Date value must be provided");
                return false;
        }
        char stripped_date[10] = {0};
        str_strip_whitespace(date, stripped_date, 10);
        // buffer to save numbers
        char time_array[3][3] = {0};
        // if part after set time is not valid with form hh:mm:ss returns with invalid date
        if (split(stripped_date,':', 3,time_array, 3) < 0 || !is_valid_date_or_time(3,time_array,3))
        {
                println("Invalid date, please try again");
                return true;
        }
        // sets the time and returns whether is was sucessful
        unsigned char hour = decimal_to_bcd(atoi(time_array[0]));
        unsigned char minute = decimal_to_bcd(atoi(time_array[1]));
        unsigned char second = decimal_to_bcd(atoi(time_array[2]));
        printf("%d,%d,%d", hour, minute, second);
        if (hour < 0x00 | hour > 0x23)
        {
                println("Hour is out of range 0-23");
                return true;
        }
        if (minute < 0x00 | minute > 0x59)
        {
                println("Minutes is out of range 0-59");
                return true;
        }
        if (second < 0x00 | second > 0x59)
        {
                println("Seconds is out of range 0-59");
                return true;
        }
        if (!set_time_clock(hour, minute, second))
        {
                println("failed to set time");
        }
        return true;
}

bool cmd_help(const char *comm)
{
        //The command's label.
        const char *label = "help";

        //Check if it matched.
        int cmp = strcicmp(comm, label);
        if (cmp != 0)
                return false;

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