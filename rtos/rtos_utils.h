/******************************************************************************
 * @file rtos_utils.h
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

#ifndef RTOS_UTILS_H
#define RTOS_UTILS_H

#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"


/**
* @brief Check and update task against scheduler parameters.
*
* Checks the current task's OS scheduler info and compares against pre-defined
* schedule parameters (see services.h), blocking the task if necessary per the
* given parameters.
*
* @param repeat number of times to repeat a task, see services.h
* @param delay  number of OS ticks to block a task, see services.h
*
* @return nothing
*/
void task_sched_update(uint32_t repeat, const TickType_t delay);

#endif

/**
* @brief Delay a task by a specified number of milliseconds.
*
* Converts the provided millisecond value into OS ticks, and calls the FreeRTOS
* vTaskDelay function to block the calling task for at least that amount of time.
*
* @param delay_ms time in milliseconds for the calling task to block itself
*
* @return nothing
*/
void task_delay_ms(uint32_t delay_ms);