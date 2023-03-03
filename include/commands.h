//
// Created by Andrew Bowie on 1/18/23.
//

#ifndef F_R_I_D_A_Y_COMMANDS_H
#define F_R_I_D_A_Y_COMMANDS_H

#include "stdbool.h"

/**
 * @file commands.h
 * @brief This file contains headers for commands run by the command
 * handler.
 */

/**
 * @brief Finds the best match for the given command,
 *        or NULL if it doesn't match OR matches multiple OR is equal to the command.
 * @param cmd the command.
 * @return the best match for it.
 * @authors Andrew Bowie
 */
const char *find_best_match(const char *cmd);

/**
 * @brief Checks if the given command exists.
 * @param cmd the command to check for.
 * @return true if it does, false if not.
 * @authors Andrew Bowie
 */
bool command_exists(const char *cmd);

/**
 * @brief The version command, used to handle when the user asks for a version number. Must Include Compilation date
 * @param comm the command string.
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
 * @brief The get time command, used to get the time on the system.
 * @param comm the command string.
 * @return true if the command was handled, false if not.
 */
bool cmd_get_time_menu(const char *comm);

/**
 * @brief The help command, used to help the user when they are struggling.
 * @param comm the command string.
 * @return true if it was handled, false if not.
 */
bool cmd_help(const char *comm);

/**
 * @brief The set time command, used to set time user wants
 * @param comm the command string.
 * @return true if it was handled, false if not.
 */
bool cmd_set_time(const char* comm);

/**
 * @brief The set date command, used to set time user wants
 * @param comm the command string.
 * @return true if it was handled, false if not.
 */
bool cmd_set_date(const char* comm);

/**
 * @brief The set timezone command, used to set the system timezone.
 * @param comm the command string.
 * @return true if it was handled, false if not.
 */
bool cmd_set_tz(const char *comm);

/**
 * @brief The clear command, used to clear the console.
 * @param comm the command string.
 * @return true if it was handled, false if not.
 */
bool cmd_clear(const char *comm);

/**
 * @brief The color command, used to change text color for the terminal.
 * @param comm the command string.
 * @return true if it was handled, false if not.
 */
bool cmd_color(const char *comm);

/**
 * @brief the yield command, causes the command handler to yield immediately.
 * @param comm the command string.
 * @return true if it was handled, false if not.
 */
bool cmd_yield(const char *comm);


/**
 * @brief The pcb command, used to interact with the pcb system.
 * @param comm the command string.
 * @return true if it was handled, false if not.
 */
bool cmd_pcb(const char *comm);

bool cmd_alarm(const char *comm);
#endif //F_R_I_D_A_Y_COMMANDS_H
