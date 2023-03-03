#include "clock.h"
#include "stdio.h"
#include "stdlib.h"



static void create_new_alarm(int[3] time_array, const char* message)
{
    char name[9] = {'a','l','a','r','m','1','\0'};
    int alarm_num = 1;
     bool generated = generate_new_pcb(name, 1, USER, &alarm_function);
     while(!generated)
        {
          alarm_num++;
          if(alarm_num > 99) {
            println("Alarm not created why do u need more than 99 that's a little excessive");
            return;
          }
          if(alarm_num > 9) 
          {
            name[6] = '0' + alarm_num;
          }
          else 
          {
            name[6] = '0' + (alarm_num / 10);
            name[7] = '0' + (alarm_num % 10);
            name[8] = '\0';
          }

          generated = generate_new_pcb(name, 1, USER, &alarm_function);

        }
       
   

    
}

void alarm_function(void)
{
/* This still need to be implemented.
* Get parameters time_array and message from stack
*/
 while(!shouldAlarm(time_array))
    {
        sys_req(IDLE);
    }

    println(message);

    sys_req(EXIT); 
}


bool shouldAlarm(int time_array[3])
{
    // get current time
    int time_buf[7] = {0};
    time_buf = get_time(time_buf)
    // index 4-6 is hours - seconds
    if(time_buf[4] < time_arrar[0]) return false;
    if(time_buf[5] < time_array[1]) return false;
    if(time_buf[6] < time_array[2]) return false;

    return true;
}

