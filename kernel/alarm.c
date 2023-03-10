#include "stdio.h"
#include "stddef.h"
#include "mpx/pcb.h"
#include "string.h"
#include "mpx/clock.h"
#include "sys_req.h"
#include "mpx/pcb.h"
#include "stdlib.h"

/**
 * @file alarm.c
 * @brief Contains logic to create alarms for the OS.
 */

///The parameters used to pass into the alarm function.
typedef struct alarm_params
{
    ///A pointer to where the time is stored.
    int *time_ptr;
    ///A pointer to where the message is stored.
    char *str_ptr;
    ///The data to store.
    unsigned char buffer[75];
} alarm_structure;

bool shouldAlarm(const int *time_array)
{
    // get current time
    int *time_buf = get_time(NULL);
    adj_timezone(time_buf, get_clock_timezone()->tz_hour_offset, get_clock_timezone()->tz_minute_offset);
    // index 4-6 is hours - seconds
    if (time_array[0] < time_buf[4]) return true;
    // printf("alarm time is %d:%d:%d current time is %d:%d:%d\n", time_array[0], time_array[1], time_array[2], time_buf[4], time_buf[5], time_buf[6]);
    if (time_array[0] == time_buf[4] && time_array[1] < time_buf[5]) return true;
    if (time_array[0] == time_buf[4] && time_array[1] == time_buf[5] && time_array[2] < time_buf[6]) return true;

    return false;
}

/**
 * @brief The alarm function used by the alarm processes.
 *
 * @param time_array the time array to go off at.
 * @param message the message to send to the user.
 * @authors Kolby Eisenhauer
 */
void alarm_function(int *time_array, const char *message)
{
/* This still need to be implemented.
* Get parameters time_array and message from stack
*/
    while (!shouldAlarm(time_array))
    {
        sys_req(IDLE);
    }

    println(message);
    sys_req(EXIT);
}

///The alarm count used to make sure names are unique.
static int alarms = 0;

bool create_new_alarm(int *time_array, const char *message)
{
    char name[20] = {'a', 'l', 'a', 'r', 'm', '\0'};

    //Prepare the parameters for the function.
    alarm_structure parameters = {0};
    parameters.str_ptr = (char *) parameters.buffer;
    size_t len = strlen(message);
    strcpy(parameters.str_ptr, message, len);
    parameters.time_ptr = (int *) (parameters.buffer + len + 2);
    memcpy(parameters.time_ptr, time_array, 3 * sizeof(int));

    //Prepare the process' name.
    size_t name_len = 15;
    char process_name[name_len];
    do {
        memset(process_name, 0, name_len);
        strcpy(process_name, name, -1);
        char num_buf[10] = {0};
        itoa(alarms, num_buf, 9);
        strcpy(process_name + strlen(name), num_buf, -1);
        alarms++;
    }while(pcb_find(process_name) != NULL);

    //Generate the actual PCB.
    bool generated = generate_new_pcb(process_name, 1, USER, &alarm_function, (char *) &parameters, sizeof(alarm_structure), 2);
    return generated;
}

