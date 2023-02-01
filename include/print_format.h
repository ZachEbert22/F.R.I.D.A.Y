//
// Created by Andrew Bowie on 2/1/23.
//

#ifndef F_R_I_D_A_Y_PRINT_FORMAT_H
#define F_R_I_D_A_Y_PRINT_FORMAT_H

#include "color.h"
#include "stdbool.h"

/**
 * Gets if the output is currently bolded according to this file.
 *
 * @return if the output is bold.
 */
bool is_bold();

/**
 * Sets output to bold depending on the input boolean.
 *
 * @param bold if the output should be bolded.
 */
void set_bold(bool bold);

/**
 * Gets if the output is currently underlined according to this file.
 *
 * @return if the output is underlined.
 */
bool is_underline();

/**
 * Sets output to underline depending on the input boolean.
 *
 * @param underline if the output should be underlined.
 */
void set_underline(bool underline);

/**
 * Gets if the output is currently italicized according to this file.
 *
 * @return if the output is italic.
 */
bool is_italic();

/**
 * Sets output to italic depending on the input boolean.
 *
 * @param italic if the output should be italic.
 */
void set_italic(bool italic);

/**
 * Gets if the output is currently strikethrough according to this file.
 *
 * @return if the output is strikethrough.
 */
bool is_strikethrough();

/**
 * Sets output to strikethrough depending on the input boolean.
 * @param strikethrough
 */
void set_strikethrough(bool strikethrough);

/**
 * Clears all formatting from the output. Does NOT clear color!
 */
void clear_formats();

#endif //F_R_I_D_A_Y_PRINT_FORMAT_H
