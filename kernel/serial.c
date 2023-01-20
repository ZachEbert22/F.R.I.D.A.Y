#include <mpx/io.h>
#include <mpx/serial.h>
#include <sys_req.h>
#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "ctype.h"
#include "linked_list.h"
#include "memory.h"

#define ANSI_CODE_READ_LEN 15

//Set to 1 to enable CLI history, 0 to disable.
#define DO_CLI_HISTORY 1

///If CLI history should be enabled.
static int cli_history_enabled = DO_CLI_HISTORY;

/**
 * Used to store a specific line previously entered.
 */
struct line_entry
{
    /**
     * The line that was entered. Does not include null terminator.
     */
    char *line;
    /**
     * The line's length, not including the null terminator.
     */
    size_t line_length;
};

/**
 * Useful keycodes.
 */
enum key_code
{
    BACKSPACE = 8,
    NEWLINE = 10,
    CARRIAGE_RETURN = 13,
    ESCAPE = 27,
    SPACE = 32, //The minimum keycode value for normal characters.
    TILDA = 126, //The maximum keycode value for normal characters.
    DELETE = 127,
};

/**
 * A direction registry for line navigation
 */
enum direction
{
    LEFT = 0,
    RIGHT = 1,
};

enum uart_registers
{
    RBR = 0,    // Receive Buffer
    THR = 0,    // Transmitter Holding
    DLL = 0,    // Divisor Latch LSB
    IER = 1,    // Interrupt Enable
    DLM = 1,    // Divisor Latch MSB
    IIR = 2,    // Interrupt Identification
    FCR = 2,    // FIFO Control
    LCR = 3,    // Line Control
    MCR = 4,    // Modem Control
    LSR = 5,    // Line Status
    MSR = 6,    // Modem Status
    SCR = 7,    // Scratch
};

static int initialized[4] = {0};

static int serial_devno(device dev)
{
    switch (dev)
    {
        case COM1:
            return 0;
        case COM2:
            return 1;
        case COM3:
            return 2;
        case COM4:
            return 3;
    }
    return -1;
}

int serial_init(device dev)
{
    int dno = serial_devno(dev);
    if (dno == -1)
    {
        return -1;
    }
    outb(dev + IER, 0x00);    //disable interrupts
    outb(dev + LCR, 0x80);    //set line control register
    outb(dev + DLL, 115200 / 9600);    //set bsd least sig bit
    outb(dev + DLM, 0x00);    //brd most significant bit
    outb(dev + LCR, 0x03);    //lock divisor; 8bits, no parity, one stop
    outb(dev + FCR, 0xC7);    //enable fifo, clear, 14byte threshold
    outb(dev + MCR, 0x0B);    //enable interrupts, rts/dsr set
    (void) inb(dev);        //read bit to reset port
    initialized[dno] = 1;
    return 0;
}

int serial_out(device dev, const char *buffer, size_t len)
{
    int dno = serial_devno(dev);
    if (dno == -1 || initialized[dno] == 0)
    {
        return -1;
    }
    for (size_t i = 0; i < len; i++)
    {
        outb(dev, buffer[i]);
    }
    return (int) len;
}

/**
 * @brief Moves the text cursor back the given amount of spaces.
 * @param direc 1 if we should move it direc, 0 if left.
 * @param spaces the amount to move the cursor back.
 */
void move_cursor(enum direction direc, int spaces)
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
    print(direc == RIGHT ? "C" : "D");
}

/**
 * @brief Finds the next word index or the given direction.
 * @param direc the direction to move.
 * @param cursor_index the current cursor index.
 * @param str the string to check in.
 * @param str_len the length of the string.
 * @return the index of the next word.
 */
int find_next_word(enum direction direc, int cursor_index, const char *str, int str_len)
{
    int characters_found = 0;
    int move_dir = direc == RIGHT ? 1 : -1;

    int index = cursor_index + move_dir;

    //Iterate over the string and find the next word index.
    for (; index >= 0 && index < str_len; index += move_dir)
    {
        char c = str[index];
        if (isspace(c))
        {
            if (characters_found > 0)
            {
                if (direc == LEFT && index > 0)
                    index++;
                break;
            }
        } else
        {
            characters_found++;
        }
    }

    //Coerce the number.
    index = index < 0 ? 0 : index;
    index = index > str_len ? str_len : index;

    return index;
}

void set_cli_history(int hist_enabled)
{
    cli_history_enabled = hist_enabled;
}

///The CLI history from the serial_poll function.
static linked_list *cli_history = NULL;

