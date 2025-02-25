/******************************************************************************
 * @file hw_registers.c
 *
 * @brief Functions for interacting directly with MCU hardware registers. The
 *        implementation of these functions is MCU-specific and will need to be
 *        changed if ported to a new hardware family.
 *
 * @author Cavin McKinley (MCKNLY LLC)
 *
 * @date 01-15-2025
 *
 * @copyright Copyright (c) 2025 Cavin McKinley (MCKNLY LLC)
 *            Released under the MIT License
 *
 * SPDX-License-Identifier: MIT
 ******************************************************************************/

#include <stdint.h>
#include "hardware_config.h"


uint32_t read_chip_register(uint32_t reg_addr) {
    return *(volatile uint32_t *)reg_addr;
}