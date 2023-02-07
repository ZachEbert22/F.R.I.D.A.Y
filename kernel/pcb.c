
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
static linked_list *running_pcb_queue;

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

struct pcb *pcb_alloc(void)
{
    setup_queue();

    struct pcb *pcb_ptr = sys_alloc_mem(sizeof (struct pcb));
    if(pcb_ptr == NULL) return NULL;
    memset(pcb_ptr->stack,0,sizeof(pcb_ptr->stack));
    pcb_ptr->stack_ptr = sizeof(pcb_ptr->stack)-1;
    return pcb_ptr;
}

int pcb_free(struct pcb* pcb_ptr)
{
    setup_queue();

    if(pcb_ptr == NULL)
        return 1;

    return sys_free_mem(pcb_ptr);
}

struct pcb *pcb_setup(const char *name, int class, int priority)
{
    setup_queue();

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

    //We need to malloc the string,
    size_t str_len = strlen(name);
    char *malloc_name = sys_alloc_mem(str_len + 1);
    if(malloc_name == NULL)
        return NULL;
    memcpy(malloc_name, name, str_len + 1);

    pcb_ptr->name = malloc_name;
    pcb_ptr->process_class = class;
    pcb_ptr->priority = priority;
    return pcb_ptr;
}

void pcb_insert(struct pcb* pcb_ptr)
{
    setup_queue();

    if(pcb_ptr == NULL) return;
    add_item(running_pcb_queue,pcb_ptr);
}
/**
 *
 * @param name
 * @return
 * @authors Jared Crowley
 */
struct pcb *pcb_find(const char *name)
{
    setup_queue();

    // get size of linked list
    int size = list_size(running_pcb_queue);

    for (int i = 0; i < size; ++i) {
        struct pcb* pcb_ptr = get_item(running_pcb_queue, i);
        if(strcmp(pcb_ptr->name, name) == 0)
            return pcb_ptr;
    }
    return NULL;
}
/**
 *
 * @param name
 * @return
 * @authors Jared Crowley
 */
int pcb_remove(struct pcb *name)
{
    setup_queue();

    // get size of linked list
    int size = list_size(running_pcb_queue);

    for (int i = 0; i < size; ++i) {
        struct pcb* pcb_ptr = get_item(running_pcb_queue, i);
        if(pcb_ptr == name) {
            remove_item(running_pcb_queue,i);
            return 0;
        }
    }
    return 1;
}

///The label for the create label.
#define CMD_CREATE_LABEL "create"
#define CMD_DELETE_LABEL "delete"
#define CMD_BLOCK_LABEL "block"
#define CMD_UNBLOCK_LABEL "unblock"
#define CMD_SUSPEND_LABEL "suspend"
#define CMD_RESUME_LABEL "resume"
#define CMD_SETPRIORITY_LABEL "priority"
#define CMD_SHOW_LABEL "show"

/**
 * The 'create' sub command.
 * @param comm the string command.
 * @return true if it matched, false if not.
 */
bool pcb_create_cmd(const char *comm)
{
    if(!first_label_matches(comm, CMD_CREATE_LABEL))
        return false;

    //Copy the string.
    size_t str_len = strlen(comm);
    char comm_cpy[str_len + 1];
    memcpy(comm_cpy, comm, str_len + 1);

    //Tokenize the string.
    char *token = strtok(comm_cpy, " ");
    //Push it forward.
    token = strtok(NULL, " ");

    //Initialize pointers, all null for error checking.
    char *name = NULL;
    int class = -1;
    int priority = -1;

    if(token == NULL)
    {
        println("Missing Arguments! Do it like this: 'pcb create (name) (class) (priority)'");
        return true;
    }

    //Copy the name.
    size_t token_size = strlen(token);
    if(token_size > PCB_MAX_NAME_LEN)
    {
        printf("Invalid Argument! '%s' exceeds maximum name length of %d!\n", token, PCB_MAX_NAME_LEN);
        return true;
    }

    if(pcb_find(token) != NULL)
    {
        printf("Invalid Argument! The PCB '%s' already exists!\n", token);
        return true;
    }

    char name_cpy[token_size + 1];
    memcpy(name_cpy, token, token_size + 1);
    name = name_cpy;

    token = strtok(NULL, " ");
    if(token == NULL)
    {
        println("Missing Arguments! Do it like this: 'pcb create (name) (class) (priority)'");
        return true;
    }

    //Check which type it was.
    if(strcicmp(token, "USER") == 0)
    {
        class = USER;
    }
    else if(strcicmp(token, "SYSTEM") == 0)
    {
        class = SYSTEM;
    }
    else
    {
        printf("Invalid Argument! %s isn't a valid class! Try 'USER' or 'SYSTEM'!\n", token);
        return true;
    }

    token = strtok(NULL, " ");
    if(token == NULL)
    {
        println("Missing Arguments! Do it like this: 'pcb create (name) (class) (priority)'");
        return true;
    }

    priority = atoi(token);
    if(priority < 0 || priority > 9)
    {
        printf("Invalid Argument! %d isn't a valid priority! Try 0-9 instead.\n", priority);
        return true;
    }

    //Alloc the pcb.
    struct pcb *pcb_ptr = pcb_setup(name, class, priority);
    if(pcb_ptr == NULL)
    {
        println("There was an error setting up the PCB!");
        return true;
    }

    //Insert it.
    pcb_insert(pcb_ptr);

    printf("Successfully created a new PCB with the following info.\nName: %s\nClass: %s\nPriority: %d\n",
           name,
           class == 0 ? "USER" : "SYSTEM",
           priority);
    return true;
}

