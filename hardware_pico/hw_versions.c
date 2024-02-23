/******************************************************************************
 * @file hw_versions.c
 *
 * @brief Functions to query MCU version numbers. The implementation of these
 *        functions is MCU-specific and will need to be changed if ported to a
 *        new hardware family.
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
#include "pico/platform.h"


uint8_t get_chip_version(void){
    return rp2040_chip_version();
}

uint8_t get_rom_version(void){
    return rp2040_rom_version();
}