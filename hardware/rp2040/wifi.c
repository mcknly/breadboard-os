#include "wifi.h"
#include <pico/stdlib.h>
#include <pico/cyw43_arch.h>
#include "hardware_config.h"
#include "hardware/uart.h"

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
