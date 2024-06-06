/******************************************************************************
 * @file hw_cores.c
 *
 * @brief Functions for querying information about MCU cores. The implementation
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

#include <stdint.h>
#include "hardware_config.h"
#include "pico/platform.h"


uint8_t get_core(void){
    return get_core_num();
}