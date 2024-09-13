/******************************************************************************
 * @file hw_watchdog.c
 *
 * @brief Functions to interact with the watchdog hardware peripheral. The
 *        implementation of these functions is MCU-specific and will need to be
 *        changed if ported to a new hardware family.
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
#include "hardware/watchdog.h"


void watchdog_en(uint32_t delay_ms) {
    watchdog_enable(delay_ms, 1); // pause_on_debug true, should allow us to debug normally
}

void watchdog_kick(void) {
    watchdog_update(); // reset the watchdog timer
}

void force_watchdog_reboot(void) {
    // enable watchdog reboot
    watchdog_reboot(0, 0, WATCHDOG_DELAY_REBOOT_MS);

    // loop until watchdog reboot
    while(1);
}