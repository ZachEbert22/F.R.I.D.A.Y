#include "mpx/pcb.h"
#include "string.h"
#include "sys_req.h"

/**
 * @file sys_call.c
 * @brief This file contains the sys_call function which is used to do context switching.
 */

///The currently running PCB.
static struct pcb *active_pcb_ptr = NULL;
///The first context saved when sys_call is called.
static struct context *first_context_ptr = NULL;

/**
 * @brief The main system call function, implementing the IDLE, EXIT, and SHUTDOWN system requests.
 * @param action the action to perform.
 * @param ctx the current PCB context.
 * @return a pointer to the next context to load.
 * @author Andrew Bowie,  Zachary Ebert, Kolby Eisenhauer
 */
struct context *sys_call(op_code action, struct context *ctx)
{
    if(first_context_ptr == NULL)
    {
        first_context_ptr = ctx;
    }

    //Handle different actions in their own way.
    switch (action)
    {
        case IDLE:
        {
            struct pcb *next = peek_next_pcb();
            //If this is the case, no PCB is ready to be loaded.
            if (next == NULL || next->exec_state == BLOCKED || next->dispatch_state == SUSPENDED)
            {
                return ctx;
            }

            poll_next_pcb();
            struct pcb *current = active_pcb_ptr;
            active_pcb_ptr = next;
            struct context *new_ctx = (struct context *) next->stack_ptr;
            if (current != NULL)
            {
                current->exec_state = READY;
                pcb_insert(current);
                //Update where the PCB's context pointer is pointing.
                current->stack_ptr = ctx;
            }
            next->exec_state = RUNNING;
            return new_ctx;
        }
        case EXIT:
        {
            //Exiting PCB.
            struct pcb *exiting_pcb = active_pcb_ptr;
            if(exiting_pcb == NULL) //We can't exit if there's no PCB.
                return ctx;

            pcb_remove(exiting_pcb);
            struct pcb *next_to_load = peek_next_pcb();
            if(next_to_load == NULL || next_to_load->exec_state == BLOCKED || next_to_load->dispatch_state == SUSPENDED) //No next process to load? Try loading the global one.
                return first_context_ptr;

            //Ready the next process.
            poll_next_pcb();
            next_to_load->exec_state = RUNNING;
            active_pcb_ptr = next_to_load;

            //Free the old one.
            pcb_free(exiting_pcb);
            return (struct context *) next_to_load->stack_ptr;
        }
        case SHUTDOWN:
        {
            if(active_pcb_ptr != NULL && active_pcb_ptr->process_class != SYSTEM) //Only system processes may invoke a shutdown.
                return ctx;

            //This should kill all active processes and return the proper value.
            struct pcb *next = NULL;
            while((next = poll_next_pcb()) != NULL)
            {
                pcb_remove(next);
                pcb_free(next);
            }

            //Free the active pointer.
            if(active_pcb_ptr != NULL)
            {
                pcb_remove(active_pcb_ptr);
                pcb_free(active_pcb_ptr);
            }
            active_pcb_ptr = NULL;
            return first_context_ptr;
        }
        default:
            return ctx;
    }
}