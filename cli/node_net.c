/******************************************************************************
 * @file node_net.c
 *
 * @brief /net folder for the CLI, contains network tools and network hardware 
 *        interaction functions
 *
 * @author Cavin McKinley (MCKNLY LLC)
 *
 * @date 01-24-2025
 *
 * @copyright Copyright (c) 2025 Cavin McKinley (MCKNLY LLC)
 *            Released under the MIT License
 *
 * SPDX-License-Identifier: MIT
 ******************************************************************************/

#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <microshell.h>
#include "hardware_config.h"
#include "shell.h"
#include "cli_utils.h"
#include "services.h"
#include "service_queues.h"
#include "lfs.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"


static void wifi_exec_callback(struct ush_object *self, struct ush_file_descriptor const *file, int argc, char *argv[])
{

}

// net directory files descriptor
static const struct ush_file_descriptor net_files[] = {
#if HW_USE_WIFI
    {
        .name = "wifi",                          // file name (required)
        .description = "WiFi network interface", // optional file description
        .help = NULL,                            // optional help manual
        .exec = wifi_exec_callback,              // optional execute callback
        .get_data = NULL,                        // optional get data (cat) callback
        .set_data = NULL                         // optional set data (echo) callback
    },
#endif /* HW_USE_WIFI */
};

// net directory handler
static struct ush_node_object net;

void shell_net_mount(void)
{
    // mount net directory
    ush_node_mount(&ush, "/net", &net, net_files, sizeof(net_files) / sizeof(net_files[0]));
}