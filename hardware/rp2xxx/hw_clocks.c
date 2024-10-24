/******************************************************************************
 * @file hw_clocks.c
 *
 * @brief Functions for interacting with clocks and timers. The implementation
 *        of these functions is MCU-specific and will need to be changed if
 *        ported to a new hardware family.
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
#include "hardware/clocks.h"
#include "hardware/timer.h"


uint32_t get_sys_clk_hz(void){
    return clock_get_hz(clk_sys);
}

uint64_t get_time_us(void){
    return time_us_64();
}

void wait_here_us(uint64_t delay_us) {
    busy_wait_us(delay_us);
}