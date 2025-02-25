/******************************************************************************
 * @file node_mnt.c
 *
 * @brief /mnt folder for the CLI, contains littlefs filesystem(s)
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

#include <stdbool.h>
#include <string.h>
#include <microshell.h>
#include "shell.h"
#include "services.h"
#include "service_queues.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"


/**
* @brief '/mnt/flash0' executable callback function.
*
* Interact with the onboard flash0 filesystem (list directories, read/write
* files, etc.), by putting the action request into the storagemanager's queue
* for servicing, and then reading out any resulting data from the global
* storagemanager data structure.
*
* @param ush_file_execute_callback Params given by typedef ush_file_execute_callback. see ush_types.h
*
* @return nothing
*/
static void flash0_exec_callback(struct ush_object *self, struct ush_file_descriptor const *file, int argc, char *argv[])
{
    bool syntax_err = false;
    struct storman_item_t smi;

    // make sure storagemanager is actually running
    if (xTaskGetHandle(xstr(SERVICE_NAME_STORMAN)) == NULL) {
        shell_print("error, " xstr(SERVICE_NAME_STORMAN) " service is not running");
    }
    else if (argc > 1) {
        if (strcmp(argv[1], "lsdir") == 0 && argc == 3) {
            smi.action = LSDIR;
            strcpy(smi.sm_item_name, argv[2]);
            storman_request(&smi);
            // wait for storagemanager to provide semaphore indicating data is ready
            if (xSemaphoreTake(smi_glob_sem, DELAY_STORMAN * 2) == pdTRUE) {
                shell_print(smi_glob.sm_item_data);
            }
        }
        else if (strcmp(argv[1], "mkdir") == 0 && argc == 3) {
            smi.action = MKDIR;
            strcpy(smi.sm_item_name, argv[2]);
            storman_request(&smi);
        }
        else if (strcmp(argv[1], "rmdir") == 0 && argc == 3) {
            smi.action = RMDIR;
            strcpy(smi.sm_item_name, argv[2]);
            storman_request(&smi);
        }
        else if (strcmp(argv[1], "mkfile") == 0 && argc == 3) {
            smi.action = MKFILE;
            strcpy(smi.sm_item_name, argv[2]);
            storman_request(&smi);
        }
        else if (strcmp(argv[1], "rmfile") == 0 && argc == 3) {
            smi.action = RMFILE;
            strcpy(smi.sm_item_name, argv[2]);
            storman_request(&smi);
        }
        else if (strcmp(argv[1], "dumpfile") == 0 && argc == 3) {
            smi.action = DUMPFILE;
            strcpy(smi.sm_item_name, argv[2]);
            storman_request(&smi);
            // wait for storagemanager to provide semaphore indicating data is ready
            if (xSemaphoreTake(smi_glob_sem, DELAY_STORMAN * 2) == pdTRUE) {
                shell_print(smi_glob.sm_item_data);
            }
        }
        else if (strcmp(argv[1], "readfile") == 0 && argc == 5) {
            smi.action = READFILE;
            strcpy(smi.sm_item_name, argv[2]);
            smi.sm_item_offset = (lfs_soff_t)strtol(argv[3], NULL, 10);
            smi.sm_item_size = strtol(argv[4], NULL, 10);
            storman_request(&smi);
            // wait for storagemanager to provide semaphore indicating data is ready
            if (xSemaphoreTake(smi_glob_sem, DELAY_STORMAN * 2) == pdTRUE) {
                shell_print(smi_glob.sm_item_data);
            }
        }
        else if (strcmp(argv[1], "writefile") == 0 && argc == 4) {
            smi.action = WRITEFILE;
            strcpy(smi.sm_item_name, argv[2]);
            strcpy(smi.sm_item_data, argv[3]);
            storman_request(&smi);
        }
        else if (strcmp(argv[1], "appendfile") == 0 && argc == 4) {
            smi.action = APPENDFILE;
            strcpy(smi.sm_item_name, argv[2]);
            strcpy(smi.sm_item_data, argv[3]);
            storman_request(&smi);
        }
        else if (strcmp(argv[1], "filestat") == 0 && argc == 3) {
            smi.action = FILESTAT;
            strcpy(smi.sm_item_name, argv[2]);
            storman_request(&smi);
            // wait for storagemanager to provide semaphore indicating data is ready
            if (xSemaphoreTake(smi_glob_sem, DELAY_STORMAN * 2) == pdTRUE) {
                shell_print(smi_glob.sm_item_data);
            }
        }
        else if (strcmp(argv[1], "fsstat") == 0 && argc == 2) {
            smi.action = FSSTAT;
            storman_request(&smi);
            // wait for storagemanager to provide semaphore indicating data is ready
            if (xSemaphoreTake(smi_glob_sem, DELAY_STORMAN * 2) == pdTRUE) {
                shell_print(smi_glob.sm_item_data);
            }
        }
        else if (strcmp(argv[1], "format") == 0 && argc == 2) {
            smi.action = FORMAT;
            storman_request(&smi);
            // wait for storagemanager to provide semaphore indicating data is ready
            if (xSemaphoreTake(smi_glob_sem, DELAY_STORMAN * 2) == pdTRUE) {
                shell_print(smi_glob.sm_item_data);
            }
        }
        else if (strcmp(argv[1], "unmount") == 0 && argc == 2) {
            smi.action = UNMOUNT;
            storman_request(&smi);
            shell_print("/mnt folder unmounted, restart storagemanager service to re-mount");
        }
        else {syntax_err = true;}
    }
    else {syntax_err = true;}

    if (syntax_err) {
        shell_print("command syntax error, see 'help <flash0>'");
    }
}

// mnt directory files descriptor
static const struct ush_file_descriptor mnt_files[] = {
    {
        .name = "flash0",
        .description = "onboard flash filesystem",
        .help = "usage: flash0 <lsdir|mkdir|rmdir|mkfile|rmfile|dumpfile> <\e[3mname\e[0m>,\r\n"
                "              <readfile> <\e[3mname\e[0m> <\e[3moffset\e[0m> <\e[3mlength\e[0m>,\r\n"
                "              <writefile|appendfile> <\e[3mname\e[0m> <\e[3mdata\e[0m>,\r\n"
                "              <filestat> <\e[3mname\e[0m>,\r\n"
                "              <fsstat>\r\n"
                "              <format>\r\n"
                "              <unmount>\r\n",
        .exec = flash0_exec_callback,
        .get_data = NULL,
        .set_data = NULL
    }
};

// mnt directory handler
static struct ush_node_object mnt;

void shell_mnt_mount(void)
{
    // mount the /mnt directory
    ush_node_mount(&ush, "/mnt", &mnt, mnt_files, sizeof(mnt_files) / sizeof(mnt_files[0]));
}

void shell_mnt_unmount(void)
{
    // unmount the /mnt directory
    ush_node_unmount(&ush, "/mnt");
}