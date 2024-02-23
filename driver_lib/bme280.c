/******************************************************************************
 * @file bme280.c
 *
 * @brief Hardware interface driver functions for the Bosch BME280 environmental
 *        sensor peripheral. Calibration routines based on Bosch datasheet, using
 *        a ridiculously convoluted confluence of algorithms.
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
#include "device_drivers.h"
#include "hardware_config.h"

// global storage for bme280 compensation params, read out once in bme280_init()
bme280_compensation_params_t bme280_compensation_params_glob;


static int32_t bme280_compensate_temperature(bme280_compensation_params_t *compensation_params, int32_t adc_T) {
    int32_t var1, var2, T;

    // calculate compensation factors for temperature reading - is there any possible way there isn't a typo here?
    var1 = ((((adc_T >> 3) - ((int32_t) compensation_params->temp_comp.T1 << 1))) * ((int32_t) compensation_params->temp_comp.T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((int32_t) compensation_params->temp_comp.T1)) * ((adc_T >> 4) - ((int32_t) compensation_params->temp_comp.T1))) >> 12) * ((int32_t) compensation_params->temp_comp.T3)) >> 14;

    // save fine temperature for other compensations - temp calc needs to always come before pressure, humidity
    compensation_params->temp_fine = var1 + var2;
    // calculate calibrated temperature
    T = (compensation_params->temp_fine * 5 + 128) >> 8;

    return T;
}

static uint32_t bme280_compensate_pressure(bme280_compensation_params_t *compensation_params, int32_t adc_P) {
    int64_t var1, var2, P;

    // calculate compensation factors for barometric pressure reading - seriously I have never manipulated so many bits
    var1 = ((int64_t) compensation_params->temp_fine) - 128000;
    var2 = var1 * var1 * (int64_t) compensation_params->press_comp.P6;
    var2 = var2 + ((var1 * (int64_t) compensation_params->press_comp.P5) << 17);
    var2 = var2 + ((int64_t) compensation_params->press_comp.P4 << 35);
    var1 = ((var1 * var1 * (int64_t) compensation_params->press_comp.P3) >> 8) + ((var1 * (int64_t) compensation_params->press_comp.P2) << 12);
    var1 = (((((int64_t) 1) << 47) + var1)) * ((int64_t) compensation_params->press_comp.P1) >> 33;

    if (var1 == 0) return 0; // avoid possible divide by zero exception by returning early

    P = 1048576 - adc_P;
    P = (((P << 31) - var2) * 3125) / var1;

    // further calculate, the BME280 is a great example of how not to design an ASIC data interface
    var1 = (((int64_t) compensation_params->press_comp.P9) * (P >> 13) * (P >> 13)) >> 25;
    var2 = (((int64_t) compensation_params->press_comp.P8) * P) >> 19;

    // calculate final calibrated barometric pressure
    P = ((P + var1 + var2) >> 8) + (((int64_t) compensation_params->press_comp.P7) << 4);

    return (uint32_t) P;
}

static uint32_t bme280_compensate_humidity(bme280_compensation_params_t *compensation_params, int32_t adc_H) {
    int32_t H;
    
    // here we go again...
    H = (compensation_params->temp_fine - ((int32_t) 76800));
    H = (((((adc_H << 14) - (((int32_t) compensation_params->hum_comp.H4) << 20) - (((int32_t) compensation_params->hum_comp.H5) * H)) +
        ((int32_t) 16384)) >> 15) * (((((((H * ((int32_t) compensation_params->hum_comp.H6)) >> 10) * (((H *((int32_t) compensation_params->hum_comp.H3)) >> 11) +
        ((int32_t) 32768))) >> 10) + ((int32_t) 2097152)) * ((int32_t) compensation_params->hum_comp.H2) + 8192) >> 14));
    H = (H - (((((H >> 15) * (H >> 15)) >> 7) * ((int32_t) compensation_params->hum_comp.H1)) >> 4));
    H = (H < 0 ? 0 : H);
    H = (H > 419430400 ? 419430400 : H);
    // I'm as stumped as you are

    return (uint32_t) (H >> 12);
}

int bme280_read_compensation_parameters(bme280_compensation_params_t *compensation_params) {
    uint8_t comp_params_buffer[26];
    int bytes_read;

    // read out factory compensation parameter data bytes 0-25 (comp data is split between 2 memory blocks)
    bytes_read = spi0_read_registers(SPI0_TARGET_DEV_0_CS, 0x88, comp_params_buffer, 26);

    if (bytes_read == 26) {
        // break out data into individual compensation factors - following
        // sec. 4.2.2 of BME280 data sheet "Trimming parameter readout"

        // temperature compensation
        compensation_params->temp_comp.T1 = comp_params_buffer[0] | (comp_params_buffer[1] << 8);
        compensation_params->temp_comp.T2 = comp_params_buffer[2] | (comp_params_buffer[3] << 8);
        compensation_params->temp_comp.T3 = comp_params_buffer[4] | (comp_params_buffer[5] << 8);

        // barometric pressure compensation
        compensation_params->press_comp.P1 = comp_params_buffer[6] | (comp_params_buffer[7] << 8);
        compensation_params->press_comp.P2 = comp_params_buffer[8] | (comp_params_buffer[9] << 8);
        compensation_params->press_comp.P3 = comp_params_buffer[10] | (comp_params_buffer[11] << 8);
        compensation_params->press_comp.P4 = comp_params_buffer[12] | (comp_params_buffer[13] << 8);
        compensation_params->press_comp.P5 = comp_params_buffer[14] | (comp_params_buffer[15] << 8);
        compensation_params->press_comp.P6 = comp_params_buffer[16] | (comp_params_buffer[17] << 8);
        compensation_params->press_comp.P7 = comp_params_buffer[18] | (comp_params_buffer[19] << 8);
        compensation_params->press_comp.P8 = comp_params_buffer[20] | (comp_params_buffer[21] << 8);
        compensation_params->press_comp.P9 = comp_params_buffer[22] | (comp_params_buffer[23] << 8);

        // humidity compensation
        compensation_params->hum_comp.H1 = comp_params_buffer[25];
        // read out factory compensation parameter data bytes 26-32, re-using the same buffer
        spi0_read_registers(SPI0_TARGET_DEV_0_CS, 0xE1, comp_params_buffer, 7);
        compensation_params->hum_comp.H2 = comp_params_buffer[0] | (comp_params_buffer[1] << 8);
        compensation_params->hum_comp.H3 = comp_params_buffer[2];
        compensation_params->hum_comp.H4 = (comp_params_buffer[3] << 4) | (comp_params_buffer[4] & 0x0F);
        compensation_params->hum_comp.H5 = (comp_params_buffer[4] >> 4) | (comp_params_buffer[5] << 4);
        compensation_params->hum_comp.H6 = comp_params_buffer[7];

        return 1;
    }
    else return 0;
}

int bme280_init(void) {
    int registers_written = 0;

    // read out compensation parameters from device ROM and store in global structure
    if (bme280_read_compensation_parameters(&bme280_compensation_params_glob)) {
        // humidity oversampling register = x1
        registers_written += spi0_write_register(SPI0_TARGET_DEV_0_CS, 0xF2, 0x01);
        // set other oversampling modes, run mode normal
        registers_written += spi0_write_register(SPI0_TARGET_DEV_0_CS, 0xF4, 0x27);
    }

    // make sure BME280 configuration has taken place
    if (registers_written == 2)
        return 1;
    else return 0;
}

int bme280_read_sensors(bme280_compensation_params_t *compensation_params, bme280_sensor_data_t *sensor_data) {
    uint8_t  raw_data_buffer[8];
    int32_t  temp_raw;
    uint32_t press_raw, hum_raw;
    
    // read raw data (uncalibrated) from sensors
    if (spi0_read_registers(SPI0_TARGET_DEV_0_CS, 0xF7, raw_data_buffer, 8) == 8) {
        // shift data into the appropriate bin (temp, press, hum)
        temp_raw  = ((uint32_t) raw_data_buffer[3] << 12) | ((uint32_t) raw_data_buffer[4] << 4) | (raw_data_buffer[5] >> 4);
        press_raw = ((uint32_t) raw_data_buffer[0] << 12) | ((uint32_t) raw_data_buffer[1] << 4) | (raw_data_buffer[2] >> 4);
        hum_raw   = ((uint32_t) raw_data_buffer[6] << 8) | raw_data_buffer[7];

        // apply corrections and save to sensor data struct
        sensor_data->temperature = (float) bme280_compensate_temperature(compensation_params, temp_raw) / 100;
        sensor_data->pressure    = (float) bme280_compensate_pressure(compensation_params, press_raw) / 25600;
        sensor_data->humidity    = (float) bme280_compensate_humidity(compensation_params, hum_raw) / 1024;

        return 1;
    }
    else return 0;
}