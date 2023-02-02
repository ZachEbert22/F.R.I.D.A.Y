//
// Created by Andrew Bowie on 1/27/23.
//

#ifndef F_R_I_D_A_Y_CLI_H
#define F_R_I_D_A_Y_CLI_H

/**
 * @file cli.h
 * @brief Contains useful commands for interfacing with the CLI.
 */

/**
 * @brief Sets if the CLI is enabled.
 * @param enabled if the CLI should be enabled.
 */
void set_cli_history(bool enabled);

/**
 * @brief If command color formatting should be enabled.
 * @param enabled if it should be enabled.
 */
void set_command_formatting(bool enabled);

#endif //F_R_I_D_A_Y_CLI_H
