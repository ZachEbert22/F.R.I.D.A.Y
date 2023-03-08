#include "stdio.h"
#include "stddef.h"
#include "mpx/pcb.h"
#include "string.h"
#include "mpx/clock.h"
#include "sys_req.h"
#include "mpx/pcb.h"

typedef struct alarm_params
{
    int *time_ptr;
    char *str_ptr;
    unsigned char buffer[55];
} alarm_structure;

bool shouldAlarm(int *time_array)
{
    // get current time
    int *time_buf = get_time(NULL);
    // index 4-6 is hours - seconds
    if (time_array[0] < time_buf[4]) return true;
    // printf("alarm time is %d:%d:%d current time is %d:%d:%d\n", time_array[0], time_array[1], time_array[2], time_buf[4], time_buf[5], time_buf[6]);
    if (time_array[0] == time_buf[4] && time_array[1] < time_buf[5]) return true;
    if (time_array[0] == time_buf[4] && time_array[1] == time_buf[5] && time_array[2] < time_buf[6]) return true;

    return false;
}

void alarm_function(int *time_array, const char *message)
{
/* This still need to be implemented.
* Get parameters time_array and message from stack
*/
    while (!shouldAlarm(time_array))
    {
        println("idling");
        sys_req(IDLE);
    }

    println(message);
    println("Exiting alarm");
    sys_req(EXIT);
}


void create_new_alarm(int *time_array, const char *message)
{
    char name[9] = {'a', 'l', 'a', 'r', 'm', '1', '\0'};
    int alarm_num = 1;
    alarm_structure parameters = {0};
    parameters.str_ptr = (char *) parameters.buffer;
    size_t len = strlen(message);
    strcpy(parameters.str_ptr, message, len);
    parameters.time_ptr = (int *) (parameters.buffer + len + 2);

    memcpy(parameters.time_ptr, time_array, 3 * sizeof(int));
    bool generated = generate_new_pcb(name, 1, USER, &alarm_function, (char *) &parameters, sizeof(alarm_structure), 2);
    while (!generated)
    {
        alarm_num++;
        if (alarm_num > 99)
        {
            println("Alarm not created why do u need more than 99 that's a little excessive");
            return;
        }
        if (alarm_num > 9)
        {
            name[6] = '0' + alarm_num;
        } else
        {
            name[6] = '0' + (alarm_num / 10);
            name[7] = '0' + (alarm_num % 10);
            name[8] = '\0';
        }

        generated = generate_new_pcb(name, 1, USER, &alarm_function, (char *) &parameters, sizeof(alarm_structure), 2);

    }
    println("alarm generated");


}

