#ifndef rp2040_WIFI_H
#define rp2040_WIFI_H

#include <stdint.h>
#include <stdbool.h>

#include "pico/cyw43_arch.h"

typedef uint32_t hw_wifi_country_t ;

/*!
 * \brief create a country code from the two character country and revision number
 */
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

typedef enum{
    HW_WIFI_AUTH_OPEN,
    HW_WIFI_AUTH_WPA_TPIK_PSK,
    HW_WIFI_AUTH_WPA2_AES_PSK,
    HW_WIFI_AUTH_MIXED,
} hw_wifi_auth_t ;

typedef enum{
    HW_WIFI_MODE_NONE,
    HW_WIFI_MODE_STA,
    HW_WIFI_MODE_AP,
} hw_wifi_mode_t ;

typedef enum{
    HW_WIFI_STATUS_LINK_DOWN,
    HW_WIFI_STATUS_JOINING,
    HW_WIFI_STATUS_NOIP,
    HW_WIFI_STATUS_UP,
    HW_WIFI_STATUS_FAIL,
    HW_WIFI_STATUS_BADAUTH,
    HW_WIFI_STATUS_NONET,

    HW_WIFI_STATUS_UNKNOWN,
} hw_wifi_status_t ;

bool hw_wifi_is_initialized();
bool hw_wifi_init();
bool hw_wifi_init_with_country(uint32_t country_code);
void hw_wifi_deinit();

// NOTE: the pico_w will _technically_ support simultaneous AP and STA mode
// connections, but this implementation does not.
void hw_wifi_enable_sta_mode();
void hw_wifi_disable_sta_mode();
void hw_wifi_enable_ap_mode(const char *ssid, const char *password, hw_wifi_auth_t auth_type);
void hw_wifi_disable_ap_mode();

const ip_addr_t *hw_wifi_get_addr();
bool hw_wifi_connect(const char *ssid, const char *password, hw_wifi_auth_t auth_type);
bool hw_wifi_connect_async(const char *ssid, const char *password, hw_wifi_auth_t auth_type);
hw_wifi_status_t hw_wifi_get_status();

#endif
