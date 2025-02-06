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
    netman_action_t nma;

    if (argc == 4) {
        if (strcmp(argv[1], "setauth") == 0) { // set the wifi network authentication parameters
            // create wifi_auth file with new credentials (or overwrite existing)
            struct storman_item_t smi;
            smi.action = WRITEFILE;
            strcpy(smi.sm_item_name, "wifi_auth");
            sprintf(smi.sm_item_data, "%s,%s", argv[2], argv[3]);
            storman_request(&smi);
            shell_print("wifi network credentials set");
        }
        else {
            shell_print("command syntax error, see 'help <wifi>'");
        }
    }
    else if (argc == 2) {
        if (strcmp(argv[1], "connect") == 0) { // connect to the wifi network defined by 'setauth'
            nma = NETJOIN;
            netman_request(nma);
        }
        else if (strcmp(argv[1], "disconnect") == 0) { // disconnect from the wifi network
            nma = NETLEAVE;
            netman_request(nma);
        }
        else {
            shell_print("command syntax error, see 'help <wifi>'");
        }
    }
    else {
        shell_print("command syntax error, see 'help <wifi>'");
    }
}

// net directory files descriptor
static const struct ush_file_descriptor net_files[] = {
#if HW_USE_WIFI
    {
        .name = "wifi",                                        // file name (required)
        .description = "WiFi network interface",               // optional file description
        .help = "usage: wifi <connect|disconnect>\r\n"         // optional help manual
                "       wifi setauth <\e[3mssid\e[0m>"         // optional help manual
                                   " <\e[3mpassword\e[0m>\r\n",
        .exec = wifi_exec_callback,                            // optional execute callback
        .get_data = NULL,                                      // optional get data (cat) callback
        .set_data = NULL                                       // optional set data (echo) callback
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