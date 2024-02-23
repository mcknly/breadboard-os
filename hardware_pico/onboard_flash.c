/******************************************************************************
 * @file onboard_flash.c
 *
 * @brief Functions for accessing the 'onboard' flash memory (application
 * storage mem). Intended to be used with a fixed chunk of leftover storage at
 * the end of flash not used by the application. Note that the functions may be
 * used standalone, however the argument format was structured so that they
 * could be leveraged by LittleFS. The implementation of these functions is
 * MCU-specific and will need to be changed if ported to a new hardware family.
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
#include "hardware/flash.h"
#include "hardware/regs/addressmap.h"
#include "hardware/sync.h"
#include "lfs.h"
#include "semphr.h"


const char* FLASH0_FS_BASE = (char*)(PICO_FLASH_SIZE_BYTES - FLASH0_FS_SIZE); // 'flash0' filesystem start address is at the end of flash
SemaphoreHandle_t onboard_flash_mutex; // global onboard flash mutex

void onboard_flash_init(void) {
    // create onboard flash mutex
    onboard_flash_mutex = xSemaphoreCreateMutex();
}

int onboard_flash_read(const struct lfs_config *c, uint32_t block, uint32_t offset, void* buffer, uint32_t size) {
    if(xSemaphoreTake(onboard_flash_mutex, 10) == pdTRUE) { // try to acquire flash access
        // copy from address of memory-mapped flash location - read address includes RAM offset XIP_NOCACHE_NOALLOC_BASE
        memcpy(buffer, FLASH0_FS_BASE + XIP_NOCACHE_NOALLOC_BASE + (block * FLASH_SECTOR_SIZE) + offset, size); // note "block" and "sector" are synonomous here
        xSemaphoreGive(onboard_flash_mutex);
        return 0;
    }
    else return -1;
}

int onboard_flash_write(const struct lfs_config *c, uint32_t block, uint32_t offset, const void* buffer, uint32_t size) {
    uint32_t addr = (uint32_t)FLASH0_FS_BASE + (block * FLASH_SECTOR_SIZE) + offset;
    if(xSemaphoreTake(onboard_flash_mutex, 10) == pdTRUE) {  // try to acquire flash access
        uint32_t interrupts = save_and_disable_interrupts(); // disable interrupts since we are writing to program memory
        flash_range_program(addr, buffer, size);
        restore_interrupts(interrupts); // re-enable interrupts
        xSemaphoreGive(onboard_flash_mutex);
        return 0;
    }
    else return -1;
}

int onboard_flash_erase(const struct lfs_config *c, uint32_t block) {
    uint32_t addr = (uint32_t)FLASH0_FS_BASE + (block * FLASH_SECTOR_SIZE);
    if(xSemaphoreTake(onboard_flash_mutex, 10) == pdTRUE) {  // try to acquire flash access
        uint32_t interrupts = save_and_disable_interrupts(); // disable interrupts since we are erasing within program memory
        flash_range_erase(addr, FLASH_SECTOR_SIZE); // erase entire block/sector
        restore_interrupts(interrupts); // re-enable interrupts
        xSemaphoreGive(onboard_flash_mutex);
        return 0;
    }
    else return -1;
}

int onboard_flash_sync(const struct lfs_config *c) {
    return 0; // always success, this is a dummy function
}

flash_usage_t onboard_flash_usage(void) {
    flash_usage_t flash_usage; // the structure to hold flash usage details
    extern char __flash_binary_end; // the last byte in the program binary (page-aligned)
    uintptr_t prog_bin_end_addr = (uintptr_t) &__flash_binary_end; // the address of the binary upper bound

    // calculate flash usage
    flash_usage.flash_total_size = PICO_FLASH_SIZE_BYTES; // assuming the correct board/flash is identified by the SDK
    flash_usage.program_used_size = prog_bin_end_addr - XIP_BASE;
    flash_usage.fs_reserved_size = FLASH0_FS_SIZE;
    flash_usage.flash_free_size = flash_usage.flash_total_size - flash_usage.program_used_size - flash_usage.fs_reserved_size;

    return flash_usage;
}