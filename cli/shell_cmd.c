/******************************************************************************
 * @file shell_cmd.c
 *
 * @brief Defines any additional CLI builtin shell commands 
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
#include <microshell.h>
#include "shell.h"

/**
* @brief 'clear' shell command callback function.
*
* Clear the screen - works when using a ANSI-compatible terminal.
*
* @param ush_file_execute_callback Params given by typedef ush_file_execute_callback. see ush_types.h
*
* @return nothing
*/
static void clear_exec_callback(struct ush_object *self, struct ush_file_descriptor const *file, int argc, char *argv[])
{
    static char clear_msg[] =   USH_SHELL_CLEAR_SCREEN
                                USH_SHELL_HOME;
    shell_print(clear_msg);
}

// cmd files descriptor
static const struct ush_file_descriptor cmd_files[] = {
    {
        .name = "clear",
        .description = "clear screen",
        .help = NULL,
        .exec = clear_exec_callback,
    },
};

// cmd commands handler
static struct ush_node_object cmd;

void shell_commands_add(void)
{
    // add custom commands
    ush_commands_add(&ush, &cmd, cmd_files, sizeof(cmd_files) / sizeof(cmd_files[0]));
}
