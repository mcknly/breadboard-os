/******************************************************************************
 * @file hw_net.c
 *
 * @brief Settings, definitions, and function prototypes for various network
 *        stacks, much of it tied to lwIP. Hardware and SDK-specific
 *        implementation resides within these functions.
 *
 * @author Cavin McKinley (MCKNLY LLC)
 *
 * @date 02-06-2025
 *
 * @copyright Copyright (c) 2025 Cavin McKinley (MCKNLY LLC)
 *            Released under the MIT License
 *
 * SPDX-License-Identifier: MIT
 ******************************************************************************/

#include <string.h>
#include "hw_net.h"
#include "pico/cyw43_arch.h"
#include "lwip/apps/httpd.h"


/************************
 * lwIP HTTPD
*************************/

#ifdef ENABLE_HTTPD

// cgi handler function - refer to tCGIHandler typedef in lwIP httpd.h for prototype description
static const char *httpd_cgi_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]) {
    if (!strcmp(pcParam[0], "test")) {
        return "/test.shtml";
    } else {
        return "/index.shtml";
    }
}

// cgi handler path assignments
static tCGI httpd_cgi_paths[] = {
    { "/", httpd_cgi_handler },
    { "/index.shtml", httpd_cgi_handler }
};

// ssi handler function - refer to tSSIHandler typedef in lwIP httpd.h for prototype description
u16_t httpd_ssi_handler(int iIndex, char *pcInsert, int iInsertLen, uint16_t current_tag_part, uint16_t *next_tag_part) {
    size_t printed;
    switch (iIndex) {
        case 0: { /* "status" */
            printed = snprintf(pcInsert, iInsertLen, "Pass");
            break;
        }
        case 1: { /* "welcome" */
            printed = snprintf(pcInsert, iInsertLen, "Hello from Pico");
            break;
        }
        default: { /* unknown tag */
            printed = 0;
            break;
        }
    }
    return (u16_t)printed;
}

// ssi tags, handled by index # in the ssi handler function
// Note that maximum tag length is 8 characters (LWIP_HTTPD_MAX_TAG_NAME_LEN)
static const char * httpd_ssi_tags[] = {
    "status",
    "welcome"
};

void net_httpd_stack_init(void) {
    // set hostname for lwIP
    char hostname[sizeof(CYW43_HOST_NAME)];
    memcpy(&hostname[0], CYW43_HOST_NAME, sizeof(CYW43_HOST_NAME));
    netif_set_hostname(&cyw43_state.netif[CYW43_ITF_STA], hostname);
    
    // initialize lwIP httpd stack
    httpd_init();

    // register cgi and ssi handlers with lwIP
    http_set_cgi_handlers(httpd_cgi_paths, LWIP_ARRAYSIZE(httpd_cgi_paths));
    http_set_ssi_handler(httpd_ssi_handler, httpd_ssi_tags, LWIP_ARRAYSIZE(httpd_ssi_tags));
}

#endif /* ENABLE_HTTPD */