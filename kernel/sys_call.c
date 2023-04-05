#include "mpx/pcb.h"
#include "sys_req.h"
#include "linked_list.h"
#include "mpx/device.h"

/**
 * @file sys_call.c
 * @brief This file contains the sys_call function which is used to do context switching.
 */

///A descriptor for a device.
typedef struct {
    ///The device this control block is describing.
    device dev;
    ///Whether or not the control block is allocated.
    bool allocated;
    ///The operation this device is currently doing.
    op_code operation;
    ///Whether or not there is an event to be handled.
    bool event;
    ///A list of all pending IOCBs.
    linked_list *iocb_queue;
    ///The total length of the ring buffer.
    size_t buf_len;
    ///The beginning of the ring buffer.
    char *r_buffer_start;
    ///The read pointer for the ring buffer.
    int read_ptr;
    ///The write pointer for the ring buffer.
    int write_ptr;
} dcb_t;

///A descriptor for pending IO operations.
typedef struct {
    ///A pointer to the device this IOCB belongs to.
    dcb_t *device;
    ///A pointer to the process this IOCB belongs to.
    struct pcb *pcb;
    ///The operation this IOCB is attempting.
    op_code operation;
    ///The length of the buffer.
    size_t buf_len;
    ///The buffer.
    char *buffer;
} iocb_t;

///The currently running PCB.
static struct pcb *active_pcb_ptr = NULL;
///The first context saved when sys_call is called.
static struct context *first_context_ptr = NULL;

/**
 * @brief The main system call function, implementing the IDLE and EXIT system requests.
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
        case READ:
        {
            //First, read in all necessary registers.
            int ebx = 0, ecx = 0, edx = 0;
            __asm__ volatile("mov %%ebx,%0" : "=r"(ebx));
            __asm__ volatile("mov %%ecx,%0" : "=r"(ecx));
            __asm__ volatile("mov %%edx,%0" : "=r"(edx));
            return 0;
        }
        case WRITE:
        {
            //First, read in all necessary registers.
            int ebx = 0, ecx = 0, edx = 0;
            __asm__ volatile("mov %%ebx,%0" : "=r"(ebx));
            __asm__ volatile("mov %%ecx,%0" : "=r"(ecx));
            __asm__ volatile("mov %%edx,%0" : "=r"(edx));
            return 0;
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
        default:
            return ctx;
    }
}