//
// Created by Andrew Bowie on 1/18/23.
//

#include "commands.h"
#include "string.h"
#include "stdio.h"
#include "sys_req.h"
#include "mpx/comhand.h"
#include "mpx/get_set_time.h"

bool cmd_version(const char *comm)
{
        //The command's label.
        const char *label = "version";

        //Check if it matched.
        int cmp = strcicmp(comm, label);
        if(cmp != 0)
                return false;

        println("Module: R1");
        println(__DATE__);
        //println(__TIME__);
        return true;
}

bool cmd_shutdown(const char *comm)
{
        const char *label = "shutdown";

        int cmp = strcicmp(comm, label);
        if(cmp != 0)
                return false;

        println("Are you sure you want to shutdown? enter y for yes or N for no");
        char confirm_buf[6] = {0};
        print(CMD_PROMPT);
        sys_req(READ, COM1, confirm_buf, 5);

        //Check confirmation.
        if (strcicmp(confirm_buf, "y") == 0 ||
                        strcicmp(confirm_buf, "yes") == 0){
                signal_shutdown();
        }
        return true;
}

bool cmd_get_time_menu(const char *comm)
{
        //The command's label.
        const char *label = "get time";

        //Check if it matched.
        int cmp = strcicmp(comm, label);
        if(cmp != 0)
                return false;

        get_time();
        //println(__TIME__);
        return true;
}

bool cmd_help(const char *comm)
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