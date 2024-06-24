/******************************************************************************
 * @file taskman_service.c
 *
 * @brief taskmanager service implementation and FreeRTOS task creation.
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
#include "hardware_config.h"
#include "rtos_utils.h"
#include "shell.h"
#include "services.h"
#include "service_queues.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"


static void prvTaskManagerTask(void *pvParameters);
TaskHandle_t xTaskManTask;

// main service function, creates FreeRTOS task from prvTaskManagerTask
BaseType_t taskman_service(void)
{
    BaseType_t xReturn;

    // create the FreeRTOS task
    xReturn = xTaskCreate(
        prvTaskManagerTask,
        xstr(SERVICE_NAME_TASKMAN),
        STACK_TASKMAN,
        NULL,
        PRIORITY_TASKMAN,
        &xTaskManTask
    );

    // print timestamp value
    cli_uart_puts(timestamp());

    if (xReturn == pdPASS) {
        // initialize all service queues
        if (init_queues() == 0) {
            cli_uart_puts("Task manager registered\r\n");
        }
        else {
            cli_uart_puts("Error creating task queues\r\n");
        }
    }
    else {
        // if we got here, taskman service failed, no other services will start
        cli_uart_puts("Error starting the taskmanager service\r\n");
    }

    return xReturn;
}

// FreeRTOS task created by taskman_service
static void prvTaskManagerTask(void *pvParameters)
{
    struct taskman_item_t tmi;

    // FreeRTOS scheduler is running by the time we get here. Print a message
    cli_uart_puts(timestamp());
    cli_uart_puts("FreeRTOS is running!\r\n");
    
    // launch startup services
    cli_uart_puts(timestamp());
    cli_uart_puts("Starting all bootup services...\r\n");
    int i;
    for (i = 0; i < service_descriptors_length; i++) {
        if(service_descriptors[i].startup) {
            service_descriptors[i].service_func();
        }
    }


    // At this point the system is fully running. Print a message
    cli_uart_puts(timestamp());
    cli_uart_puts("All startup services launched.\r\n");
    
    while(true) {
        // check for any task actions in the queue
        if (xQueueReceive(taskman_queue, (void *)&tmi, 0) == pdTRUE) {
            // perform the action. note that "START" is not implemented in task manager
            // because an unregistered task does not have a TaskHandle yet (used
            // in the taskman_item struct). This could be a future enhancement
            switch (tmi.action)
            {
                case DELETE:
                    vTaskDelete(tmi.task);
                    break;

                case SUSPEND:
                    vTaskSuspend(tmi.task);
                    break;

                case RESUME:
                    vTaskResume(tmi.task);
                    break;
                
                default:
                    break;
            }
        }

        // update this task's schedule
        task_sched_update(REPEAT_TASKMAN, DELAY_TASKMAN);
    }
}