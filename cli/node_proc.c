/******************************************************************************
 * @file node_proc.c
 *
 * @brief /proc folder for the CLI, contains system information
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
#include <microshell.h>
#include <lfs.h>
#include <git.h>
#include "hardware_config.h"
#include "shell.h"
#include "FreeRTOS.h"
#include "task.h"
#include "version.h"


/**
* @brief '/proc/mcuinfo' get data callback function.
*
* Returns infomation about the MCU by using the built-in 'cat' CLI command.
*
* @param ush_file_data_getter Params given by typedef ush_file_data_getter. see ush_types.h
*
* @return nothing, print the data directly so we can malloc/free
*/
size_t mcuinfo_get_data_callback(struct ush_object *self, struct ush_file_descriptor const *file, uint8_t **data)
{
    // use malloc rather than passing a pointer to a static char back to uShell,
    // since it is a rather large array
    char *mcuinfo_msg = pvPortMalloc(250);
    const char* scheduler_state[] = {"suspended", "not started", "running"};
    
    sprintf(mcuinfo_msg, "MCU: " xstr(MCU_NAME) ", running at %ld Hz\r\n", get_sys_clk_hz());
    sprintf(mcuinfo_msg + strlen(mcuinfo_msg), "RP2040 chip version: %d\r\n", get_chip_version()); // MCU silicon version
    sprintf(mcuinfo_msg + strlen(mcuinfo_msg), "RP2040 ROM version: %d\r\n", get_rom_version());   // MCU chip ROM version
    sprintf(mcuinfo_msg + strlen(mcuinfo_msg), "Board type: %s\r\n", xstr(BOARD_TYPE));
    sprintf(mcuinfo_msg + strlen(mcuinfo_msg),
            "RTOS scheduler is [ %s ], %ld tasks registered\r\n",
            scheduler_state[(int32_t)xTaskGetSchedulerState()], uxTaskGetNumberOfTasks());
    sprintf(mcuinfo_msg + strlen(mcuinfo_msg), xstr(PROJECT_NAME) " CLI running on core: %d\r\n", get_core());

    // print directly from this function rather than returning pointer to uShell.
    // this allows us to malloc/free rather than using static memory
    shell_print(mcuinfo_msg);
    vPortFree(mcuinfo_msg);
    // return null since we already printed output
    return 0;
}

/**
* @brief '/proc/version' get data callback function.
*
* Returns version information by using the built-in 'cat' CLI command.
*
* @param ush_file_data_getter Params given by typedef ush_file_data_getter. see ush_types.h
*
* @return nothing, print the data directly so we can malloc/free
*/
size_t version_get_data_callback(struct ush_object *self, struct ush_file_descriptor const *file, uint8_t **data)
{
    // use malloc rather than passing a pointer to a static char array back to uShell,
    // since it is a rather large array
    char *version_msg = pvPortMalloc(300);

    sprintf(version_msg, USH_SHELL_FONT_STYLE_BOLD USH_SHELL_FONT_COLOR_BLUE);
    sprintf(version_msg + strlen(version_msg), xstr(PROJECT_NAME) " version:\t" xstr(PROJECT_VERSION) "\r\n"); // Top level project version
    sprintf(version_msg + strlen(version_msg), USH_SHELL_FONT_STYLE_RESET);
    sprintf(version_msg + strlen(version_msg), "Git commit date:\t%s\r\n",  git_CommitDate());           // Git commit date of build
    sprintf(version_msg + strlen(version_msg), "Git commit hash:\t%s\r\n",  git_CommitSHA1());           // Git commit hash of build
    sprintf(version_msg + strlen(version_msg), "%s version:\t%d.%d\r\n",    BBOS_NAME,                   // BreadboardOS version          
                                                                            BBOS_VERSION_MAJOR,
                                                                            BBOS_VERSION_MINOR);
    sprintf(version_msg + strlen(version_msg), "FreeRTOS version:\t%s\r\n", tskKERNEL_VERSION_NUMBER);   // FreeRTOS Kernel version
    sprintf(version_msg + strlen(version_msg), "%s version:\t%s\r\n", USH_NAME, USH_VERSION);            // microshell version
    sprintf(version_msg + strlen(version_msg), "littlefs version:\t%d.%d\r\n",  LFS_VERSION_MAJOR,       // littlefs version
                                                                                LFS_VERSION_MINOR);

    // print directly from this function rather than returning pointer to uShell.
    // this allows us to malloc/free rather than using static memory
    shell_print(version_msg);
    vPortFree(version_msg);
    // return null since we already printed output
    return 0;
}

