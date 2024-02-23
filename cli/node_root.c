/******************************************************************************
 * @file node_root.c
 *
 * @brief root folder ('/') for the CLI containing directories and files
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
#include "hardware_config.h"

/**
* @brief 'info.txt' get data callback function.
*
* Returns the "contents" of the info.txt file by using the built-in 'cat' CLI command.
*
* @param ush_file_data_getter Params given by typedef ush_file_data_getter. see ush_types.h
*
* @return length of pointer to the text strings to print
*/
size_t info_get_data_callback(struct ush_object *self, struct ush_file_descriptor const *file, uint8_t **data)
{
    static char *info = xstr(PROJECT_NAME) ", compiled with love\r\n"; // the contents of the file

    // set pointer to data
    *data = (uint8_t*)info;
    // return data size
    return strlen(info);
}

// root directory files descriptor
static const struct ush_file_descriptor root_files[] = {
    {
        .name = "info.txt",                     // file name (required)
        .description = NULL,                    // optional file description
        .help = NULL,                           // optional help manual
        .exec = NULL,                           // optional execute callback
        .get_data = info_get_data_callback,     // optional get data (cat) callback
        .set_data = NULL                        // optional set data (echo) callback
    }
};

// root directory handler
static struct ush_node_object root;

void shell_root_mount(void)
{
    // mount root directory
    ush_node_mount(&ush, "/", &root, root_files, sizeof(root_files) / sizeof(root_files[0]));
}
