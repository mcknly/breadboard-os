/******************************************************************************
 * @file hw_wifi.h
 *
 * @brief settings, definitions, typedefs and function prototypes for WiFi and
 *        interacting with the CYW43 wireless module. Hardware and SDK-specific
 *        implementation resides within these functions.
 *
 * @author Alec Lanter (Github @Kintar)
 * @author Cavin McKinley (Github @mcknly)
 *
 * @date 01-24-2025
 *
 * @copyright Copyright (c) 2025 Alec Lanter, Cavin McKinley
 *            Released under the MIT License
 *
 * SPDX-License-Identifier: MIT
 ******************************************************************************/

#ifndef HW_WIFI_H
#define HW_WIFI_H

#include <stdint.h>
#include <stdbool.h>
#include "pico/cyw43_arch.h"


typedef uint32_t hw_wifi_country_t ;

// create a country code from the two character country and revision number
#define HW_WIFI_COUNTRY(A, B, REV) (hw_wifi_country_t)((unsigned char)(A) | ((unsigned char)(B) << 8) | ((REV) << 16))

// Worldwide Locale (passive Ch12-14)
#define HW_WIFI_COUNTRY_WORLDWIDE         HW_WIFI_COUNTRY('X', 'X', 0)

#define HW_WIFI_COUNTRY_AUSTRALIA         HW_WIFI_COUNTRY('A', 'U', 0)
#define HW_WIFI_COUNTRY_AUSTRIA           HW_WIFI_COUNTRY('A', 'T', 0)
#define HW_WIFI_COUNTRY_BELGIUM           HW_WIFI_COUNTRY('B', 'E', 0)
#define HW_WIFI_COUNTRY_BRAZIL            HW_WIFI_COUNTRY('B', 'R', 0)
#define HW_WIFI_COUNTRY_CANADA            HW_WIFI_COUNTRY('C', 'A', 0)
#define HW_WIFI_COUNTRY_CHILE             HW_WIFI_COUNTRY('C', 'L', 0)
#define HW_WIFI_COUNTRY_CHINA             HW_WIFI_COUNTRY('C', 'N', 0)
#define HW_WIFI_COUNTRY_COLOMBIA          HW_WIFI_COUNTRY('C', 'O', 0)
#define HW_WIFI_COUNTRY_CZECH_REPUBLIC    HW_WIFI_COUNTRY('C', 'Z', 0)
#define HW_WIFI_COUNTRY_DENMARK           HW_WIFI_COUNTRY('D', 'K', 0)
#define HW_WIFI_COUNTRY_ESTONIA           HW_WIFI_COUNTRY('E', 'E', 0)
#define HW_WIFI_COUNTRY_FINLAND           HW_WIFI_COUNTRY('F', 'I', 0)
#define HW_WIFI_COUNTRY_FRANCE            HW_WIFI_COUNTRY('F', 'R', 0)
#define HW_WIFI_COUNTRY_GERMANY           HW_WIFI_COUNTRY('D', 'E', 0)
#define HW_WIFI_COUNTRY_GREECE            HW_WIFI_COUNTRY('G', 'R', 0)
#define HW_WIFI_COUNTRY_HONG_KONG         HW_WIFI_COUNTRY('H', 'K', 0)
#define HW_WIFI_COUNTRY_HUNGARY           HW_WIFI_COUNTRY('H', 'U', 0)
#define HW_WIFI_COUNTRY_ICELAND           HW_WIFI_COUNTRY('I', 'S', 0)
#define HW_WIFI_COUNTRY_INDIA             HW_WIFI_COUNTRY('I', 'N', 0)
#define HW_WIFI_COUNTRY_ISRAEL            HW_WIFI_COUNTRY('I', 'L', 0)
#define HW_WIFI_COUNTRY_ITALY             HW_WIFI_COUNTRY('I', 'T', 0)
#define HW_WIFI_COUNTRY_JAPAN             HW_WIFI_COUNTRY('J', 'P', 0)
#define HW_WIFI_COUNTRY_KENYA             HW_WIFI_COUNTRY('K', 'E', 0)
#define HW_WIFI_COUNTRY_LATVIA            HW_WIFI_COUNTRY('L', 'V', 0)
#define HW_WIFI_COUNTRY_LIECHTENSTEIN     HW_WIFI_COUNTRY('L', 'I', 0)
#define HW_WIFI_COUNTRY_LITHUANIA         HW_WIFI_COUNTRY('L', 'T', 0)
#define HW_WIFI_COUNTRY_LUXEMBOURG        HW_WIFI_COUNTRY('L', 'U', 0)
#define HW_WIFI_COUNTRY_MALAYSIA          HW_WIFI_COUNTRY('M', 'Y', 0)
#define HW_WIFI_COUNTRY_MALTA             HW_WIFI_COUNTRY('M', 'T', 0)
#define HW_WIFI_COUNTRY_MEXICO            HW_WIFI_COUNTRY('M', 'X', 0)
#define HW_WIFI_COUNTRY_NETHERLANDS       HW_WIFI_COUNTRY('N', 'L', 0)
#define HW_WIFI_COUNTRY_NEW_ZEALAND       HW_WIFI_COUNTRY('N', 'Z', 0)
#define HW_WIFI_COUNTRY_NIGERIA           HW_WIFI_COUNTRY('N', 'G', 0)
#define HW_WIFI_COUNTRY_NORWAY            HW_WIFI_COUNTRY('N', 'O', 0)
#define HW_WIFI_COUNTRY_PERU              HW_WIFI_COUNTRY('P', 'E', 0)
#define HW_WIFI_COUNTRY_PHILIPPINES       HW_WIFI_COUNTRY('P', 'H', 0)
#define HW_WIFI_COUNTRY_POLAND            HW_WIFI_COUNTRY('P', 'L', 0)
#define HW_WIFI_COUNTRY_PORTUGAL          HW_WIFI_COUNTRY('P', 'T', 0)
#define HW_WIFI_COUNTRY_SINGAPORE         HW_WIFI_COUNTRY('S', 'G', 0)
#define HW_WIFI_COUNTRY_SLOVAKIA          HW_WIFI_COUNTRY('S', 'K', 0)
#define HW_WIFI_COUNTRY_SLOVENIA          HW_WIFI_COUNTRY('S', 'I', 0)
#define HW_WIFI_COUNTRY_SOUTH_AFRICA      HW_WIFI_COUNTRY('Z', 'A', 0)
#define HW_WIFI_COUNTRY_SOUTH_KOREA       HW_WIFI_COUNTRY('K', 'R', 0)
#define HW_WIFI_COUNTRY_SPAIN             HW_WIFI_COUNTRY('E', 'S', 0)
#define HW_WIFI_COUNTRY_SWEDEN            HW_WIFI_COUNTRY('S', 'E', 0)
#define HW_WIFI_COUNTRY_SWITZERLAND       HW_WIFI_COUNTRY('C', 'H', 0)
#define HW_WIFI_COUNTRY_TAIWAN            HW_WIFI_COUNTRY('T', 'W', 0)
#define HW_WIFI_COUNTRY_THAILAND          HW_WIFI_COUNTRY('T', 'H', 0)
#define HW_WIFI_COUNTRY_TURKEY            HW_WIFI_COUNTRY('T', 'R', 0)
#define HW_WIFI_COUNTRY_UK                HW_WIFI_COUNTRY('G', 'B', 0)
#define HW_WIFI_COUNTRY_USA               HW_WIFI_COUNTRY('U', 'S', 0)

