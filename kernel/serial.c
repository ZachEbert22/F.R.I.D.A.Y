#include <mpx/io.h>
#include <mpx/serial.h>
#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "ctype.h"
#include "linked_list.h"
#include "memory.h"
#include "commands.h"
#include "color.h"

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

#include "cli.h"
#include "commands.h"

#define ANSI_CODE_READ_LEN 15
#define MAX_CLI_HISTORY_LEN (100)

///Used to store a specific line previously entered.
struct line_entry
{
    ///These are a hacky way to use linked lists without excessive allocation (temp until R5)
    void *_dont_use_1;
    ///These are a hacky way to use linked lists without excessive allocation (temp until R5)
    void *_dont_use_2;

    /**
     * The line that was entered. Does not include null terminator.
     */
    char *line;
    /**
     * The line's length, not including the null terminator.
     */
    size_t line_length;
};

///Contains constants for useful or common keycodes.
enum key_code
{
    BACKSPACE = 8,
    TAB = 9,
    NEWLINE = 10,
    CARRIAGE_RETURN = 13,
    ESCAPE = 27,
    SPACE = 32, //The minimum keycode value for normal characters.
    TILDA = 126, //The maximum keycode value for normal characters.
    DELETE = 127,
};


///A direction registry for line navigation.
enum direction
{
    LEFT = 0,
    RIGHT = 1,
};

///If CLI history should be enabled.
static bool cli_history_enabled = false;
///If CLI command color formatting should be enabled.
static bool command_formatting_enabled = false;
///If the CLI input should be invisible.
static bool cli_invisible = false;
///If the CLI should implement tab completions.
static bool tab_completions = false;
///The prompt to print when requesting input.
static const char *prompt = NULL;

void set_cli_prompt(const char *str)
{
    if(str != NULL)
    {
        size_t len = strlen(str);
        //Don't allow prompts longer than 40 chars.
        if(len > 40)
            return;
    }

    prompt = str;
}

void set_cli_history(bool hist_enabled)
{
    cli_history_enabled = hist_enabled;
}

void set_command_formatting(bool enabled)
{
    command_formatting_enabled = enabled;
}

void set_invisible(bool enabled)
{
    cli_invisible = enabled;
}

void set_tab_completions(bool enabled)
{
    tab_completions = enabled;
}

/**
 * @brief Moves the text cursor back the given amount of spaces.
 * @param dev the device to print to.
 * @param direc 1 if we should move it direc, 0 if left.
 * @param spaces the amount to move the cursor back.
 */