/**
* @brief '/proc/resetreason' get data callback function.
*
* Returns the reason for the last reset by using the built-in 'cat' CLI command.
*
* @param ush_file_data_getter Params given by typedef ush_file_data_getter. see ush_types.h
*
* @return length of pointer to the reset reason string
*/
size_t resetreason_get_data_callback(struct ush_object *self, struct ush_file_descriptor const *file, uint8_t **data)
{
    // get reset reason string
    char *reset_reason_string = get_reset_reason_string();
    // copy the pointer to the reset reason string
    *data = (uint8_t*)reset_reason_string;
    // return data size
    return strlen(reset_reason_string);
}

/**
* @brief '/proc/uptime' get data callback function.
*
* Print the system uptime (days, hours, minutes, seconds) since last system boot.
*
* @param ush_file_data_getter Params given by typedef ush_file_data_getter. see ush_types.h
*
* @return nothing, print the data directly so we can malloc/free
*/
size_t uptime_get_data_callback(struct ush_object *self, struct ush_file_descriptor const *file, uint8_t **data)
{
    // get current microsecond timer value
    uint64_t current_time_us = get_time_us();

    // calculate time divisions, i.e. maths
    uint16_t num_seconds = (uint16_t) (current_time_us / 1E6) % 60;
    uint16_t num_minutes = (uint16_t) (((current_time_us / 1E6) - num_seconds) / 60) % 60;
    uint16_t num_hours   = (uint16_t) (((((current_time_us / 1E6) - num_seconds) / 60) - num_minutes) / 60) % 24;
    uint16_t num_days    = (uint16_t) ((((((current_time_us / 1E6) - num_seconds) / 60) - num_minutes) / 60) - num_hours) / 24;

    // build formatted string
    char *uptime_msg = pvPortMalloc(55);
    snprintf(uptime_msg, 55, "System up %d days, %d hours, %d minutes, %d seconds\r\n", num_days, num_hours, num_minutes, num_seconds);

    // print the uptime msg
    shell_print(uptime_msg);
    vPortFree(uptime_msg);

    // return null since we printed directly to the shell
    return 0;
}

// proc directory files descriptor
static const struct ush_file_descriptor proc_files[] = {
    {
        .name = "mcuinfo",                              // file name (required)
        .description = "get details about the MCU",     // optional file description
        .help = NULL,                                   // optional help manual
        .exec = NULL,                                   // optional execute callback
        .get_data = mcuinfo_get_data_callback,          // optional get data (cat) callback
        .set_data = NULL                                // optional set data (echo) callback
    },
    {
        .name = "version",
        .description = "get version numbers of firmware libraries",
        .help = NULL,
        .exec = NULL,
        .get_data = version_get_data_callback,
        .set_data = NULL
    },
    {
        .name = "resetreason",
        .description = "get the last reset reason",
        .help = NULL,
        .exec = NULL,
        .get_data = resetreason_get_data_callback,
        .set_data = NULL
    },
    {
        .name = "uptime",
        .description = "get system uptime since boot",
        .help = NULL,
        .exec = NULL,
        .get_data = uptime_get_data_callback,
        .set_data = NULL
    }
};

// proc directory handler
static struct ush_node_object proc;

void shell_proc_mount(void)
{
    // mount proc directory
    ush_node_mount(&ush, "/proc", &proc, proc_files, sizeof(proc_files) / sizeof(proc_files[0]));
}