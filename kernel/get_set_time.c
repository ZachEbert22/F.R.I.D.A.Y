#include <sys_req.h>
#include "stdio.h"
#include "string.h"
#include "stdbool.h"
#include "mpx/comhand.h"
#include "math.h"
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
    printf("%02d ", year);
    int month = get_index(MONTH);
    printf("%02d ", month);
    int date = get_index(DATE);
    printf("%02d ", date);
    int day_of_week = get_index(DAY);
    printf("%02d ", day_of_week);
    int hours = get_index(HOURS) ; // converts to eastern time
    int hour_adj = hours -5;
    printf("%02d ", hour_adj);
    int minutes = get_index(MINUTES);
    int min_adj = minutes;
    if (min_adj > 59){
        min_adj =min_adj-60;
        hour_adj = hour_adj + 1;  
    }
    if (min_adj < 0){
        min_adj =min_adj+60;
        hour_adj = hour_adj - 1;  
    }
    if (hour_adj > 23){
            hour_adj = hour_adj-24;
            date = date + 1;
        }
    if (hour_adj < 0){
            hour_adj = 24+hour_adj;
            date = date - 1;
    }
    if (day_of_week > 7){
            hour_adj = hour_adj-7;
            
        }
    if (hour_adj < 1){
            hour_adj = 24+7;
            
    }
    printf("%02d ", minutes);
        int seconds = get_index(SECONDS);
    printf("%02d\n", seconds);

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
        printf("%s, %02d/%02d/%02d @ %02d:%02d:%02d\n", week, month, date, year, hour_adj, minutes, seconds);
    return 0;
}


int get_index(int a){
    outb(0x70, a);
    int bits = inb(0x71);

    int fixed = ((bits >> 4) & 0xF) * 10;
    fixed = fixed + (bits & 0xF);

    return fixed;
}

unsigned int get_days_in_month(int month, int year){
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

bool is_valid_date_or_time(int word_len,char buf[][word_len], int buff_len)
{
    for(int i = 0; i < buff_len; i++){
        for(int j = 0; j < word_len;j++){
            if(!isdigit(buf[i][j])) return false;
        }
    }
    return true;
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



unsigned char decimal_to_bcd(unsigned int decimal){
    unsigned int first_half = decimal / 10;
    unsigned int second_half = decimal % 10;
    return ((first_half << 4) + second_half);
}