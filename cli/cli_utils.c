/******************************************************************************
 * @file cli_utils.c
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

#include <string.h>
#include <stdint.h>
#include "shell.h"
#include "cli_utils.h"
#include "FreeRTOS.h"


size_t hex_string_to_byte_array(char *hex_string, uint8_t *byte_array) {
    if (hex_string[0] != '0' || hex_string[1] != 'x') {
        return 0; // error if no "0x" prefix
    }
    int len = strlen(hex_string + 2); // hex string length minus the prefix
    if (len % 2 != 0) {
        return 0; // error if not an even number of full bytes
    }
    int string_index, byte_index;
    for (string_index = 0, byte_index = 0; byte_index < len / 2; string_index += 2, byte_index++) {
        // craaaaaazy long if to check for characters in the range 0-9, a-f, or A-F
        if (((((hex_string[string_index + 2] - 48) <= 9) && ((hex_string[string_index + 2] - 48) >= 0))  || // first character 0-9?...or
             (((hex_string[string_index + 2] - 65) <= 5) && ((hex_string[string_index + 2] - 65) >= 0))  || // first character A-F?...or
             (((hex_string[string_index + 2] - 97) <= 5) && ((hex_string[string_index + 2] - 97) >= 0)))    // first character a-f?
            &&                                                                                              // plus
            ((((hex_string[string_index + 3] - 48) <= 9) && ((hex_string[string_index + 3] - 48) >= 0))  || // second character 0-9?...or
             (((hex_string[string_index + 3] - 65) <= 5) && ((hex_string[string_index + 3] - 65) >= 0))  || // second character A-F?...or
             (((hex_string[string_index + 3] - 97) <= 5) && ((hex_string[string_index + 3] - 97) >= 0))))   // second character a-f?
        // whitespace for the win!
        {
            // prepare yourself for this wizardry... convert 2-char ASCII hex to decimal integer
            // this works due to some clever spacing between 0, A, and a in the ASCII table.
            // thanks to: https://gist.github.com/xsleonard/7341172 for the tip
            byte_array[byte_index] = (hex_string[string_index + 2] % 32 + 9) % 25 * 16 + (hex_string[string_index + 3] % 32 + 9) % 25;
        }
        else {return 0;} // error if not a hexadecimal value
    }
    return byte_index;
}

void print_motd(void) {
    // global Message of the Day stored in motd.h. Declared here and defined in the header (violates best practice, sorry y'all)
    extern const char *motd_ascii;
    
    // print directly from MOTD message stored in flash
    shell_print_slow(motd_ascii);
}