#ifndef MPX_SERIAL_H
#define MPX_SERIAL_H

#include <stddef.h>
#include <mpx/device.h>

/**
 @file mpx/serial.h
 @brief Kernel functions and constants for handling serial I/O
*/

/**
 Initializes devices for user input and output
 @param device A serial port to initialize (COM1, COM2, COM3, or COM4)
 @return 0 on success, non-zero on failure
*/
int serial_init(device dev);

/**
 Initializes devices for user input and output
 @param device A serial port to initialize (COM1, COM2, COM3, or COM4)
 @return 0 on success, non-zero on failure
*/
int serial_open(device dev, int speed);

/**
 Writes a buffer to a serial port
 @param device The serial port to output to
 @param buffer A pointer to an array of characters to output
 @param len The number of bytes to write
 @return The number of bytes written
*/
int serial_out(device dev, const char *buffer, size_t len);

/**
 * @brief Writes len bytes from the given buffer to the device.
 * @param dev the device to write to.
 * @param buffer the buffer to read from.
 * @param len the amount of bytes to write.
 * @return the number of bytes written.
 */
int serial_write(device dev, char *buffer, size_t len);

/**
 * @brief Reads input on the given device.
 *
 * @param dev the device to read on.
 * @param buf the buffer to read with.
 * @param len the length of the buffer.
 * @return 0 on success, negative values on error.
 */
int serial_read(device dev, char *buf, size_t len);

/**
 Reads a string from a serial port
 @param device The serial port to read data from
 @param buffer A buffer to write data into as it is read from the serial port
 @param count The maximum number of bytes to read
 @return The number of bytes read on success, a negative number on failure
*/   		   

int serial_poll(device dev, char *buffer, size_t len);

#endif
