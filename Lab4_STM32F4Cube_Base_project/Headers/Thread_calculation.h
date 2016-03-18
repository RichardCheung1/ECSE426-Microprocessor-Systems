/*******************************************************************************
  * @file    Thread_calculation.h
  * @author  Taha Saifuddin, Richard Cheung
	* @version V1.0.0
  * @date    17-March-2016
  * @brief   This file declares the variables and functions implemented in the angle  
  *					 and temp calculation thread	
  ******************************************************************************
  */
	
#ifndef THREAD_CALCULATION_H
#define THREAD_CALCULATION_H

/* Includes ----------------------------------------------------------------- */
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "stm32f4xx_hal.h"
#include "main.h"

/* Defines -------------------------------------------------------------------*/
#define INTERRUPT_ACTIVE_FLAG 1

/* Exported types ----------------------------------------------------------- */
extern float display_temp, display_pitch, display_roll;

/* Exported functions ------------------------------------------------------- */
extern int start_Thread_calculation (void);

/* Private functions -------------------------------------------------------- */
//int Kalmanfilter_C(float measured_temp, kalman_state* kstate);
//void Kalmanfilter_init(void);

#endif /** THREAD_CALCULATION_H */
