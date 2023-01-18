//
// Created by Andrew Bowie on 1/13/23.
//

#ifndef F_R_I_D_A_Y_COMHAND_H
#define F_R_I_D_A_Y_COMHAND_H

#define CMD_PROMPT ">> "

/**
 * @brief Signals to the command handler that it should stop whenever it
 * gets the chance.
 */
void signal_shutdown(void);

/**
 * @brief Starts the command handler.
 */
void comhand(void);

#endif //F_R_I_D_A_Y_COMHAND_H
