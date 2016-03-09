/**
  ******************************************************************************
  * File Name          : accelerometer.h
  * Description        : Accelerometer functions that can be used by other classes
	* Author						 : Taha Saifuddin, Richard Cheung
	* Version            : 1.0.0
	* Date							 : March 9th, 2016
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

/* Define to prevent recursive inclusion -------------------------------------*/
#include "main.h"

/* Exported functions ------------------------------------------------------- */
extern void configure_init_accelerometer(void);
extern void configure_enable_EXTI0_interrupt_line(void);

#endif /** ACCELEROMETER_H */
