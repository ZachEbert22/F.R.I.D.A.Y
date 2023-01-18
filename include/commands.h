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
bool version(const char *comm);

/**
 * The version command, used to handle when the user asks for a version number. Must Include Compilation date
 * @param comm_version the command string.
 * @return true if the command was handled, false if not.
 */
bool get_time_menu(const char *comm);

/**
 * The help command, used to help the user when they are struggling.
 * @param comm_version the command string.
 * @return help true if it was handled, false if not.
 */
bool help(const char *comm);

/**
 * The set time command, used to set time user wants
 * @param comm_version the command string.
 * @return help true if it was handled, false if not.
 */
bool set_time(const char* comm);
/**
 * The set date command, used to set time user wants
 * @param comm_version the command string.
 * @return help true if it was handled, false if not.
 */
bool set_date(const char* comm);

#endif //F_R_I_D_A_Y_COMMANDS_H
