
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
#include "memory.h"
#include "mpx/pcb.h"
#include "linked_list.h"

///The PCB queue.
static const linked_list *pcb_queue;

struct pcb *pcb_alloc(void)
{
    struct pcb *pcb_ptr = sys_alloc_mem(sizeof (struct pcb));
    return pcb_ptr;
}

int pcb_free(struct pcb* pcb_ptr)
{
    if(pcb_ptr == NULL)
        return 1;

    return sys_free_mem(pcb_ptr);
}

struct pcb *pcb_setup(const char *name, int class, int priority)
{
    //Don't allow null names or names that are too long.
    if(name == NULL || strlen(name) > PCB_MAX_NAME_LEN)
        return NULL;

    //Check validity of class.
    if(class < USER || class > SYSTEM)
        return NULL;

    //Check validity of priority.
    if(priority < 0 || priority > 9)
        return NULL;

    struct pcb *pcb_ptr = pcb_alloc();
    if(pcb_ptr == NULL)
        return NULL;

    pcb_ptr->name = name;
    pcb_ptr->process_class = class;
    pcb_ptr->priority = priority;
}
//pcb_allocoate
//pcb_free
//pcb* pcb_setup
//pcb* pcb_find
//pcb_insert
//pcb_remove
