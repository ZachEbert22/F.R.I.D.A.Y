#ifndef F_R_I_D_A_Y_ALARM_H
#define F_R_I_D_A_Y_ALARM_H

/**
 * @brief Creates a new pcb that will display message at or after given time
 * @param time_array the time to display message
 * @param message message to display
 * @author Kolby Eisenhauer
 */
void create_new_alarm(int *time_array, const char* message);

void alarm_function(int *time_array, const char* message);
/**
 * @brief Checks current time to see if alarm should alert
 * @param time_array the time to display message
 * @return boolean if should alarm
 * @author Kolby Eisenhauer
 */
bool shouldAlarm(int *time_array);
#endif
