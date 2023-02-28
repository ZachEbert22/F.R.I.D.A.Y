
#include "commands.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "memory.h"
#include "processes.h"
#include <stdint.h>
#include "mpx/pcb.h"

bool loadr3(const char *comm){

    //The command's label.
    const char *label = "loadr3";

    //Check if it matched.
    if (!first_label_matches(comm, label))
        return false;
    
    for (int i = 0; i < 5; i++){\
        void *p = NULL;
        switch(i){
            case 0:
                p = proc1;
                break;
            case 1:
                p = proc2;
                break;
            case 2:
                p = proc3;
                break;
            case 3:
                p = proc4;
                break;
            case 4:
                p = proc5;
                break;
        }
        char name[3] = {0};
        itoa(i, name, 2);
        generate_new_pcb(name, 1, USER, p);
        //pcb* loading_pcb = pcb_create_cmd("pcb create process USER 1");
    }
   
    return true;
}