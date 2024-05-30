/******************************************************************************
 * @file shell.h
 *
 * @brief Microshell custom settings and wrapper functions to use at the CLI
 *        application layer
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

#ifndef SHELL_H
#define SHELL_H

#include <stdbool.h>
#include <microshell.h>


// microshell in/out buffer allocations
#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_AVR_MEGA2560) || defined(ARDUINO_ARCH_STM32)
    #define BUF_IN_SIZE    128
    #define BUF_OUT_SIZE   128
    #define PATH_MAX_SIZE  128
    #define SHELL_HISTORY_LINES 6
    #define SHELL_WORK_BUFFER_SIZE 256
#elif defined(PICO)
    #define BUF_IN_SIZE    128
    #define BUF_OUT_SIZE   1024
    #define PATH_MAX_SIZE  128
    #define SHELL_HISTORY_LINES 6
    #define SHELL_WORK_BUFFER_SIZE 256
#else
    #define BUF_IN_SIZE    32
    #define BUF_OUT_SIZE   32
    #define PATH_MAX_SIZE  32
    #define SHELL_HISTORY_LINES 6
    #define SHELL_WORK_BUFFER_SIZE 256
#endif

// custom prompt formatting -
// microshell prompt format is prompt_prefix+HOST_NAME+prompt_space+[path]+prompt_suffix
// see "ush_shell.h" for font formatting codes
#define HOST_NAME pico
#define SHELL_PROMPT_PREFIX USH_SHELL_FONT_COLOR_MAGENTA \
                            USH_SHELL_FONT_STYLE_BOLD \
                            "[" \
                            USH_SHELL_FONT_COLOR_YELLOW
#define SHELL_PROMPT_SPACE  USH_SHELL_FONT_COLOR_CYAN \
                            " "
#define SHELL_PROMPT_SUFFIX USH_SHELL_FONT_COLOR_MAGENTA \
                            USH_SHELL_FONT_STYLE_BOLD \
                            "]$ " \
                            USH_SHELL_FONT_STYLE_RESET

// miscellaneous definitions
#define NOCHAR 255 // return value of cli_uart_getc() in ush_read() if timeout occurs
#define TIMESTAMP_LEN 20 // length of timestamp() string to use when sizing print buffers
#define SLOW_PRINT_CHAR_DELAY_MS 1   // shell_print_slow() OS tick delay between chars
#define SLOW_PRINT_LINE_DELAY_MS 5  // shell_print_slow() OS tick delay between lines

// global microshell instance handler
extern struct ush_object ush;

/**
* @brief Initialize the CLI shell.
*
* Initializes the microshell instance and mounts all directory and file nodes.
*
* @param none
*
* @return nothing
*/
void shell_init(void);

/**
* @brief Service routine for the CLI shell.
*
* Runs in a loop to service all microshell functions.
*
* @param none
*
* @return nothing
*/
void shell_service(void);

/**
* @brief Print string output in the shell.
*
* Wrapper which allows other functions within the CLI to print output to the shell.
* Anything outside the CLI task should use the print queue (cli_print_raw() or cli_print_timestamped()).
* Note that this function blocks until printing is complete.
*
* @param buf pointer to the string to print
*
* @return nothing
*/
void shell_print(char *buf);

/**
* @brief Print string output in the shell, slowly.
*
* Similar to shell_print(), with the addition of RTOS task delays to give the output
* text that groovy retro console feel. This is a silly function with no real world
* value. Note that this is only intended for large multi-line prints like MOTD,
* which are stored in flash (const char*)
*
* @param buf pointer to the string to print
*
* @return nothing
*/
void shell_print_slow(const char *buf);

/**
* @brief Check if the shell is currently printing.
*
* Wrap this function in while() to wait until the shell is done printing all
* characters of a string before moving on. This is implemented inside shell_print().
*
* @param none
*
* @return true if shell is still printing the current string, false when complete
*/
bool shell_is_printing(void);

/**
* @brief Generate a timestamp that can be used in shell prints.
*
* Provides a pointer to a string in '[ xxx ]' format that is useful for
* displaying clear timestamps along with shell output.
*
* @param none
*
* @return pointer to the timestamp string
*/
char * timestamp(void);

/**
* @brief Shell serial read interface.
*
* Hardware-abstracted, non-blocking read interface for microshell to receive
* input characters.
*
* @param ush_io_interface_read_char Params given by typedef ush_io_interface_read_char. see ush_types.h
*
* @return See ush_io_interface_read_char in ush_types.h
*/
static int ush_read(struct ush_object *self, char *ch);

/**
* @brief Shell serial write interface.
*
* Hardware-abstracted, non-blocking write interface for microshell to send
* output characters.
*
* @param ush_io_interface_write_char Params given by typedef ush_io_interface_write_char. see ush_types.h
*
* @return See ush_io_interface_read_write in ush_types.h
*/
static int ush_write(struct ush_object *self, char ch);

#endif /* SHELL_H */
