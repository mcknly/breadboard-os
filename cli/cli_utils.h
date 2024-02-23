/******************************************************************************
 * @file cli_utils.h
 *
 * @brief Utility functions to use for CLI interaction
 *
 * @author Cavin McKinley (MCKNLY LLC)
 *
 * @date 02-14-2024
 * 
 * @copyright Copyright (c) 2024 Cavin McKinley (MCKNLY LLC)
 *            Released under the MIT License
 * 
 * SPDX-License-Identifier: MIT
 ******************************************************************************/

#ifndef CLI_UTILS_H
#define CLI_UTILS_H

#include <string.h>
#include <stdint.h>


/**
* @brief Convert a string of hex values to an integer byte array.
*
* Takes an ASCII string (prefixed with "0x") and converts each hex-digit pair
* into its 8-bit integer equivalent, then appends into an array of bytes. The
* function will check for valid hexadecimal format, returning either error or
* the length of the array (number of bytes). Note that there must be an even
* number of hex characters (full bytes).
*
* @param hex_string pointer to the hex string char array
* @param byte_array pointer to the location to store the resulting byte array
*
* @return length of the resulting byte array (number of bytes > 0) if success,
*         0 if formatting error on input string
*/
size_t hex_string_to_byte_array(char *hex_string, uint8_t *byte_array);

/**
* @brief Print the Message of the Day (MOTD)
*
* Calling this function will print the MOTD out to the CLI using an advanced
* Graphical Ascii Slow Processing (GASP!) algorithm.
*
* @param none
*
* @return nothing
*/
void print_motd(void);

#endif /* CLI_UTILS_H */