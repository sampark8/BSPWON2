#ifndef _NUS_TO_CENTRAL_H
#define _NUS_TO_CENTRAL_H

#include <stdint.h>
#include "ble.h"
#include "nrf.h"

typedef void (*nus_to_central_on_data_received_t)(const uint8_t *data_ptr, uint16_t data_length);

uint32_t nus_to_central_send_data(const uint8_t *data_array, uint16_t length);

void nus_to_central_ble_evt_handler(ble_evt_t const * p_ble_evt);

void nus_to_central_init(nus_to_central_on_data_received_t on_data_received);
static unsigned int *bytes_to_send;
#endif

