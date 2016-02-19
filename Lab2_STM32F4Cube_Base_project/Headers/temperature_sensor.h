#ifndef TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_H

#include "main.h"
#include "stm32f4xx_hal.h"

extern float get_data_from_sensor(void);
extern float convert_voltage_to_celcius (float voltage);

#endif /** TEMPERATURE_SENSOR_H */
