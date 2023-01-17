#include <sys_req.h>
#include "stdio.h"
#include "string.h"
#include "stdbool.h"
#include "mpx/comhand.h"
#include "mpx/get_set_time.h"
#include <mpx/io.h>

#define YEAR 0x09
#define MONTH 0x08
#define DATE 0x07
#define DAY 0x06
#define HOURS 0x04
#define MINUTES 0x02
#define SECONDS 0x00

 //Tuesday, 1/17/23  @ 09:08:04
int get_time(){
    int year = get_index(YEAR);
    printf("%d ", year);
    int month = get_index(MONTH);
    printf("%d ", month);
    int date = get_index(DATE);
    printf("%d ", date);
    int day_of_week = get_index(DAY);
    printf("%d ", day_of_week);
    int hours = get_index(HOURS);
    int hour_adj = hours + 0;
    printf("%d ", hour_adj);
    int minutes = get_index(MINUTES);
    int min_adj = minutes + 27;
    if (min_adj > 59){
        min_adj =min_adj-60;
        hour_adj = hour_adj + 1;
    
        if (hour_adj > 23){
            hour_adj = hour_adj-24;
            date = date + 1;
        }
    }
    printf("%d ", min_adj);
    int seconds = get_index(SECONDS);
    printf("%d\n", seconds);
   
    char* week;
    if (day_of_week == 1){
         week = "Sunday";
    } else if (day_of_week == 2){
        week = "Monday";
    } else if (day_of_week == 3){
         week = "Tuesday";
    } else if (day_of_week == 4){
        week = "Wednesday";
    }else if (day_of_week == 5){
        week = "Thursday";
    }else if (day_of_week == 6){
        week = "Friday";
    }else{
        week = "Saturday";
    }
    printf("%s\n", week);
    printf("%s, %d/%d/%d @ %d:%d:%d\n", week, month, date, year, hour_adj, min_adj, seconds);
    return 0;
}
   
    
int get_index(int a){
    outb(0x70, a);
    int bits = inb(0x71);

    int fixed = ((bits >> 4) & 0xF) * 10;
    fixed = fixed + (bits & 0xF);

    return fixed;
}