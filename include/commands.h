//
// Created by Andrew Bowie on 1/18/23.
//

#ifndef F_R_I_D_A_Y_COMMANDS_H
#define F_R_I_D_A_Y_COMMANDS_H

#include "stdbool.h"

/**
 * @file This file contains headers for commands run by the command
 * handler.
 */

/**
 * The version command, used to handle when the user asks for a version number. Must Include Compilation date
 * @param comm_version the command string.
 * @return true if the command was handled, false if not.
 */
bool cmd_version(const char *comm);

/**
 * @brief The shutdown command. If ran, will re-prompt the user for confirmation.
 * @param comm the command string.
 * @return true if the command was handled, false if not.
 */
bool cmd_shutdown(const char *comm);

/**
 * The get time command, used to get the time on the system.
 * @param comm_version the command string.
 * @return true if the command was handled, false if not.
 */
bool cmd_get_time_menu(const char *comm);

/**
 * The help command, used to help the user when they are struggling.
 * @param comm_version the command string.
 * @return help true if it was handled, false if not.
 */
bool cmd_help(const char *comm);

/**
 * The set time command, used to set time user wants
 * @param comm_version the command string.
 * @return help true if it was handled, false if not.
 */
bool cmd_set_time(const char* comm);
/**
 * The set date command, used to set time user wants
 * @param comm_version the command string.
 * @return help true if it was handled, false if not.
 */
bool cmd_set_date(const char* comm);

bool fix_tmz(const char *comm);

#endif //F_R_I_D_A_Y_COMMANDS_H
