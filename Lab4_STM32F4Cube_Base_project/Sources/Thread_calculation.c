/*******************************************************************************
  * @file    Thread_calculation.c
  * @author  Taha Saifuddin, Richard Cheung
	* @version V1.0.0
  * @date    17-March-2016
  * @brief   This file implements the angle and temp calculation thread, and a  
  *					 function which creates and starts the thread	
  ******************************************************************************
  */

#include "Thread_calculation.h"

/* Variables -----------------------------------------------------------------*/

/* Functions -----------------------------------------------------------------*/

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);