// wifi auth types
typedef enum{
    HW_WIFI_AUTH_OPEN,
    HW_WIFI_AUTH_WPA_TPIK_PSK,
    HW_WIFI_AUTH_WPA2_AES_PSK,
    HW_WIFI_AUTH_MIXED,
} hw_wifi_auth_t ;

// wifi modes
typedef enum{
    HW_WIFI_MODE_NONE,
    HW_WIFI_MODE_STA,
    HW_WIFI_MODE_AP,
} hw_wifi_mode_t ;

// wifi status
typedef enum{
    HW_WIFI_STATUS_LINK_DOWN,
    HW_WIFI_STATUS_JOINED,
    HW_WIFI_STATUS_NOIP,
    HW_WIFI_STATUS_UP,
    HW_WIFI_STATUS_FAIL,
    HW_WIFI_STATUS_BADAUTH,
    HW_WIFI_STATUS_NONET,

    HW_WIFI_STATUS_UNKNOWN,
} hw_wifi_status_t ;


/**************************************************
 * Wireless module (CYW43) initialization functions
***************************************************/

/**
* @brief Perform a hard reset of the CYW43 module.
*
* Force a hard reset of the CYW43 module by toggling the reset pin with the
* appropriate RP2xxx GPIO pin.
*
* @param none
*
* @return nothing
*/
void hw_wifi_hard_reset(void);

