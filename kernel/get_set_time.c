#include <sys_req.h>
#include "stdio.h"
#include "string.h"
#include "stdbool.h"
#include "mpx/comhand.h"
#include "mpx/get_set_time.h"
#include <mpx/io.h>
#include <ctype.h>
#include <mpx/interrupts.h>

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
    printf("%d ", minutes);
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
    printf("%s, %d/%d/%d @ %d:%d:%d\n", week, month, date, year, hours, minutes, seconds);
    return 0;
}
   
    
int get_index(int a){
    outb(0x70, a);
    int bits = inb(0x71);

    int fixed = ((bits >> 4) & 0xF) * 10;
    fixed = fixed + (bits & 0xF);

    return fixed;
}
int getDaysInMonth(int month, int year){
        switch(month){
                case 1:
                        return 0x31;
                case 2:
                        if(year % 4 == 0) return 0x29;
                        return 0x28;
                case 3: 
                        return 0x31;
                case 4: 
                        return 0x30;
                case 5:
                        return 0x31;
                case 6:
                        return 0x30;
                case 7: 
                        return 0x31;
                case 8:
                        return 0x31;
                case 9: 
                        return 0x30;
                case 0x10:
                        return 0x31;
                case 0x11:
                        return 0x30;
                case 0x12: 
                        return 0x31;
        }
        return 0x00;
}

bool isValidTimeOrDate(const char* date, unsigned int buf[], int buf_len, char c){
    int index = 0;
    int colonCount = 0;
    int count = 0;
    while(isspace(*date)) date++;
    while(*date != '\0') {
    if(index >= buf_len) return false;
    if(isdigit(*date)) {
    buf[index++] = todigit(*date);
    count++;
    date++;
    continue; 
    }
    if(*date == c && count == 2) {
    count = 0;
    colonCount++;
    date++;
    continue;
    }
    if(isspace(*date)){
        break;
    }
    return false;
}

    buf[index+1] = '\0';
    if(index == 6 && colonCount < 3) return true;
    return false;
}

bool set_time_clock(unsigned int hr, unsigned int min, unsigned int sec){
    cli();
    outb(0x70,0x04);
    outb(0x71,hr);
    outb(0x70,0x02);
    outb(0x71,min);
    outb(0x70,0x00);
    outb(0x71,sec);
    sti();
    return true;
}
bool set_date_clock(unsigned int month, unsigned int day, unsigned int year){
    cli();

    outb(0x70,0x07);
    outb(0x71,day); 
    outb(0x70,0x08);
    outb(0x71,month);
    outb(0x70,0x09);
    outb(0x71,year);

    sti();
    return true;
}



unsigned char decimalToBCD(unsigned int first, unsigned int second){
    return first + second;
}