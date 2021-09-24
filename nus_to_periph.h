#ifndef _NUS_TO_PERIPH_H
#define _NUS_TO_PERIPH_H

#include <stdint.h>
#include "nrf.h"
#include "ble.h"

typedef void (*nus_to_periph_on_data_received_t)(const uint8_t *data_ptr, uint16_t data_length);

uint32_t nus_to_periph_send_data(const uint8_t *data_array, uint16_t length);

void nus_to_periph_ble_evt_handler(ble_evt_t const * p_ble_evt);

void nus_to_periph_init(nus_to_periph_on_data_received_t on_data_received);


#endif