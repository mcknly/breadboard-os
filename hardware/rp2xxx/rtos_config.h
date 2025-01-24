/******************************************************************************
 * @file rtos_config.h
 *
 * @brief Hardware-specific configuration options for FreeRTOS, included by
 *        rtos/FreeRTOSConfig.h, which contains all the basic settings.
 *        Only FreeRTOS settings that are MCU-specific are defined here.
 *
 * @author Cavin McKinley (MCKNLY LLC)
 *
 * @date 01-14-2025
 * 
 * @copyright Copyright (c) 2025 Cavin McKinley (MCKNLY LLC)
 *            Released under the MIT License
 * 
 * SPDX-License-Identifier: MIT
 ******************************************************************************/

#ifndef RTOS_CONFIG_H
#define RTOS_CONFIG_H


// FreeRTOS total heap size settings - trial and error to find what linker will accept
// RP2040 has 264KB RAM total, RP2350 has 520KB RAM total. RTOS heap will be something less than this.
// Using the wireless stack appears to chew up about 30-40KB.
#if defined(USING_RP2350) && !defined(HW_USE_WIFI)
#define RTOS_HEAP_SIZE (480*1024) // RP2350 without wireless stack
#elif defined(USING_RP2350) && defined(HW_USE_WIFI)
#define RTOS_HEAP_SIZE (440*1024) // RP2350 with wireless stack
#elif defined(USING_RP2040) && !defined(HW_USE_WIFI)
#define RTOS_HEAP_SIZE (230*1024) // RP2040 without wireless stack
#elif defined(USING_RP2040) && defined(HW_USE_WIFI)
#define RTOS_HEAP_SIZE (188*1024) // RP2040 with wireless stack
#endif

// For FreeRTOS SMP (multicore) support only
#define RTOS_NUM_CORES                   1
#define RTOS_TICK_CORE                   0
#define RTOS_RUN_MULTIPLE_PRIORITIES     1
#define RTOS_USE_CORE_AFFINITY           1
#define RTOS_USE_PASSIVE_IDLE_HOOK       0

// RP2040/RP2350-specific FreeRTOS settings
#define configSUPPORT_PICO_SYNC_INTEROP  1
#define configSUPPORT_PICO_TIME_INTEROP  1
// timer init to be called before runtime stats
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()
extern uint64_t get_time_us(void); // extern declared so we can use here, defined in hw_clocks.c
// divider for proper runtime stats granularity
#define RUN_TIME_STATS_time_us_64_divider (1e6 / configTICK_RATE_HZ)
// function to use for run time stats timer
#define portGET_RUN_TIME_COUNTER_VALUE() (get_time_us() / RUN_TIME_STATS_time_us_64_divider)

// RP2350-specific - ARMv8m/Cortex-m33 options
#ifdef USING_RP2350
#define configENABLE_TRUSTZONE                  0
#define configRUN_FREERTOS_SECURE_ONLY          1
#define configENABLE_FPU                        1
#define configENABLE_MPU                        0
#define configMAX_SYSCALL_INTERRUPT_PRIORITY    16
#endif

#endif /* RTOS_CONFIG_H */