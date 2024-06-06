/******************************************************************************
 * @file hw_gpio.c
 *
 * @brief Functions for interacting with the GPIO pins.
 *        The implementation of these functions is MCU-specific and will need
 *        to be changed if ported to a new hardware family.
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
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "FreeRTOS.h"
#include "semphr.h"


// global GPIO settings structure, declared in hardware_config.h
// structure arrays are expanded using the GPIO list defines, so they better
// not be jacked up in that file
struct gpio_settings_t gpio_settings = {
    .gpio_mcu_id = {GPIO_MCU_IDS},
    .gpio_direction = {GPIO_DIRECTIONS},
    .gpio_pull = {GPIO_PULLS},
    .gpio_irq_en = {GPIO_IRQS}
};

// global GPIO last IRQ event storage location
struct gpio_event_t gpio_event;

// global GPIO mutex
SemaphoreHandle_t gpio_mutex;

void gpio_process(uint gpio, uint32_t event_mask) {
    // populate the IRQ event structure
    gpio_event.gpio = gpio;
    gpio_event.event_mask = event_mask;
    gpio_event.timestamp = get_time_us();
}

void gpio_init_all(void) {
    // create GPIO mutex
    gpio_mutex = xSemaphoreCreateMutex();
    
    for (int gpio_num = 0; gpio_num < GPIO_COUNT; gpio_num++) {
        // enable the GPIO pin
        gpio_init(gpio_settings.gpio_mcu_id[gpio_num]);

        // set mode to input or output
        gpio_set_dir(gpio_settings.gpio_mcu_id[gpio_num], gpio_settings.gpio_direction[gpio_num]);

        // if GPIO is an input, set pull direction and IRQ if applicable
        if (gpio_settings.gpio_direction[gpio_num] == GPIO_IN) {
            // set pull direction
            if (gpio_settings.gpio_pull[gpio_num] == GPIO_PULL_UP) {
                gpio_pull_up(gpio_settings.gpio_mcu_id[gpio_num]);
            }
            else if (gpio_settings.gpio_pull[gpio_num] == GPIO_PULL_DOWN) {
                gpio_pull_down(gpio_settings.gpio_mcu_id[gpio_num]);
            }
            else {
                gpio_disable_pulls(gpio_settings.gpio_mcu_id[gpio_num]);
            }

            // set up IRQ if enabled - IRQ is set to happen on both edges
            // ISR function is gpio_process()
            if (gpio_settings.gpio_irq_en[gpio_num] == true) {
                gpio_set_irq_enabled_with_callback(gpio_settings.gpio_mcu_id[gpio_num], GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_process);
            }
        }
    }
}

bool gpio_read_single(uint gpio_id) {
    bool gpio_value = 0;
    if(xSemaphoreTake(gpio_mutex, 10) == pdTRUE) {
        gpio_value = gpio_get(gpio_settings.gpio_mcu_id[gpio_id]);
        xSemaphoreGive(gpio_mutex);
    }
    return gpio_value;
}

void gpio_write_single(uint gpio_id, bool value) {
    // only write to a pin that has been configured as output
    if (gpio_settings.gpio_direction[gpio_id] == GPIO_OUT) {
        if(xSemaphoreTake(gpio_mutex, 10) == pdTRUE) {
            gpio_put(gpio_settings.gpio_mcu_id[gpio_id], value);
            xSemaphoreGive(gpio_mutex);
        }
    }
}

uint32_t gpio_read_all(void) {
    uint32_t gpio_values = 0;

    // cycle through all the configured GPIOs
    for (int gpio_num = 0; gpio_num < GPIO_COUNT; gpio_num++) {
        if(xSemaphoreTake(gpio_mutex, 10) == pdTRUE) {
            // move the read pin state into the correct bit position corresponding to current GPIO index
            gpio_values |= gpio_get(gpio_settings.gpio_mcu_id[gpio_num]) << gpio_num;
            xSemaphoreGive(gpio_mutex);
        }
    }

    return gpio_values;
}

void gpio_write_all(uint32_t gpio_states) {
    bool gpio_value;
    for (int gpio_num = 0; gpio_num < GPIO_COUNT; gpio_num++) {
        // write GPIO if it is set to an output
        if (gpio_settings.gpio_direction[gpio_num] == GPIO_OUT) {
            if(xSemaphoreTake(gpio_mutex, 10) == pdTRUE) {
                // shift out the bit corresponding to the current GPIO index into a single bool
                gpio_value = (bool) ((gpio_states >> gpio_num) & (uint32_t) 1);
                gpio_put(gpio_settings.gpio_mcu_id[gpio_num], gpio_value);
                xSemaphoreGive(gpio_mutex);
            }
        }
    }
}