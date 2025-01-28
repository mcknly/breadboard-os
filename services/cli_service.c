/******************************************************************************
 * @file cli_service.c
 *
 * @brief CLI service implementation and FreeRTOS task creation.
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

#include <microshell.h>
#include <string.h>
#include <git.h>
#include "version.h"
#include "hardware_config.h"
#include "rtos_utils.h"
#include "shell.h"
#include "cli_utils.h"
#include "services.h"
#include "service_queues.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#ifndef PRINT_MOTD_AT_BOOT
#define PRINT_MOTD_AT_BOOT false
#endif


static void prvCliTask(void *pvParameters); // microshell cli task
TaskHandle_t xShellTask;
char BBOS_VERSION_MOD; // global "modified version" variable - declared in version.h

// main service function, creates FreeRTOS task from prvCliTask
BaseType_t cli_service(void)
{
    BaseType_t xReturn;
    
    shell_init(); // init microshell cli

    // spawn the cli task
    xReturn = xTaskCreate(
        prvCliTask,
        xstr(SERVICE_NAME_CLI),
        STACK_CLI,
        NULL,
        PRIORITY_CLI,
        &xShellTask
    );

    // print timestamp value
    cli_uart_puts(timestamp());

    if (xReturn == pdPASS) {
        cli_uart_puts("CLI service started\r\n");
    }
    else {
        cli_uart_puts("Error starting the CLI service\r\n");
    }

    return xReturn;
}

// FreeRTOS task created by cli_service
static void prvCliTask(void *pvParameters)
{
    char print_string[PRINT_QUEUE_ITEM_SIZE];
    extern const char *bbos_header_ascii;

    // Set the global "modified version" indicator if on a branch other than main
    BBOS_VERSION_MOD = strcmp(git_Branch(), "main") ? '+' : ' ';

    // delay CLI startup to allow taskmanager to finish with its startup status prints
    vTaskDelay(DELAY_TASKMAN * 5);

    // print MOTD for additional YouTube likes
    if (PRINT_MOTD_AT_BOOT) {
        // initialize toasty graphics - make sure your MCU is liquid cooled...
        cli_uart_puts(timestamp());
        cli_uart_puts("Initializing toasty graphics");
        for (int i = 0; i < 10; i++) {
            wait_here_us(2E5); // waste a bunch of cycles for no reason
            cli_uart_putc('.');
        }
        cli_uart_puts("\r\n");
        print_motd();
    }

    // copy the CLI ASCII header into RAM
    char *cli_header = pvPortMalloc(strlen(bbos_header_ascii+2)); // two extra bytes for BBOS_VERSION_MOD and NULL
    strcpy(cli_header, bbos_header_ascii);
    // add the "modified version" indicator to the header
    memcpy( (cli_header + strlen(cli_header) - 78) , &BBOS_VERSION_MOD, 1); // manually offset to the correct position
    // print the ASCII header before dropping into the CLI
    shell_print(cli_header);
    // free up the RAM
    vPortFree(cli_header);

    while(true) {
        // peek into the print queue to see if an item is available
        if (xQueuePeek(print_queue, (void *)&print_string, 0) == pdTRUE) {
            // only pull an item from the queue if microshell is available to process it
            // note that if any characters have been entered at the prompt, it will
            // block prints from the queue so as not to interfere with user input
            if (ush.state == USH_STATE_READ_CHAR && ush.in_pos == 0) {
                if (xQueueReceive(print_queue, (void *)&print_string, 0) == pdTRUE) {
                    shell_print(print_string);
                }
            }
        }

        shell_service(); // the main cli service gets called forever in a loop

        // update this task's schedule
        task_sched_update(REPEAT_CLI, DELAY_CLI);
    }
}