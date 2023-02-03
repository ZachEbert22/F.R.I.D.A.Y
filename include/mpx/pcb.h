#ifndef MPX_PCB_H
#define MPX_PCB_H

#define PCB_MAX_NAME_LEN 8
#define PCB_STACK_SIZE 4096

///The clas of a PCB.
enum pcb_class {
    USER = 0,
    SYSTEM = 1,
};

///The execution state of a PCB.
enum pcb_exec_state {
    READY = 0,
    RUNNING = 1,
    BLOCKED = 2,
};

///An enum of dispatch state for PCBs.
enum pcb_dispatch_state {
    SUSPENDED = 0,
    NOT_SUSPENDED = 1,
};

///The definition of a process control block.
struct pcb {
    ///The name of the PCB, max length of 8.
    const char *name;
    ///The process class type.
    enum pcb_class process_class;
    ///Integer priority of PCB, 0-9, lower = higher priority;
    int priority;
    ///The execution state of this PCB.
    enum pcb_exec_state exec_state;
    ///The dispatch state of this PCB.
    enum pcb_dispatch_state dispatch_state;
    ///A pointer to the next available byte in the stack.
    int stack_ptr;
    ///The stack itself.
    unsigned char stack[PCB_STACK_SIZE];
};
/** 
*@brief Sets up queue for PCBS
*/
void setup_queue(void);

/**
 * @brief Allocates memory for a PCB block.
 *
 * @return A pointer to the allocated PCB.
 * @authors Andrew Bowie, Kolby Eisenhauer
 */
struct pcb *pcb_alloc(void);

/**
 * @brief Frees the memory associated with the given PCB block.
 *
 * @param pcb_ptr the pointer to the pcb.
 * @return 0 on success, non-zero on failure.
 * @authors Andrew Bowie
 */
int pcb_free(struct pcb* pcb_ptr);

/**
 * @brief Sets up a PCB with the given information.
 *
 * @param name the name of the PCB, cannot be longer than @code PCB_MAX_NAME_LEN chars.
 * @param class the class of the PCB.
 * @param priority the priority of the PCB.
 * @return the created PCB, or NULL on error.
 * @authors Andrew Bowie
 */
struct pcb *pcb_setup(const char *name, int class, int priority);

/**
* @brief Inserts a PCB into appropriate queue, based on state and priority
* @param pcb_ptr pointer to pcb
* @return void
* @authors Kolby Eisenhauers
*/
void pcb_insert(struct pcb* pcb_ptr);

/**
 *
 * @param name
 * @return
 * @authors Jared Crowley
 */
struct pcb *pcb_find(const char *name);

/**
 *
 * @param name
 * @return
 * @authors Jared Crowley
 */
int pcb_remove(struct pcb *name);


#endif