/******************************************************************************
 * @file hw_adc.c
 *
 * @brief Functions for interacting with the analog-to-digital converter
 *        peripheral(s). The implementation of these functions is MCU-specific
 *        and will need to be changed if ported to a new hardware family.
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
#include "hardware/adc.h"
#include "semphr.h"

// global ADC mutex
SemaphoreHandle_t adc_mutex;


void adcs_init(void) {
    // create ADC mutex
    adc_mutex = xSemaphoreCreateMutex();

    adc_init();
    if (ADC0_INIT)
        adc_gpio_init(ADC0_GPIO);
    if (ADC1_INIT)
        adc_gpio_init(ADC1_GPIO);
    if (ADC2_INIT)
        adc_gpio_init(ADC2_GPIO);
}

float read_adc(int adc_channel) {
    uint16_t result = 0;
    if(xSemaphoreTake(adc_mutex, 10) == pdTRUE) {
        // select ADC input pin/channel (0-2)
        adc_select_input(adc_channel);
        // read raw value
        result = adc_read();
        xSemaphoreGive(adc_mutex);
    }
    // return floating-point voltage
    return result * ADC_CONV_FACT;
}