int serial_poll(device dev, char *buffer, size_t len)
{
    // insert your code to gather keyboard input via the technique of polling.
    // You must validate each key and handle special keys such as delete, back-space, and
    // arrow keys
    if (cli_history == NULL && cli_history_enabled)
    {
        cli_history = nl_unbounded();
    }

    //Keeps track of the current line entry. Used when command line
    //history needs to swap.
    char swap[len];
    memset(swap, 0, len);
    struct line_entry current_entry = {
            .line = swap,
            .line_length = len
    };

    int cli_index = cli_history != NULL && cli_history_enabled ? list_size(cli_history) : 0;
    size_t bytes_read = 0;
    int line_pos = 0;
    while (bytes_read < len)
    {
        //Check the LSR.
        if ((inb(dev + LSR) & 1) == 0)
            continue;

        int beginning_pos = line_pos;

        char read_char = inb(dev);

        //Get the keycode and check it against known characters.
        int keycode = (int) read_char;
        char k_str[20] = {0};
        itoa(keycode, k_str, 20);

        if (keycode >= SPACE && keycode <= TILDA)
        {
            //Copy the current characters forward.
            for (int i = (int) bytes_read; i > line_pos; --i)
            {
                buffer[i] = buffer[i - 1];
            }

            buffer[line_pos++] = read_char;
            bytes_read++;
        }

        if (keycode == CARRIAGE_RETURN || keycode == NEWLINE)
        {
            buffer[bytes_read] = '\0';
            break;
        }

        //Handle backspace and delete.
        if (keycode == BACKSPACE || keycode == DELETE)
        {
            if (line_pos == 0)
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
        if (keycode == ESCAPE)
        {
            //Get the ascii action_arr.
            char action_arr[ANSI_CODE_READ_LEN] = {0};

            //Continuously read until something matches.
            int matched = 0;
            int read_pos = 0;
            while (!matched)
            {
                //Check for more data.
                if (read_pos < ANSI_CODE_READ_LEN && (inb(dev + LSR) & 1) != 0)
                {
                    char in = inb(dev);
                    //If we get another escape, reset.
                    if (in == ESCAPE)
                    {
                        read_pos = 0;
                        memset(action_arr, 0, ANSI_CODE_READ_LEN);
                    } else
                    {
                        action_arr[read_pos++] = in;
                    }
                }

                //Movement right or left
                if (action_arr[1] == 'C' || action_arr[1] == 'D')
                {
                    matched = 1;

                    //Adjust value then coerce.
                    line_pos += action_arr[1] == 'C' ? 1 : -1;
                    line_pos = line_pos < 0 ? 0 : line_pos;
                    line_pos = line_pos > (int) bytes_read ? (int) bytes_read : line_pos;
                }
                //Word movement right or left
                else if (action_arr[0] == 'f' || action_arr[0] == 'b')
                {
                    matched = 1;
                    int next_index =
                            find_next_word(action_arr[0] == 'b' ? LEFT : RIGHT,
                                           line_pos,
                                           buffer,
                                           (int) bytes_read);

                    line_pos = next_index;
                }
                //Movement up and down
                else if (action_arr[1] == 'A' || action_arr[1] == 'B')
                {
                    matched = 1;

                    if(!cli_history_enabled)
                        continue;

                    int l_size = list_size(cli_history);
                    //Check if we can move in the history.
                    if (cli_history == NULL || (cli_index <= 0 && action_arr[1] == 'A')
                        || (cli_index >= l_size && action_arr[1] == 'B'))
                        continue;

                    //Get previous or future line.
                    int delta = action_arr[1] == 'A' ? -1 : 1;
                    cli_index += delta;
                    struct line_entry *l_entry = cli_index >= l_size ?
                                                 &current_entry :
                                                 get_item(cli_history, cli_index);
                    size_t copy_len = l_entry->line_length > len
                                      ? len :
                                      l_entry->line_length;

                    //Save current, load old line.
                    if (cli_index == l_size - 1 && delta == -1)
                    {
                        memcpy(current_entry.line, buffer, len);
                        current_entry.line_length = bytes_read;
                    }

                    //Zero out buffer, then copy in new string.
                    memset(buffer, 0, len);
                    memcpy(buffer, l_entry->line, copy_len);
                    buffer[copy_len] = '\0';
                    line_pos = (int) copy_len;
                    bytes_read = copy_len;
                }
                //Word deletion
                else if (action_arr[0] == DELETE)
                {
                    matched = 1;

                    int delete_index = find_next_word(LEFT,
                                                      line_pos,
                                                      buffer,
                                                      (int) bytes_read);
                    int deleted = line_pos - delete_index;

                    //Copy the string down.
                    for (int i = delete_index; i < (int) line_pos; ++i)
                    {
                        buffer[i] = buffer[i + deleted];
                        buffer[i + deleted] = '\0';
                    }

                    line_pos -= deleted;
                    bytes_read -= deleted;
                }
            }
        }

        //Reset the line.
        if (beginning_pos > 0)
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

        if (bytes_read > 0)
            move_cursor(LEFT, (int) bytes_read);

        //Get the string amount to move the cursor.
        if (line_pos > 0)
            move_cursor(RIGHT, line_pos);
    }

    //Allocate the line for storage.
    if (cli_history != NULL && cli_history_enabled)
    {
        //Allocate memory and store string.
        struct line_entry *to_store = sys_alloc_mem(sizeof(struct line_entry));
        char *store_line = sys_alloc_mem(bytes_read);
        memcpy(store_line, buffer, bytes_read);

        to_store->line = store_line;
        to_store->line_length = bytes_read;
        add_item_index(cli_history, list_size(cli_history), to_store);
    }

    sys_req(WRITE, COM1, "\n", 1);
    return (int) bytes_read;
}
