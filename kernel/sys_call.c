#include "mpx/pcb.h"
#include "sys_req.h"
#include "linked_list.h"
#include "mpx/device.h"
#include "mpx/serial.h"

/**
 * @file sys_call.c
 * @brief This file contains the sys_call function which is used to do context switching.
 */

///The currently running PCB.
static struct pcb *active_pcb_ptr = NULL;
///The first context saved when sys_call is called.
static struct context *first_context_ptr = NULL;

/**
 * @brief Want to check if next PCB is blocked, unblocked, IDLE, NULL, etc
 * @param ctx the current PCB context.
 * @param next_state the new state of the PCB.
 * @return Pointer to the next context struct
 * @author Zachary Ebert
 */
struct context *next_pcb(struct context *ctx, enum pcb_exec_state next_state)
{
    struct pcb *next = peek_next_pcb();
    //If this is the case, no PCB is ready to be loaded.
    if (next == NULL || next->exec_state == BLOCKED || next->dispatch_state == SUSPENDED)
    {
        return ctx;
    }
    //Peaks and polls the pcb
    poll_next_pcb();
    struct pcb *present_pcb = active_pcb_ptr;
    active_pcb_ptr = next;
    struct context *new_ctx = (struct context *) next->stack_ptr;
    //Checks to see if the active pointer pcb is null
    if (present_pcb != NULL)
    {
        present_pcb->exec_state = next_state;
        pcb_insert(present_pcb);
        //Update where the PCB's context pointer is pointing.
        present_pcb->stack_ptr = ctx;
    }
    next->exec_state = RUNNING;
    return new_ctx;
}

/**
 * @brief The main system call function, implementing the IDLE and EXIT system requests.
 * @param action the action to perform.
 * @param ctx the current PCB context.
 * @return a pointer to the next context to load.
 * @author Andrew Bowie,  Zachary Ebert, Kolby Eisenhauer
 */
struct context *sys_call(op_code action, struct context *ctx)
{
    if (first_context_ptr == NULL)
    {
        first_context_ptr = ctx;
    }

    int ebx = 0, ecx = 0, edx = 0;
    __asm__ volatile("mov %%ebx,%0" : "=r"(ebx));
    __asm__ volatile("mov %%ecx,%0" : "=r"(ecx));
    __asm__ volatile("mov %%edx,%0" : "=r"(edx));

    //First, we need to check for completed IO operations.
    struct pcb *to_load = check_completed();
    if (to_load != NULL)
    {
        //We need to context switch to this PCB.
        to_load->exec_state = RUNNING;
        pcb_remove(to_load);
        active_pcb_ptr->exec_state = READY;
        active_pcb_ptr->stack_ptr = ctx;

        //Throw the current PCB back onto the queue.
        pcb_insert(active_pcb_ptr);
        active_pcb_ptr = to_load;
        return (struct context *) to_load->stack_ptr;
    }

    //Handle different actions in their own way.
    switch (action)
    {
        case READ:
        {
            device dev = (device) ebx;
            char *buffer = (char *) ecx;
            size_t bytes = (size_t) edx;
            io_req_result result = io_request(active_pcb_ptr, action, dev, buffer, bytes);

            if (result == INVALID_PARAMS || result == SERVICED)
                return ctx;

            //In this case, we need to move this device to a blocked state and CTX switch.
            if (result == PARTIALLY_SERVICED || result == DEVICE_BUSY)
            {
                return next_pcb(ctx, BLOCKED);
            }
            return ctx;
        }
        case WRITE:
        {
            device dev = (device) ebx;
            char *buffer = (char *) ecx;
            size_t bytes = (size_t) edx;
            io_req_result result = io_request(active_pcb_ptr, action, dev, buffer, bytes);

            if (result == INVALID_PARAMS || result == SERVICED)
                return ctx;

            //In this case, we need to move this device to a blocked state and CTX switch.
            if (result == PARTIALLY_SERVICED || result == DEVICE_BUSY)
            {
                return next_pcb(ctx, BLOCKED);
            }
            return ctx;
        }
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
            if (exiting_pcb == NULL) //We can't exit if there's no PCB.
                return ctx;

            pcb_remove(exiting_pcb);
            struct pcb *next_to_load = peek_next_pcb();
            if (next_to_load == NULL || next_to_load->exec_state == BLOCKED ||
                next_to_load->dispatch_state == SUSPENDED) //No next process to load? Try loading the global one.
                return first_context_ptr;

            //Ready the next process.
            poll_next_pcb();
            next_to_load->exec_state = RUNNING;
            active_pcb_ptr = next_to_load;

            //Free the old one.
            pcb_free(exiting_pcb);
            return (struct context *) next_to_load->stack_ptr;
        }
        default:
            return ctx;
    }
}