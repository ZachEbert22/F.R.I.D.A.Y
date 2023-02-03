
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

///The PCB queue for processes.
static const linked_list *running_pcb_queue;

/**
 * A pointer for comparing PCBs.
 * @param ptr1
 * @param ptr2
 * @return
 */
int pcb_cmpr(void *ptr1, void *ptr2)
{
    struct pcb *pcb_ptr1 = (struct pcb *) ptr1;
    struct pcb *pcb_ptr2 = (struct pcb *) ptr2;

    if(pcb_ptr1->exec_state == BLOCKED || pcb_ptr1->dispatch_state == SUSPENDED)
        return 1;

    if(pcb_ptr2->exec_state == BLOCKED || pcb_ptr2->dispatch_state == SUSPENDED)
        return -1;

    return pcb_ptr1->priority - pcb_ptr2->priority;
}

/**
 * @brief Sets up the backing pcb queues.
 */
void setup_queue()
{
    if(running_pcb_queue != NULL)
        return;

    running_pcb_queue = nl_unbounded();
    set_sort_func((linked_list *) running_pcb_queue, &pcb_cmpr);
}

//pcb_allocoate
struct pcb *pcb_alloc(void)
{
    struct pcb *pcb_ptr = sys_alloc_mem(sizeof (struct pcb));
    memset(pcb_ptr->stack,0,sizeof(pcb_ptr->stack));
    pcb_ptr->stack_ptr = sizeof(pcb_ptr->stack)-1;
    return pcb_ptr;
}

//pcb_free
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
    return pcb_ptr;
}
//pcb* pcb_setup
//pcb* pcb_find
//pcb_insert
//pcb_remove
