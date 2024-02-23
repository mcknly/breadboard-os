/******************************************************************************
 * @file device_drivers.h
 *
 * @brief Driver interfaces for peripheral hardware devices
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

#ifndef DEVICE_DRIVERS_H
#define DEVICE_DRIVERS_H

#include <stdint.h>
#include <stdbool.h>


/**************************
 * Peripheral Configuration
***************************/

// use the below settings to specify which devices are attached to the system
// set to true to load driver and run init for this peripheral device
#define BME280_ATTACHED  true
#define MCP4725_ATTACHED true

/**
* @brief Initialize all attached peripheral devices.
*
* Single function to initialize all peripheral devices attached to the system,
* to be run once after all onboard hardware interfaces have been brought up
* with hardware_init().
*
* @param none
*
* @return nothing
*/
void driver_init(void);


/************************************
 * Bosch BME280 Environmental Sensor
*************************************/

// bme280 compensation parameters typedef
// these parameters are written into device ROM at manufacture
typedef struct bme280_compensation_params_t {
    struct {
        uint16_t T1;
        int16_t T2, T3;
    } temp_comp;
    struct {
        uint16_t P1;
        int16_t P2, P3, P4, P5, P6, P7, P8, P9;
    } press_comp;
    struct {
        uint8_t H1, H3;
        int8_t H6;
        int16_t H2, H4, H5;
    } hum_comp;
    int32_t temp_fine;
} bme280_compensation_params_t;

// bme280 calibrated sensor readings typedef
typedef struct bme280_sensor_data_t {
    float temperature;
    float pressure;
    float humidity;
} bme280_sensor_data_t;

// global storage for bme280 compensation params, read out once in bme280_init()
extern bme280_compensation_params_t bme280_compensation_params_glob;

/**
* @brief Initialize the BME280 device.
*
* Initialize the BME20 environmental sensor device by writing any pre-configuration
* parameters and setting up the device to start polling sensor values.
*
* @param none
*
* @return 1 if initialization success, 0 if failure
*/
int bme280_init(void);

/**
* @brief Read out the factory compensation values.
*
* Read out the factory compensation values that are stored in device ROM during
* manufacture, to be applied to the raw sensor readings for calibrated results.
* This only needs to be done once, and then applied to all further sensor readings.
*
* @param compensation_params pointer to the compensation data structure to store values
*
* @return 1 if compensation data successfully read, 0 if failed to read
*/
int bme280_read_compensation_parameters(bme280_compensation_params_t *compensation_params);

/**
* @brief Read BME280 sensor data.
*
* Reads out current sensor data readings from the BME280 and applies the calibration
* parameters previously read out from the device with bme280_read_compensation_parameters.
*
* @param compensation_params pointer to the compensation values data structure
* @param sensor_data pointer to the structure to hold the calibrated sensor readings
*
* @return 1 if sensor data read successfully, 0 if failed to read
*/
int bme280_read_sensors(bme280_compensation_params_t *compensation_params, bme280_sensor_data_t *sensor_data);



/************************************************
 * Microchip MCP4725 Digital-to-Analog Converter
*************************************************/

// MCP4725 specific settings
// VDD rail voltage - defines scaling for the 12-bit DAC value
#define MCP4725_VDD 3.3
// I2C address (hex)
#define MCP4725_I2C_ADDR 0x60

/**
* @brief Initialize the MCP4725 device.
*
* Initiate communication with the MCP4725 and verify that the device is
* connected and operational.
*
* @param none
*
* @return 1 if initialization success, 0 if failure
*/
int mcp4725_init(void);

/**
* @brief Set the voltage of the DAC.
*
* Sets the output voltage of the MCP4725 by writing the 12-bit value over I2C.
* The conversion is based on the MCP4725_VDD parameter definition. The value can
* be written in "fast mode" (don't save to EEPROM), or, one can choose to store
* the value to built-in EEPROM so that the DAC setting is preserved through
* power cycles.
*
* @param voltage desired output voltage from 0 - VCC
* @param save_in_eeprom also write the DAC setting into internal NVM
*
* @return 1 if successfully set, 0 if fail
*/
int mcp4725_set_voltage(float voltage, bool save_in_eeprom);

/**
* @brief Get the current DAC voltage
*
* Gets the current value of the DAC setting in volts. This is the real-time
* setting and does not reflect what might be stored in built-in EEPROM.
*
* @param none
*
* @return floating-point voltage setting of the DAC (negative indicates error)
*/
float mcp4725_get_voltage(void);


#endif /* DEVICE_DRIVERS_H */