#include "temperature.h"

int32_t temp; 

int cpu_get_temperature()
{
    sd_temp_get(&temp);
    temp = temp/4; 
    printf("TEMPERATURE::%d\r\n", temp);
    return temp;
}
