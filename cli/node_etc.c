/******************************************************************************
 * @file node_etc.c
 *
 * @brief /etc folder for the CLI, contains various system configurations
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
#include "motd.h"
#include "FreeRTOS.h"


/**
* @brief '/etc/motd' get data callback function.
*
* Prints out the Message of the Day (MOTD) character array that is stored in
* motd.h.
*
* @param ush_file_data_getter Params given by typedef ush_file_data_getter. see ush_types.h
*
* @return nothing, print the data directly so we can malloc/free
*/
size_t motd_get_data_callback(struct ush_object *self, struct ush_file_descriptor const *file, uint8_t **data)
{
    // copy motd string into ram
    char *motd_msg = pvPortMalloc(strlen(motd_ascii));
    strcpy(motd_msg, motd_ascii);
    // print to cli
    shell_print(motd_msg);
    vPortFree(motd_msg);

    // return null since we already printed output
    return 0;
}

// etc directory files descriptor
static const struct ush_file_descriptor etc_files[] = {
    {
        .name = "motd",                         // file name (required)
        .description = "message of the day",    // optional file description
        .help = NULL,                           // optional help manual
        .exec = NULL,                           // optional execute callback
        .get_data = motd_get_data_callback,     // optional get data (cat) callback
        .set_data = NULL                        // optional set data (echo) callback
    }
};

// etc directory handler
static struct ush_node_object etc;

void shell_etc_mount(void)
{
    // mount the /mnt directory
    ush_node_mount(&ush, "/etc", &etc, etc_files, sizeof(etc_files) / sizeof(etc_files[0]));
}