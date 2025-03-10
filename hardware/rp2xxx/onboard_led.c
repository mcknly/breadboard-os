/******************************************************************************
 * @file onboard_led.c
 *
 * @brief Functions to interact with the onboard LED GPIO. The implementation of
 *        these functions is MCU-specific and will need to be changed if ported
 *        to a new hardware family.
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


#ifdef HAS_CYW43
#include "pico/cyw43_arch.h"

void onboard_led_init(void) {
    // no work to do with CYW43 LED pin
}

void onboard_led_set(bool led_state) {
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_state);
}

bool onboard_led_get(void) {
    return cyw43_arch_gpio_get(CYW43_WL_GPIO_LED_PIN);
}
#else
void onboard_led_init(void) {
    gpio_init(PIN_NO_ONBOARD_LED);
    gpio_set_dir(PIN_NO_ONBOARD_LED, GPIO_OUT);
}

void onboard_led_set(bool led_state) {
    gpio_put(PIN_NO_ONBOARD_LED, led_state);
}

bool onboard_led_get(void) {
    return gpio_get(PIN_NO_ONBOARD_LED);
}
#endif
