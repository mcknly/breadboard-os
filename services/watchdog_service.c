/******************************************************************************
 * @file watchdog_service.c
 *
 * @brief watchdog service implementation and FreeRTOS task creation.
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

#include "hardware_config.h"
#include "services.h"
#include "rtos_utils.h"
#include "FreeRTOS.h"
#include "task.h"


static void prvWatchdogTask(void *pvParameters);
TaskHandle_t xWatchdogTask;

// main service function, creates FreeRTOS task from prvWatchdogTask
BaseType_t watchdog_service(void)
{
    BaseType_t xReturn;

    // create the FreeRTOS task
    xReturn = xTaskCreate(
        prvWatchdogTask,
        xstr(SERVICE_NAME_WATCHDOG),
        STACK_WATCHDOG,
        NULL,
        PRIORITY_WATCHDOG,
        &xWatchdogTask
    );

    // print timestamp value
    cli_uart_puts(timestamp());

    if (xReturn == pdPASS) {
        cli_uart_puts("Watchdog service started\r\n");
    }
    else {
        cli_uart_puts("Error starting the watchdog service\r\n");
    }

    return xReturn;
}

// FreeRTOS task created by watchdog_service
static void prvWatchdogTask(void *pvParameters)
{
    // enable the watchdog timer
    watchdog_en(WATCHDOG_DELAY_MS);

    while(true) {
        // reset watchdog timer
        watchdog_kick();
        
        // update this task's schedule
        task_sched_update(REPEAT_WATCHDOG, DELAY_WATCHDOG);
    }
}