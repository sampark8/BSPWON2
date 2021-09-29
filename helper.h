
#ifndef _HELPER_H
#define _HELPER_H

#include "bsp.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "app_timer.h"
#include "ble_advertising.h"
#include "bsp_btn_ble.h"

#include "gpio_control.h"

#define DEAD_BEEF                       0xDEADBEEF                                  /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

void timers_init(void);
void log_init(void);
void lfclk_config(void);

#endif