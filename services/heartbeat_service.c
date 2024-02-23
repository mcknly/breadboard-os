/******************************************************************************
 * @file heartbeat_service.c
 *
 * @brief Heartbeat service implementation and FreeRTOS task creation.
 *        This can be used as a reference for how to create a simple "service"
 *        (FreeRTOS task), and how to pass output strings to the CLI service
 *        (in lieu of stdout/printf) via the 'cli_print' functions which utilize
 *        inter-task queues.
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
#include "rtos_utils.h"
#include "services.h"
#include "service_queues.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"


// when using this example to implement a new service, replace all references
// to "hearbeat" with the new service name.

static void prvHeartbeatTask(void *pvParameters); // entry function for this service, implementation below
TaskHandle_t xHeartbeatTask; // FreeRTOS task handle for this service

// service creation function, creates FreeRTOS task xHeartbeatTask from entry function prvHeartbeatTask
// this is the function pointer added to the service_functions[] array in services.h
BaseType_t heartbeat_service(void)
{
    BaseType_t xReturn;

    xReturn = xTaskCreate(
        prvHeartbeatTask,             // main function of this service, defined below
        xstr(SERVICE_NAME_HEARTBEAT), // name defined in services.h
        STACK_HEARTBEAT,              // stack size defined in services.h
        NULL,                         // parameters passed to created task (not used)
        PRIORITY_HEARTBEAT,           // priority of this service, defined in services.h
        &xHeartbeatTask               // FreeRTOS task handle
    );

    if (xReturn == pdPASS) {
        cli_print_raw("heartbeat service started");
    }
    else {
        cli_print_raw("Error starting the heartbeat service");
    }

    return xReturn;
}

// FreeRTOS task created by heartbeat_service
static void prvHeartbeatTask(void *pvParameters)
{
    //
    // Service startup (run once) code can be placed here
    // (similar to Arduino setup(), if that's your thing)
    //
    static char *heartbeat_string = "ba-bump";

    while(true) {
        //
        // Main service (run continuous) code can be placed here
        // (similar to Arduino loop(), if that's your thing)
        //
        cli_print_timestamped(heartbeat_string);
        
        // always include the below, with REPEAT & DELAY settings in services.h,
        // otherwise the service will starve out other RTOS tasks

        // update this task's schedule
        task_sched_update(REPEAT_HEARTBEAT, DELAY_HEARTBEAT);
    }

    // the task runs forever unless the RTOS kernel suspends or kills it.
    // for a one-shot service, just eliminate the infinite loop.
}