/**
* @brief Check if the CYW43 module is initialized.
*
* Performs a check to determine if the CYW43 module has already been initialized
* so that it is not tried multiple times, with a boolean response to indicate.
*
* @param none
*
* @return true if the CYW43 module is already initialized, otherwise false
*/
bool hw_wifi_is_initialized(void);

/**
* @brief Initialize the CYW43 module without country code.
*
* Check weather the CYW43 module has already been initialized, and if not,
* perform the initialization routine. This routine will also initialze the lwIP
* stack. Note that this function does not use a country code, and will default
* to the worldwide locale. This may not give the best performance in all regions.
*
* @param none
*
* @return true if initialization was successful, otherwise false
*/
bool hw_wifi_init(void);

/**
* @brief Initialize the CYW43 module with country code.
*
* Check weather the CYW43 module has already been initialized, and if not,
* perform the initialization routine using the given country code. This routine
* will also initialze the lwIP stack.
*
* @param country_code country code from the list of definitions in this file
*
* @return true if initialization was successful, otherwise false
*/
bool hw_wifi_init_with_country(hw_wifi_country_t country_code);

/**
* @brief De-initialize the CYW43 module.
*
* This will de-initialize the CYW43 driver and the lwIP stack.
*
* @param none
*
* @return nothing
*/
void hw_wifi_deinit(void);


/*****************************
 * WiFi mode control functions
******************************/

// NOTE: the pico_w will _technically_ support simultaneous AP and STA mode
// connections, but this implementation does not.

/**
* @brief Enable STA mode for the WiFi module.
*
* This function will enable the STA mode for the WiFi module, disabling AP mode
* if it is currently enabled.
*
* @param none
*
* @return nothing
*/
void hw_wifi_enable_sta_mode(void);

/**
* @brief Disable STA mode for the WiFi module.
*
* This function will disable the STA mode for the WiFi module.
*
* @param none
*
* @return nothing
*/
void hw_wifi_disable_sta_mode(void);

/**
* @brief Enable AP mode for the WiFi module.
*
* This function will enable the AP mode for the WiFi module, disabling STA mode
* if it is currently enabled.
*
* @param ssid SSID string for the AP to broadcast
* @param password password string for the AP
* @param auth_type authentication type for the AP
*
* @return nothing
*/
void hw_wifi_enable_ap_mode(const char *ssid, const char *password, hw_wifi_auth_t auth_type);

/**
* @brief Disable AP mode for the WiFi module.
*
* This function will disable the AP mode for the WiFi module.
*
* @param none
*
* @return nothing
*/
void hw_wifi_disable_ap_mode(void);


/**************************************
 * WiFi connection and status functions
***************************************/

/**
* @brief Connect to a WiFi network (blocking).
*
* Connect to a WiFi network with the given SSID, password, and authentication
* type. This function will block until the connection is established.
*
* @param ssid SSID string of the network to connect to
* @param password password string for the network
* @param auth_type authentication type for the network
*
* @return true if the connection was successful, false if connection error detected
*/
bool hw_wifi_connect(const char *ssid, const char *password, hw_wifi_auth_t auth_type);

/**
* @brief Connect to a WiFi network (non-blocking).
*
* Connect to a WiFi network with the given SSID, password, and authentication
* type. This function can be called in a non-blocking fashion.
*
* @param ssid SSID string of the network to connect to
* @param password password string for the network
* @param auth_type authentication type for the network
*
* @return true if the connection was successful, false if connection error detected
*/
bool hw_wifi_connect_async(const char *ssid, const char *password, hw_wifi_auth_t auth_type);

/**
* @brief Get the IP address of the WiFi module.
*
* This function will return the 32-bit integer representation of the IP address
* of the WiFi module, assuming it is connected to a network. The returned address
* will be zero if it has not yet been assigned. The IP address is provided with
* a pointer to a struct with member 'addr' containing the 32-bit address.
*
* @param none
*
* @return pointer to the IP address struct for the WiFi module
*/
const ip_addr_t *hw_wifi_get_addr(void);

/**
* @brief Get the current status of the WiFi connection.
*
* This function will return the current status of the WiFi connection, given by
* the hw_wifi_status_t enum.
*
* @param none
*
* @return enumeration providing the current status of the WiFi connection
*/
hw_wifi_status_t hw_wifi_get_status(void);


#endif /* HW_WIFI_H */