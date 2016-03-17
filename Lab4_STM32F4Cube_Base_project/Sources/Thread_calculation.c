/*******************************************************************************
  * @file    Thread_calculation.c
  * @author  Taha Saifuddin, Richard Cheung
	* @version V1.0.0
  * @date    17-March-2016
  * @brief   This file implements the angle and temp calculation thread, and a  
  *					 function which creates and starts the thread	
  ******************************************************************************
  */
	
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "stm32f4xx_hal.h"
#include "main.h"

/* Defines -------------------------------------------------------------------*/
#define INTERRUPT_ACTIVE_FLAG 1

/* Variables -----------------------------------------------------------------*/
int EXTI0_flag_value;
int TIM3_flag_value;

void Thread_calculation (void const *argument);                 // thread function
osThreadId tid_Thread_calculation;                              // thread id
osThreadDef(Thread_calculation, osPriorityNormal, 1, 0);

/*----------------------------------------------------------------------------
 *      Create the thread within RTOS context
 *---------------------------------------------------------------------------*/
int start_Thread_calculation (void) {

  tid_Thread_calculation = osThreadCreate(osThread(Thread_calculation), NULL); // Start calculation_Thread
  if (!tid_Thread_calculation) return(-1); 
  return(0);
}

 /*----------------------------------------------------------------------------
*      Thread  'LED_Thread': Toggles LED
 *---------------------------------------------------------------------------*/
void Thread_calculation (void const *argument) {
	while(1){
		//i =0;
		get_key();
		
		//If the EXTI0 callback function is called and flag is set to active, read accelerometer values
		if(EXTI0_flag_value == INTERRUPT_ACTIVE_FLAG) {
			
			//Get the accelerometer readings
			get_calibrated_acceleration();
			
			//Reset the flag
			EXTI0_flag_value = 0;
		}
		
		/*Get the temp values-------------------------------------*/
		
	}
}

/**
   * @brief Sets the TIM3 interrupt flag to 1
   * @param uint16_t GPIO_Pin
   * @retval None
   */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	EXTI0_flag_value = 1;
}

/**
   * @brief Sets the TIM3 interrupt flag to 1
   * @param *htim
   * @retval None
   */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	TIM3_counter ++;
	TIM3_flag_value = 1;
	segment_display_flag = 1;
}