/**
 * The 'delete' sub command.
 * @param comm the string command.
 * @return true if it matched, false if not.
 */
bool pcb_delete_cmd(const char *comm)
{
    if(!first_label_matches(comm, CMD_DELETE_LABEL))
        return false;

    //Copy the command.
    size_t s_len = strlen(comm);
    char comm_cpy[s_len + 1];
    memcpy(comm_cpy, comm, s_len + 1);

    //Tokenize.
    char *token = strtok(comm_cpy, " ");
    token = strtok(NULL, " ");

    if(token == NULL)
    {
        println("Missing Arguments! Do it like this: 'pcb delete (name)'");
        return true;
    }

    //Find the PCB.
    struct pcb *pcb_ptr = pcb_find(token);
    if(pcb_ptr == NULL)
    {
        printf("Could not find PCB named '%s'!\n", token);
        return true;
    }

    if(pcb_ptr->process_class == SYSTEM)
    {
        println("You cannot delete PCBs with the 'SYSTEM' class.");
        return true;
    }

    pcb_remove(pcb_ptr);
    printf("Removed PCB named '%s'!\n", pcb_ptr->name);
    return true;
}

/**
 * The 'unblock' sub command.
 * @param comm the string command.
 * @return true if it matched, false if not.
 * @author Kolby Eisenhauer
 */
bool pcb_unblock_cmd(const char* comm)
{
     if(!first_label_matches(comm, CMD_UNBLOCK_LABEL))
        return false;
    size_t comm_strlen = strlen(comm);
    char comm_cpy[comm_strlen + 1];
    memcpy(comm_cpy, comm, comm_strlen + 1);
    char *name_token = strtok(comm_cpy, " ");
    name_token = strtok(NULL, " ");
    struct pcb* pcb_ptr = pcb_find(name_token);
    if(pcb_ptr == NULL) {
        printf("PCB with name: %s, not found\n",name_token);
        return true;
    }
    if(pcb_ptr->exec_state != BLOCKED)
    {
        printf("PCB %s is not blocked\n",name_token);
        return true;
    }

    pcb_ptr->exec_state = READY;
    pcb_remove(pcb_ptr);
    pcb_insert(pcb_ptr);
        printf("The pcb named: %s was unblocked\n", pcb_ptr->name);
    return true;
}

/**
 * The 'block' sub command.
 * @param comm the string command.
 * @return true if it matched, false if not.
 * @author Kolby Eisenhauer
 */
bool pcb_block_cmd(const char* comm)
{
    if(!first_label_matches(comm, CMD_BLOCK_LABEL))
        return false;
    size_t comm_strlen = strlen(comm);
    
    char comm_cpy[comm_strlen + 1];
    memcpy(comm_cpy, comm, comm_strlen + 1);
    char *name_token = strtok(comm_cpy, " ");
    name_token = strtok(NULL, " ");
    struct pcb* pcb_ptr = pcb_find(name_token);
    if(pcb_ptr == NULL) {
        printf("PCB with name: %s, not found\n",name_token);
        return true;
    }
    if(pcb_ptr->exec_state == BLOCKED)
    {
        printf("PCB %s is blocked already\n", name_token);
        return true;
    }

    pcb_ptr->exec_state = BLOCKED;
    pcb_remove(pcb_ptr);
    pcb_insert(pcb_ptr);
        printf("The pcb named: %s was blocked\n", pcb_ptr->name);
    return true;
}

