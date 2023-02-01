//
// Created by Andrew Bowie on 2/1/23.
//

#include "print_format.h"
#include "stdio.h"

/**
 * Prints an ANSII format code to the standard output.
 *
 * @param code the code to print.
 */
void print_format_code(int code)
{
    printf("%c[%dm", 27, code);
}

///The bold format code.
static const int BOLD_CODE = 1;
///The reset code for bold.
static const int BOLD_RESET_CODE = 22;
///The bold format code.
static const int ITALIC_CODE = 3;
///The reset code for bold.
static const int ITALIC_RESET_CODE = 23;
///The bold format code.
static const int UNDERLINE_CODE = 4;
///The reset code for bold.
static const int UNDERLINE_RESET_CODE = 24;
///The bold format code.
static const int STRIKETHROUGH_CODE = 9;
///The reset code for bold.
static const int STRIKETHROUGH_RESET_CODE = 29;

///If the output is currently bolded.
static bool bolded = false;
///If the output is currently italicized.
static bool italicized = false;
///If the output is currently underlined.
static bool underlined = false;
///If the output is currently struck through
static bool struck_through = false;

bool is_bold()
{
    return bolded;
}

void set_bold(bool bold)
{
    bolded = bold;
    if(bold)
        print_format_code(BOLD_CODE);
    else
        print_format_code(BOLD_RESET_CODE);
}

bool is_italic()
{
    return italicized;
}

void set_italic(bool italic)
{
    italicized = italic;
    if(italic)
        print_format_code(ITALIC_CODE);
    else
        print_format_code(ITALIC_RESET_CODE);
}

bool is_underline()
{
    return underlined;
}

void set_underline(bool underline)
{
    underlined = underline;
    if(underline)
        print_format_code(UNDERLINE_CODE);
    else
        print_format_code(UNDERLINE_RESET_CODE);
}

bool is_strikethrough()
{
    return struck_through;
}

void set_strikethrough(bool strikethrough)
{
    struck_through = strikethrough;
    if(strikethrough)
        print_format_code(STRIKETHROUGH_CODE);
    else
        print_format_code(STRIKETHROUGH_RESET_CODE);
}

void clear_formats()
{
    set_bold(false);
    set_italic(false);
    set_underline(false);
    set_strikethrough(false);
}