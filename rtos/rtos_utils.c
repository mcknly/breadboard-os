/******************************************************************************
 * @file rtos_utils.c
 *
 * @brief Utility functions to use for various RTOS interactions
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

#include <stdint.h>
#include "hardware_config.h"
#include "rtos_utils.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"


void task_sched_update(uint32_t repeat, const TickType_t delay) {

#ifdef SCHED_TEST_DELAY
    // used for OS scheduler testing - force task to take at least this
    // much time for more usable data from vTaskGetRunTimeStats()
    // set this to at least the OS tick rate
    wait_here_us(1000);
#endif

    if (xTaskGetTickCount() % repeat == 0) {
        vTaskDelay(delay);
    }
}

void task_delay_ms(uint32_t delay_ms) {
    TickType_t delay_ticks = (TickType_t)(delay_ms * configTICK_RATE_HZ / 1000); // convert millisecond delay to OS ticks
    vTaskDelay(delay_ticks);
}