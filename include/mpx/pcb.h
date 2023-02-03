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

#endif