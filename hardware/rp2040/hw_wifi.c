#include "hw_wifi.h"

#include "pico/cyw43_arch.h"

bool hw_wifi_is_initialized() {
    return cyw43_is_initialized(&cyw43_state);
}

bool hw_wifi_init() {
    if (hw_wifi_is_initialized()) return true;
    return !cyw43_arch_init();
}

bool hw_wifi_init_with_country(hw_wifi_country_t country_code) {
    if (hw_wifi_is_initialized()) return true;
    return !cyw43_arch_init_with_country(country_code);
}

void hw_wifi_deinit() {
    if (hw_wifi_is_initialized()) return;
    cyw43_arch_deinit();
}

// todo : this is locked to lwIP, and we maybe don't want that?
const ip_addr_t *hw_wifi_get_addr() {
    return netif_ip4_addr(netif_list);
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

bool hw_wifi_connect(const char *ssid, const char *password, hw_wifi_auth_t auth_type) {
    uint32_t cw_auth = hw_wifi_auth_to_cyw43(auth_type);

    return !cyw43_arch_wifi_connect_blocking(ssid, password, cw_auth);
}

bool hw_wifi_connect_async(const char *ssid, const char *password, hw_wifi_auth_t auth_type) {
    uint32_t cw_auth = hw_wifi_auth_to_cyw43(auth_type);

    return !cyw43_arch_wifi_connect_async(ssid, password, cw_auth);
}

static hw_wifi_mode_t current_mode = HW_WIFI_MODE_NONE;

// NOTE: the pico_w will _technically_ support simultaneous AP and STA mode
// connections, but this implementation does not.
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
}

void hw_wifi_disable_ap_mode() {
    if (current_mode == HW_WIFI_MODE_AP) {
        cyw43_arch_disable_ap_mode();
        current_mode = HW_WIFI_MODE_NONE;
    }
}

hw_wifi_status_t hw_wifi_get_status() {
    // AP mode always returns LINKDOWN from cyw43
    if (current_mode == HW_WIFI_MODE_AP) {
        return HW_WIFI_STATUS_LINK_DOWN;
    }

    uint32_t status = cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA);
    switch (status) {
        case CYW43_LINK_DOWN:
            return HW_WIFI_STATUS_LINK_DOWN;
        case CYW43_LINK_JOIN:
            return HW_WIFI_STATUS_JOINING;
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
