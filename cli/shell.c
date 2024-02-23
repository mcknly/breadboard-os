/******************************************************************************
 * @file shell.h
 *
 * @brief Microshell instance definition, settings, & wrapper functions to use
 *        at CLI application level
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

#include <stdio.h>
#include <string.h>
#include <microshell.h>
#include "hardware_config.h"
#include "shell.h"
#include "services.h"
#include "service_queues.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "rtos_utils.h"


// microshell character read interface
static int ush_read(struct ush_object *self, char *ch)
{
    char inchar = NOCHAR;
    if (CLI_USE_USB) {
        inchar = cli_usb_getc();  // CLI over USB get single char
    }
    else {
        inchar = cli_uart_getc(); // CLI over UART get single char
    }
    if(inchar != NOCHAR) {
        *ch = inchar;
        return 1;
    }
    return 0;
}

// microshell character write interface
static int ush_write(struct ush_object *self, char ch)
{
    if (CLI_USE_USB) {
        return cli_usb_putc(ch);  // CLI over USB put single char
    }
    else {
        return cli_uart_putc(ch); // CLI over UART put single char
    }
}

// I/O interface descriptor
static const struct ush_io_interface ush_iface = {
    .read = ush_read,
    .write = ush_write,
};

static char ush_in_buf[BUF_IN_SIZE];
static char ush_out_buf[BUF_OUT_SIZE];

// global microshell instance handler (extern declared in shell.h)
struct ush_object ush;

// microshell custom prompt formatting structure - defined in shell.h
static const struct ush_prompt_format ush_prompt = {
    .prompt_prefix = SHELL_PROMPT_PREFIX,
    .prompt_space = SHELL_PROMPT_SPACE,
    .prompt_suffix = SHELL_PROMPT_SUFFIX
};

// microshell descriptor
static const struct ush_descriptor ush_desc = {
    .io = &ush_iface,                           // I/O interface pointer
    .input_buffer = ush_in_buf,                 // working input buffer
    .input_buffer_size = sizeof(ush_in_buf),    // working input buffer size
    .output_buffer = ush_out_buf,               // working output buffer
    .output_buffer_size = sizeof(ush_out_buf),  // working output buffer size
    .path_max_length = PATH_MAX_SIZE,           // path maximum length (stack)
    .hostname = xstr(HOST_NAME),                // hostname (in prompt) - defined in hardware_config.h
    .prompt_format = &ush_prompt                // custom prompt formatting
};

// declare the extern functions to build the global directory structure - defined in individual source files
extern void shell_commands_add(void);
extern void shell_root_mount(void);
extern void shell_dev_mount(void);
extern void shell_bin_mount(void);
extern void shell_proc_mount(void);
extern void shell_etc_mount(void);
extern void shell_lib_mount(void);

void shell_init(void)
{
    // initialize microshell instance
    ush_init(&ush, &ush_desc);

    // add commands
    shell_commands_add();

    // mount nodes (root must be first) - order printed with 'ls' follows reverse
    // of this mount order - if nodes are mounted on the fly later, they will
    // be on the top
    shell_root_mount();
    shell_lib_mount();
    shell_dev_mount();
    shell_proc_mount();
    shell_bin_mount();
    shell_etc_mount();
    // note that /mnt is currently mounted by storagemanager service
}

void shell_service(void)
{
    ush_service(&ush);
}

void shell_print(char *buf)
{
    // print buffer using microshell interface
    ush_print(&ush, buf);
    // wait until printing is finished to exit function (blocking)
    while(shell_is_printing()) {}
}

void shell_print_slow(const char *buf) {
    uint16_t string_index = 0;
    char char_buf[2];
    bool end_of_string = false;

    // move through the print buffer one character at a time
    while (!end_of_string) {
        char_buf[0] = buf[string_index]; // copy current char into print buffer
        char_buf[1] = '\0'; // terminate the single char as a string
        if (buf[string_index] == '\n') { // upon newline block printing for specified delay time
            string_index++;
            task_delay_ms(SLOW_PRINT_LINE_DELAY_MS);
        }
        else if (buf[string_index] == '\0') { // end of print buffer
            end_of_string = true;
        }
        else {
            string_index++;
            task_delay_ms(SLOW_PRINT_CHAR_DELAY_MS);
        }
        if (!end_of_string) {
            ush_print(&ush, char_buf);
            while(shell_is_printing()) {}
        }
    }
}

bool shell_is_printing(void)
{
    // check if microshell is currently in writing state
    if (ush.state == USH_STATE_WRITE_CHAR) {
        ush_service(&ush); // keep servicing the shell if it is still writing chars
        return true;
    } else {
        return false;
    }
}

char * timestamp(void)
{
    static char timestamp_msg[TIMESTAMP_LEN]; // 14 bytes plus spaces and brackets, ~3 years before rollover
    snprintf(timestamp_msg, TIMESTAMP_LEN, "[%llu\t] ", get_time_us());
    return timestamp_msg;
}