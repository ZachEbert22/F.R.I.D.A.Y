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

int get_time(){
    printf("%d ", get_index(YEAR));
    printf("%d ", get_index(MONTH));
    printf("%d ", get_index(DATE));
    printf("%d ", get_index(DAY));
    printf("%d ", get_index(HOURS)-2);
    printf("%d ", get_index(MINUTES)-4);
    printf("%d\n", get_index(SECONDS));

    return 0;
}

int get_index(int a){
    outb(0x70, a);
    int bits = inb(0x71);

    int fixed = ((bits >> 4) & 0xF) * 10;
    fixed = fixed + (bits & 0xF);

    return fixed;
}