/**
 * The 'suspend' sub command.
 * @param comm the string command.
 * @return true if it matched, false if not.
 * @author Kolby Eisenhauer
 */
bool pcb_suspend_cmd(const char* comm)
{
    if(!first_label_matches(comm, CMD_SUSPEND_LABEL))
        return false;
    size_t comm_strlen = strlen(comm);

    char comm_cpy[comm_strlen + 1];
    memcpy(comm_cpy, comm, comm_strlen + 1);
    char *name_token = strtok(comm_cpy, " ");
    name_token = strtok(NULL, " ");
    struct pcb* pcb_ptr = pcb_find(name_token);
    if(pcb_ptr == NULL) {
        printf("PCB with name: %s, not found\n",name_token);
        return true;
    }
    if(pcb_ptr->process_class == SYSTEM)
    {
        printf("PCB %s is a system class PCB cannot be suspended by user\n",name_token);
        return true;
    }

    pcb_ptr->dispatch_state = SUSPENDED;
    pcb_remove(pcb_ptr);
    pcb_insert(pcb_ptr);
    printf("The pcb named: %s was suspended\n", pcb_ptr->name);
    return true;
}
bool pcb_resume_cmd(const char* comm)
{
    if(!first_label_matches(comm, CMD_RESUME_LABEL))
        return false;
    size_t comm_strlen = strlen(comm);

    char comm_cpy[comm_strlen + 1];
    memcpy(comm_cpy, comm, comm_strlen + 1);
    char *name_value = strtok(comm_cpy, " ");
    name_value = strtok(NULL, " ");

    struct pcb* pcb_ptr = pcb_find(name_value);
    if(pcb_ptr == NULL) {
        printf("PCB with name: %s, cannot be found \n",name_value);
        return true;
    }
    if(pcb_ptr->process_class == SYSTEM)
    {
        printf("PCB %s is a system class PCB cannot be suspended nor resumed by user\n",name_value);
        return true;
    }

    pcb_ptr->dispatch_state = NOT_SUSPENDED;
    pcb_remove(pcb_ptr);
    pcb_insert(pcb_ptr);
    printf("The pcb named: %s was resumed\n", pcb_ptr->name);
    return true;
}
bool pcb_priority_cmd(const char* comm){
    if(!first_label_matches(comm, CMD_SETPRIORITY_LABEL))
        return false;
    size_t comm_strlen = strlen(comm);

    char comm_cpy[comm_strlen + 1];
    memcpy(comm_cpy, comm, comm_strlen + 1);
    char *parameters = strtok(comm_cpy, " ");
    parameters = strtok(NULL, " ");

    struct pcb* pcb_ptr = pcb_find(parameters);
    if(pcb_ptr == NULL) {
        printf("PCB with name: %s, cannot be found \n", parameters);
        return true;
    }

    parameters = strtok(NULL, " ");
    int priority;
    if(parameters != NULL && parameters[0] <= '9'  && parameters[0] >= '0') {
        priority = atoi(parameters);
    }else{
        println("Priority is Invalid: Priority must be a number");
        return true;
    }

    if(priority > 9 || priority < 0){
        println("The Number is Out of Range. Enter a Number between 0-9");
        return true;
    }

    pcb_ptr->priority = priority;
    pcb_remove(pcb_ptr);
    pcb_insert(pcb_ptr);
    printf("The pcb named: %s was changed to priority %d\n", pcb_ptr->name, pcb_ptr->priority);
    return true;
}

// pcb_show(const char* comm){

// }
///All commands within this file, terminated with NULL.
static bool (*command[])(const char *) = {
        &pcb_create_cmd,
        &pcb_delete_cmd,
        &pcb_block_cmd,
        &pcb_unblock_cmd,
        &pcb_suspend_cmd,
        &pcb_resume_cmd,
        &pcb_priority_cmd,
        NULL,
};

void exec_pcb_cmd(const char *comm)
{
    size_t str_len = strlen(comm);
    char comm_cpy[str_len + 1];
    memcpy(comm_cpy, comm, str_len + 1);

    str_strip_whitespace(comm_cpy, NULL, 0);

    int index = 0;
    while(command[index] != NULL)
    {
        bool result = command[index](comm_cpy);
        if(result)
            return;
        index++;
    }

    //Inform the user that there wasn't any matches.
    if(strlen(comm) > 0)
        printf("PCB sub command '%s' does not exist! Type 'help pcb' for more info!\n", comm_cpy);
    else
        println("Please provide a PCB sub command!");
}
