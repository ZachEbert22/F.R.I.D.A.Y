//
// Created by Andrew Bowie on 1/18/23.
//

#include "commands.h"
#include "string.h"
#include "stdio.h"
#include "mpx/get_set_time.h"

bool version(const char *comm)
{
        //The command's label.
        const char *label = "Version";

        //Check if it matched.
        int cmp = strcmp(comm, label);
        if(cmp != 0)
                return false;

        println("Module: R1");
        println(__DATE__);
        //println(__TIME__);
        return true;
}

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

bool help(const char *comm)
{
        //The command's label.
        const char *label = "help";

        //Check if it matched.
        int cmp = strcicmp(comm, label);
        if(cmp != 0)
                return false;

        println("If you want to set the Time for the OS, enter 'Set Time'");
        println("If you want to get the Time for the OS, enter 'Get Time'");
        println("If you need this screen reprompted, re-enter 'help'");
        println("If you want to get the version history, enter 'Version'");
        println("If you want to shutdown, enter 'shutdown' down below");
        println("If you need help in your actual class, dont use Stack Overflow");
        println("Hope this helps!");

        return true;
}