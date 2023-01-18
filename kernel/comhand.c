//
// Created by Andrew Bowie on 1/13/23.
//

#include <sys_req.h>
#include "stdio.h"
#include "string.h"
#include "stdbool.h"
#include "mpx/comhand.h"
#include "mpx/get_set_time.h"

#define CMD_PROMPT ">> "

/**
 * The version command, used to handle when the user asks for a version number. Must Include Compilation date
 * @param comm_version the command string.
 * @return true if the command was handled, false if not.
 */
bool version(const char *comm)
{
        //The command's label.
        const char *label = "version";

        //Check if it matched.
        int cmp = strcmp(comm, label);
        if(cmp != 0)
                return false;

        println("Module: R1");
        println(__DATE__);
        //println(__TIME__);
        return true;
}

/**
 * The version command, used to handle when the user asks for a version number. Must Include Compilation date
 * @param comm_version the command string.
 * @return true if the command was handled, false if not.
 */
bool get_time_menu(const char *comm)
{
        //The command's label.
        const char *label = "get time";

        //Check if it matched.
        int cmp = strcmp(comm, label);
        if(cmp != 0)
                return false;

        get_time();
        //println(__TIME__);
        return true;
}


/**
 * The help command, used to help the user when they are struggling.
 * @param comm_version the command string.
 * @return help true if it was handled, false if not.
 */
bool help(const char *comm)
{
        //The command's label.
        const char *label = "help";

        //Check if it matched.
        int cmp = strcicmp(comm, label);
        if(cmp != 0)
                return false;

        println("If You want to set the Time for the OS, enter 'Set Time'");
        println("if You want to get the Time for the OS, enter 'Get Time'");
        println("If you need this screen reprompted, re-enter 'help'");
        println("if you want to shutdown, enter 'shutdown' down below");
        println("if you need help in your actual class, dont use Stack Overflow");
        println("Hope this helps!");
        
        return true;
}
/**
 * @brief An array of pointers to command functions. These functions
 * will return true if the command was handled by them, or false if not.
 */
bool (*comm_funcs[])(const char *comm) = {
        &version,
        &help,
        &get_time_menu
};

void comhand(void)
{
        println("Welcome to MPX. Please select an option");

        println("=> Help");
        println("=> Set Time");
        println("=> Get Time");
        println("=> version");
        println("=> shutdown");
        for(;;)
        {
                //100 + 1 for the null terminator.
                char buf[101] = {0};

                print(CMD_PROMPT);
                sys_req(READ, COM1, buf, 100);

                //Handle all functions.
                int comm_func_count = sizeof (comm_funcs) /
                        sizeof (comm_funcs[0]);

                if (strcmp(buf, "shutdown") == 0){
                        println("Are you sure you want to shutdown? enter y for yes or N for no");
                        memset(buf,'\0', 101);
                        sys_req(READ, COM1, buf, 5); 
                        if (strcmp(buf, "y") == 0 || strcmp(buf, "yes") == 0 || strcmp(buf, "Yes") == 0){
                                return;
                        }else{
                                continue;
                        }
                }

                for (int i = 0; i < comm_func_count; ++i)
                {
                        bool result = comm_funcs[i](buf);
                        if(result)
                                break;
                }


        }
}
