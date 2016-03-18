/**
  ******************************************************************************
  * File Name          : temperature_sensor.h
  * Description        : Temperature sensor functions that can be used by other classes
	* Author						 : Taha Saifuddin, Richard Cheung
	* Version            : 1.0.0
	* Date							 : February 19th, 2016
  ******************************************************************************
  */
	
#ifndef TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_H

/* Includes ----------------------------------------------------------------- */
#include "main.h"
#include "stm32f4xx_hal.h"

/* Exported types ----------------------------------------------------------- */
extern ADC_HandleTypeDef ADC1_Handle;
extern int is_alarm_on;

/* Exported functions ------------------------------------------------------- */
extern void configure_init_temp_sensor(void);
extern float get_data_from_sensor(void);
extern float convert_voltage_to_celcius(float voltage);
extern void check_temperature_status(float temperature);

#endif /** TEMPERATURE_SENSOR_H */