void move_cursor(device dev, enum direction direc, int spaces)
{
    char full_len_str[20] = {0};
    itoa((int) spaces, full_len_str, 19);
    size_t str_len = strlen(full_len_str);

    char m_left_prefix[3] = {
            ESCAPE,
            '[',
            '\0'
    };
    serial_out(dev, m_left_prefix, 2);
    serial_out(dev, full_len_str, str_len);
    serial_out(dev, direc == RIGHT ? "C" : "D", 1);
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

/**
 * @brief Sets the output color using serial_out instead of printf. (Avoids sys_req call)
 * @param color the color to set.
 */
void internal_soc(const color_t *color)
{
    static const char format_arr[2] = {27, '['};
    char color_arr[3] = {0};
    itoa(color->color_num, color_arr, 3);

    serial_out(COM1, format_arr, 2);
    serial_out(COM1, color_arr, strlen(color_arr));
    serial_out(COM1, "m", 1);
}

///The CLI history from the serial_poll function.
static linked_list *cli_history = NULL;

int serial_poll(device dev, char *buffer, size_t len)
{
    if (cli_history == NULL && cli_history_enabled)
    {
        cli_history = nl_unbounded();
    }

    //Check if the CLI should be forcefully disabled.
    if(cli_history_enabled && list_size(cli_history) > MAX_CLI_HISTORY_LEN)
    {
        cli_history_enabled = false;
    }

    //Keeps track of the current line entry. Used when command line
    //history needs to swap.
    char swap[len];
    memset(swap, 0, len);
    struct line_entry current_entry = {
            .line = swap,
            .line_length = len
    };

    int cli_index = cli_history != NULL && cli_history_enabled
            ? list_size(cli_history) : 0;
    size_t bytes_read = 0;
    int line_pos = 0;

    if(prompt != NULL)
    {
        serial_out(COM1, prompt, strlen(prompt));
    }

    while (bytes_read < len)
    {
        //Check the LSR.
        //A null buffer indicates the user simply wants to poll.
        if ((inb(dev + LSR) & 1) == 0)
        {
            if(buffer == NULL)
                return 0;
            continue;
        }

        int beginning_pos = line_pos;

        char read_char = inb(dev);

        //Get the keycode and check it against known characters.
        int keycode = (int) read_char;
        char k_str[20] = {0};
        itoa(keycode, k_str, 20);

        //Check if the buffer is null.
        if(buffer == NULL)
        {
            if(keycode >= SPACE && keycode <= TILDA)
            {
                return keycode;
            }
            else
            {
                return 0;
            }
        }

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
                //We check every loop as ALL data from an ANSI escape isn't immediately available.
                //For example, a read might look like: <esc>000000[000A
                //All data isn't immediately available, so we have to continuously look for more data.
                if(read_pos >= ANSI_CODE_READ_LEN)
                    break;

                if ((inb(dev + LSR) & 1) != 0)
                {
                    char in = inb(dev);

                    //Throw away the bracket.
                    if(in == '[')
                        continue;

                    //If we get another escape, reset.
                    if (in == ESCAPE)
                    {
                        read_pos = 0;
                        memset(action_arr, 0, ANSI_CODE_READ_LEN);
                    }
                    else
                    {
                        action_arr[read_pos++] = in;
                    }
                }

                //Movement right or left
                if (action_arr[0] == 'C' || action_arr[0] == 'D')
                {
                    matched = 1;

                    //Adjust value then coerce.
                    line_pos += action_arr[0] == 'C' ? 1 : -1;
                    line_pos = line_pos < 0 ? 0 : line_pos;
                    line_pos = line_pos > (int) bytes_read ? (int) bytes_read : line_pos;
                }
                //Word movement right or left. (Different codes for Unix/Windows)
                else if ((action_arr[0] == 'f' || action_arr[3] == 'C') ||
                        (action_arr[0] == 'b' || action_arr[3] == 'D'))
                {
                    matched = 1;
                    int next_index =
                            find_next_word(action_arr[0] == 'b' || action_arr[3] == 'D' ? LEFT : RIGHT,
                                           line_pos,
                                           buffer,
                                           (int) bytes_read);

                    line_pos = next_index;
                }
                //Movement up and down
                else if (action_arr[0] == 'A' || action_arr[0] == 'B')
                {
                    matched = 1;

                    if(!cli_history_enabled)
                        continue;

                    int l_size = list_size(cli_history);
                    //Check if we can move in the history.
                    if (cli_history == NULL || (cli_index <= 0 && action_arr[0] == 'A')
                        || (cli_index >= l_size && action_arr[0] == 'B'))
                        continue;

                    //Get previous or future line.
                    int delta = action_arr[0] == 'A' ? -1 : 1;
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
                //The 'delete' key
                else if (action_arr[0] == '3' && action_arr[1] == '~')
                {
                    matched = 1;

                    if(line_pos >= (int) bytes_read)
                        continue;

                    //Delete the character.
                    buffer[line_pos] = '\0';

                    //Copy down the new characters.
                    for (int i = 0; i < (int) bytes_read; ++i)
                    {
                        buffer[line_pos + i] =
                                buffer[line_pos + i + 1];
                    }
                    bytes_read--;
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
                //This acts as a 'catch-all' for all input-able ansi escape codes.
                else if((action_arr[0] >= 'A' && action_arr[0] <= 'Z' && action_arr[0] != 'O') ||
                        action_arr[read_pos - 1] == '~' ||
                        (action_arr[0] == 'O' && action_arr[1] != 0))
                {
                    matched = 1;
                }
            }
        }

        //Check if it was a TAB completion.
        if(keycode == TAB && tab_completions)
        {
            //Find the best match.
            const char *best = find_best_match(buffer);
            if(best != NULL)
            {
                size_t best_len = strlen(best);
                bytes_read = best_len;
                line_pos = (int) bytes_read;

                //Empty the buffer.
                memset(buffer, 0, len);
                strcpy(buffer, best, -1);
            }
        }

        if(cli_invisible)
            continue;

        //Reset the line.
        if (beginning_pos > 0)
            move_cursor(dev, LEFT, beginning_pos);

        //Move it back one more.
        char clear_action[5] = {
                ESCAPE,
                '[',
                '0',
                'K',
                '\0'
        };

        serial_out(dev, clear_action, 4);

        //Get the current color.
        const color_t *clr = get_output_color();
        bool cmd_exists = false;
        if(command_formatting_enabled)
        {
            cmd_exists = command_exists(buffer);
            if(cmd_exists)
            {
                internal_soc(get_color("bright-green"));
            }
            else
            {
                internal_soc(get_color("red"));
            }
        }

        serial_out(dev, buffer, bytes_read);

        if (bytes_read > 0)
            move_cursor(dev, LEFT, (int) bytes_read);

        //Get the string amount to move the cursor.
        if (line_pos > 0)
            move_cursor(dev, RIGHT, line_pos);

        if(command_formatting_enabled)
        {
            internal_soc(clr);
        }
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
    serial_out(dev, "\n", 1);
    return (int) bytes_read;
}
