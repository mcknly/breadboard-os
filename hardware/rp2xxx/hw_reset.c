/******************************************************************************
 * @file hw_reset.c
 *
 * @brief Functions related to resetting the chip. The
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
#include <string.h>
#include "hardware_config.h"
#include "hardware/watchdog.h"
#include "pico/bootrom.h"

#ifdef USING_RP2040
#include "hardware/regs/vreg_and_chip_reset.h" // this register file does not exist for rp2350
#endif


reset_reason_t get_reset_reason(void) {
    reset_reason_t reset_reason;

    // first check if it was a watchdog reboot
    if (watchdog_caused_reboot()) {
        if (watchdog_enable_caused_reboot()) {
            reset_reason = WATCHDOG; // expiration of watchdog timer
        }
        else reset_reason = FORCED;  // program-forced watchdog reboot
    }
    else {
#ifdef USING_RP2040
    // RP2040 CHIP_RESET register - can tell us POR, RUN pin, or debugger reset
    io_rw_32 *chip_reset_reg = (io_rw_32 *)(VREG_AND_CHIP_RESET_BASE + VREG_AND_CHIP_RESET_CHIP_RESET_OFFSET);
    if (*chip_reset_reg & VREG_AND_CHIP_RESET_CHIP_RESET_HAD_PSM_RESTART_BITS) {
        reset_reason = DEBUGGER; // reset from debugger -- currently this is not detecting correctly
    }
    else if (*chip_reset_reg & VREG_AND_CHIP_RESET_CHIP_RESET_HAD_POR_BITS) {
        reset_reason = POWERON;  // power-on or brownout
    }
    else if (*chip_reset_reg & VREG_AND_CHIP_RESET_CHIP_RESET_HAD_RUN_BITS) {
        reset_reason = PIN;      // reset pin ("run" pin) toggled
    }
    else reset_reason = UNKNOWN; // can't determine reset reason
#elif USING_RP2350
    reset_reason = UNKNOWN; // TODO: implement reset_reason for rp2350
#else
    reset_reason = UNKNOWN;
#endif
    return reset_reason;
    }
}

char* get_reset_reason_string(void) {
    // get the last reset reason
    reset_reason_t reset_reason = get_reset_reason();
    static char reset_reason_string[40];
    
    strcpy(reset_reason_string, "Last reset reason: ");

    switch (reset_reason)
    {
    case POWERON:
        strcpy(reset_reason_string + strlen(reset_reason_string), "power-on\r\n");
        break;
    case WATCHDOG:
        strcpy(reset_reason_string + strlen(reset_reason_string), "watchdog\r\n");
        break;
    case FORCED:
        strcpy(reset_reason_string + strlen(reset_reason_string), "program-requested\r\n");
        break;
    case PIN:
        strcpy(reset_reason_string + strlen(reset_reason_string), "reset pin asserted\r\n");
        break;
    case DEBUGGER:
        strcpy(reset_reason_string + strlen(reset_reason_string), "debugger\r\n");
        break;
    case UNKNOWN:
        strcpy(reset_reason_string + strlen(reset_reason_string), "unknown\r\n");
        break;
    default:
        break;
    }

    return reset_reason_string;
}

void reset_to_bootloader(void) {
    // reset to the USB bootloader (i.e. BOOTSEL for UF2 mode)
    reset_usb_boot(0, 0);
}