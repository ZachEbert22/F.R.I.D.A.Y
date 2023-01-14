#include <mpx/io.h>
#include <mpx/serial.h>
#include <sys_req.h>
#include "stdlib.h"
#include "string.h"
#include "stdio.h"

enum key_code {
        BACKSPACE = 8,
        CARRIAGE_RETURN = 13,
        ESCAPE = 27,
        SPACE = 32, //The minimum keycode value for normal characters.
        TILDA = 126, //The maximum keycode value for normal characters.
        DELETE = 127,
};

enum direction {
        LEFT = 0,
        RIGHT = 1,
};

enum uart_registers {
	RBR = 0,	// Receive Buffer
	THR = 0,	// Transmitter Holding
	DLL = 0,	// Divisor Latch LSB
	IER = 1,	// Interrupt Enable
	DLM = 1,	// Divisor Latch MSB
	IIR = 2,	// Interrupt Identification
	FCR = 2,	// FIFO Control
	LCR = 3,	// Line Control
	MCR = 4,	// Modem Control
	LSR = 5,	// Line Status
	MSR = 6,	// Modem Status
	SCR = 7,	// Scratch
};

static int initialized[4] = { 0 };

static int serial_devno(device dev)
{
	switch (dev) {
	case COM1: return 0;
	case COM2: return 1;
	case COM3: return 2;
	case COM4: return 3;
	}
	return -1;
}

int serial_init(device dev)
{
	int dno = serial_devno(dev);
	if (dno == -1) {
		return -1;
	}
	outb(dev + IER, 0x00);	//disable interrupts
	outb(dev + LCR, 0x80);	//set line control register
	outb(dev + DLL, 115200 / 9600);	//set bsd least sig bit
	outb(dev + DLM, 0x00);	//brd most significant bit
	outb(dev + LCR, 0x03);	//lock divisor; 8bits, no parity, one stop
	outb(dev + FCR, 0xC7);	//enable fifo, clear, 14byte threshold
	outb(dev + MCR, 0x0B);	//enable interrupts, rts/dsr set
	(void)inb(dev);		//read bit to reset port
	initialized[dno] = 1;
	return 0;
}

int serial_out(device dev, const char *buffer, size_t len)
{
	int dno = serial_devno(dev);
	if (dno == -1 || initialized[dno] == 0) {
		return -1;
	}
	for (size_t i = 0; i < len; i++) {
		outb(dev, buffer[i]);
	}
	return (int)len;
}

/**
 * @brief Moves the text cursor back the given amount of spaces.
 * @param right 1 if we should move it right, 0 if left.
 * @param spaces the amount to move the cursor back.
 */
void move_cursor(enum direction right, int spaces)
{
        char full_len_str[20] = {0};
        itoa((int) spaces, full_len_str, 20);

        char m_left_prefix[3] = {
                ESCAPE,
                '[',
                '\0'
        };
        print(m_left_prefix);
        print(full_len_str);
        print(right ? "C" : "D");
}

int serial_poll(device dev, char *buffer, size_t len)
{
	// insert your code to gather keyboard input via the technique of polling.
	// You must validate each key and handle special keys such as delete, back-space, and
	// arrow keys

        size_t bytes_read = 0;
        int line_pos = 0;
        while(bytes_read < len)
        {
                //Check the LSR.
                if((inb(dev + LSR) & 1) == 0)
                        continue;

                int beginning_pos = line_pos;

                char read_char = inb(dev);

                //Get the keycode and check it against known characters.
                int keycode = (int) read_char;
                char k_str[20] = {0};
                itoa(keycode, k_str, 20);

                if(keycode >= SPACE && keycode <= TILDA)
                {
                        //Copy the current characters forward.
                        for (int i = (int) bytes_read; i > line_pos; --i)
                        {
                                buffer[i] = buffer[i - 1];
                        }

                        buffer[line_pos++] = read_char;
                        bytes_read++;
                }

                if(keycode == CARRIAGE_RETURN)
                {
                        buffer[bytes_read] = '\0';
                        break;
                }

                //Handle backspace and delete.
                if(keycode == BACKSPACE || keycode == DELETE)
                {
                        if(line_pos == 0)
                                continue;

                        //Delete the character.
                        buffer[--line_pos] = '\0';

                        //Copy down the new characters.
                        for (int i = 0; i < (int) bytes_read; ++i)
                        {
                                buffer[line_pos + i] =
                                        buffer[line_pos + i + 1];
                        }
                        bytes_read--;
                }

                //Handle the ASCII escape function.
                if(keycode == ESCAPE)
                {
                        //Get the ascii action_arr.
                        char action_arr[15] = {0};

                        //Continuously read until something matches.
                        int matched = 0;
                        int read_pos = 0;
                        while(!matched)
                        {
                                //Check for more data.
                                if(read_pos < 15 && (inb(dev + LSR) & 1) != 0)
                                {
                                        char in = inb(dev);
                                        //If we get another escape, reset.
                                        if(in == ESCAPE)
                                        {
                                                read_pos = 0;
                                                memset(action_arr, 0, 15);
                                        }
                                        else
                                        {
                                                action_arr[read_pos++] = in;
                                        }
                                }

                                //Movement right
                                if(action_arr[1] == 'C' ||
                                                action_arr[0] == 'f')
                                {
                                        matched = 1;
                                        if(line_pos >= (int) bytes_read)
                                                continue;

                                        line_pos++;
                                }
                                //Movement left
                                else if(action_arr[1] == 'D'
                                                || action_arr[0] == 'b')
                                {
                                        matched = 1;
                                        if(line_pos <= 0)
                                                continue;

                                        line_pos--;
                                }
                                //Movement up
                                else if(action_arr[1] == 'A')
                                {
                                        matched = 1;
                                }
                                //Movement down
                                else if(action_arr[1] == 'B')
                                {
                                        matched = 1;
                                }
                        }
                }

                //Reset the line.
                if(beginning_pos > 0)
                        move_cursor(LEFT, beginning_pos);

                //Move it back one more.
                char clear_action[5] = {
                        ESCAPE,
                        '[',
                        '0',
                        'K',
                        '\0'
                };
                print(clear_action);

                print(buffer);

                if(bytes_read > 0)
                        move_cursor(LEFT, (int) bytes_read);

                //Get the string amount to move the cursor.
                if(line_pos > 0)
                        move_cursor(RIGHT, line_pos);
        }

        sys_req(WRITE, COM1, "\n", 1);
	return (int) bytes_read;
}
