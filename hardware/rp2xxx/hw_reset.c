/******************************************************************************
 * @file hw_reset.c
 *
 * @brief Functions related to resetting the chip. The implementation of these
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

#include <stdint.h>
#include <string.h>
#include "hardware_config.h"
#include "hardware/watchdog.h"
#include "pico/bootrom.h"

#ifdef USING_RP2040
#include "hardware/regs/vreg_and_chip_reset.h"
#elif USING_RP2350
#include "hardware/structs/powman.h"
#include "hardware/regs/powman.h"
#endif


reset_reason_t get_reset_reason(void) {
    reset_reason_t reset_reason;

#ifdef USING_RP2040
    // RP2040 CHIP_RESET register is stored in LDO_POR block -
    // it can tell us POR, RUN pin, or debugger reset
    io_rw_32 *chip_reset_reg = (io_rw_32 *)(VREG_AND_CHIP_RESET_BASE + VREG_AND_CHIP_RESET_CHIP_RESET_OFFSET);
    // first check if it was a watchdog reboot before interrogating the reset register
    if (watchdog_caused_reboot()) {
        if (watchdog_enable_caused_reboot()) {
            reset_reason = WATCHDOG; // expiration of watchdog timer
        }
        else reset_reason = FORCED;  // program-forced watchdog reboot
    }
    else if (*chip_reset_reg & VREG_AND_CHIP_RESET_CHIP_RESET_HAD_PSM_RESTART_BITS) {
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
    // RP2350 CHIP_RESET register is stored in POWMAN block -
    // it can tell us POR, RUN pin, or debugger reset, plus glitch & brownout
    powman_hw_t *powman_reg = powman_hw;
    // first check if it was a watchdog reboot before interrogating the reset register
    if (watchdog_caused_reboot()) {
        if (watchdog_enable_caused_reboot()) {
            reset_reason = WATCHDOG; // expiration of watchdog timer
        }
        else reset_reason = FORCED;  // program-forced watchdog reboot
    }
    else if (powman_reg->chip_reset & POWMAN_CHIP_RESET_HAD_DP_RESET_REQ_BITS) {
        reset_reason = DEBUGGER;  // reset from ARM debugger
    }
    else if (powman_reg->chip_reset & POWMAN_CHIP_RESET_HAD_POR_BITS) {
        reset_reason = POWERON;   // normal power-on
    }
    else if (powman_reg->chip_reset & POWMAN_CHIP_RESET_HAD_GLITCH_DETECT_BITS) {
        reset_reason = GLITCH;    // reset from power glitch detect
    }
    else if (powman_reg->chip_reset & POWMAN_CHIP_RESET_HAD_BOR_BITS) {
        reset_reason = BROWNOUT;  // reset from brown-out detect
    }
    else if (powman_reg->chip_reset & POWMAN_CHIP_RESET_HAD_RUN_LOW_BITS) {
        reset_reason = PIN;       // reset pin ("run" pin) toggled
    }
    else if (powman_reg->chip_reset & POWMAN_CHIP_RESET_DOUBLE_TAP_BITS) {
        reset_reason = DOUBLETAP; // reset pin ("run" pin) double-tapped
    }
    else reset_reason = UNKNOWN;  // can't determine reset reason
#else
    reset_reason = UNKNOWN;
#endif
    return reset_reason;
    }

char* get_reset_reason_string(reset_reason_t reset_reason) {
    static char reset_reason_string[50];
    
    strcpy(reset_reason_string, "Last reset reason: ");

    switch (reset_reason)
    {
    case POWERON:
        strcpy(reset_reason_string + strlen(reset_reason_string), "power-on\r\n");
        break;
    case GLITCH:
        strcpy(reset_reason_string + strlen(reset_reason_string), "power supply glitch\r\n");
        break;
    case BROWNOUT:
        strcpy(reset_reason_string + strlen(reset_reason_string), "power supply brown-out\r\n");
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
    case DOUBLETAP:
        strcpy(reset_reason_string + strlen(reset_reason_string), "reset pin double-tap\r\n");
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