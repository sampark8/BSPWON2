
#ifndef TEMPERATURE_H
#define TEMPERATURE_H
#include "nrf_temp.h"
#include "helper.h"
#include "nrf_nvic.h"

void cpu_temp_init();
int cpu_get_temperature(); 

#endif