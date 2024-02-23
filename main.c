/******************************************************************************
 * @file main.c
 *
 * @brief Main entrypoint of the application.
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
#include "device_drivers.h"
#include "services/services.h"
#include "task.h"


/************************
 * Here be main
*************************/

void main()
{
    // initialize hardware
    hardware_init();

    // initialize any connected peripheral devices
    driver_init();

    // register the taskmanager base service
    taskman_service();

    // start the rtos scheduler (boot the system!)
    vTaskStartScheduler();

    // will not reach here unless rtos crashes, reboot if that happens
    force_watchdog_reboot();
}