/******************************************************************************
 * @file hw_wifi.c
 *
 * @brief Function implementations for WiFi connectivity using the CYW43
 *        wireless module. The implementation of these functions is hardware and
 *        SDK-specific and will need to be changed if ported to a new device.
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

#include "hw_wifi.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"


static hw_wifi_mode_t current_mode = HW_WIFI_MODE_NONE;

void hw_wifi_hard_reset(void) {
    // toggle CYW43 WL_ON GPIO to make sure we are starting from POR
    gpio_init(23);
    gpio_set_dir(23, GPIO_OUT);
    gpio_put(23, 0); // set WL_ON low to hold in reset
    busy_wait_us(1000);
    gpio_put(23,1); // release reset
    // note: per the CYW43 data sheet, "wait at least 150ms after VDDC and
    // VDDIO are available before initiating SDIO access". Should we have
    // a hard pause here?
}

bool hw_wifi_is_initialized() {
    return cyw43_is_initialized(&cyw43_state);
}

bool hw_wifi_init() {
    if (hw_wifi_is_initialized()) {
        return true;
    }
    else {
        // force POR on wifi module if we are unsure of state
        // hw_wifi_hard_reset();

        // initialize the wifi module
        return !cyw43_arch_init();
    }
}

bool hw_wifi_init_with_country(hw_wifi_country_t country_code) {
    if (hw_wifi_is_initialized()) {
        return true;
    }
    else {
        // force POR on wifi module if we are unsure of state
        // hw_wifi_hard_reset();

        // initialize the wifi module
        return !cyw43_arch_init_with_country(country_code);
    }
}

void hw_wifi_deinit() {
    if (hw_wifi_is_initialized()) return;
    cyw43_arch_deinit();
}

static uint32_t hw_wifi_auth_to_cyw43(hw_wifi_auth_t auth) {
    switch (auth) {
        case HW_WIFI_AUTH_MIXED:
            return CYW43_AUTH_WPA2_MIXED_PSK;
        case HW_WIFI_AUTH_WPA_TPIK_PSK:
            return CYW43_AUTH_WPA_TKIP_PSK;
        case HW_WIFI_AUTH_WPA2_AES_PSK:
            return CYW43_AUTH_WPA2_AES_PSK;
        default:
            return CYW43_AUTH_OPEN;
    }
}

void hw_wifi_enable_sta_mode() {
    hw_wifi_disable_ap_mode();
    cyw43_arch_enable_sta_mode();
    current_mode = HW_WIFI_MODE_STA;
}

void hw_wifi_disable_sta_mode() {
    if (current_mode == HW_WIFI_MODE_STA) {
        cyw43_arch_disable_sta_mode();
        current_mode = HW_WIFI_MODE_NONE;
    }
}

void hw_wifi_enable_ap_mode(const char *ssid, const char *password, hw_wifi_auth_t auth_type) {
    hw_wifi_disable_sta_mode();
    cyw43_arch_enable_ap_mode(ssid, password, hw_wifi_auth_to_cyw43(auth_type));
    current_mode = HW_WIFI_MODE_AP;
}

void hw_wifi_disable_ap_mode() {
    if (current_mode == HW_WIFI_MODE_AP) {
        cyw43_arch_disable_ap_mode();
        current_mode = HW_WIFI_MODE_NONE;
    }
}

bool hw_wifi_connect(const char *ssid, const char *password, hw_wifi_auth_t auth_type) {
    uint32_t cw_auth = hw_wifi_auth_to_cyw43(auth_type);

    return !cyw43_arch_wifi_connect_blocking(ssid, password, cw_auth);
}

bool hw_wifi_connect_async(const char *ssid, const char *password, hw_wifi_auth_t auth_type) {
    uint32_t cw_auth = hw_wifi_auth_to_cyw43(auth_type);

    return !cyw43_arch_wifi_connect_async(ssid, password, cw_auth);
}

void hw_wifi_reset_connection(void) {
    // disable and re-enable AP mode to reset connection
    hw_wifi_disable_ap_mode();
    hw_wifi_disable_sta_mode();
    cyw43_arch_enable_sta_mode();
    current_mode = HW_WIFI_MODE_STA;
}

const ip_addr_t *hw_wifi_get_addr() {
    // query lwIP for address
    return netif_ip4_addr(netif_list);
}

hw_wifi_status_t hw_wifi_get_status() {
    // AP mode always returns LINKDOWN from cyw43
    if (current_mode == HW_WIFI_MODE_AP) {
        return HW_WIFI_STATUS_LINK_DOWN;
    }

    // TODO: the cyw43_arch_wifi_link_status function will not actually return
    // all the statuses below. In fact, it appears that it always returns
    // CYW43_LINK_JOIN. This function is somewhat useless for now, it may be
    // that the pico-sdk implementation is flawed, needs more research.
    uint32_t status = cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA);
    switch (status) {
        case CYW43_LINK_DOWN:
            return HW_WIFI_STATUS_LINK_DOWN;
        case CYW43_LINK_JOIN:
            return HW_WIFI_STATUS_JOINED;
        case CYW43_LINK_NOIP:
            return HW_WIFI_STATUS_NOIP;
        case CYW43_LINK_UP:
            return HW_WIFI_STATUS_UP;
        case CYW43_LINK_FAIL:
            return HW_WIFI_STATUS_FAIL;
        case CYW43_LINK_NONET:
            return HW_WIFI_STATUS_NONET;
        case CYW43_LINK_BADAUTH:
            return HW_WIFI_STATUS_BADAUTH;
        default:
            return HW_WIFI_STATUS_UNKNOWN;
    }
}
