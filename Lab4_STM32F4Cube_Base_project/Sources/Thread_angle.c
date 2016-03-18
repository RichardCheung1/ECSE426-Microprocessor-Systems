/*******************************************************************************
  * @file    Thread_angle.c
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

void Thread_angle (void const *argument);                 // thread function
osThreadId tid_Thread_angle;                              // thread id
osThreadDef(Thread_angle, osPriorityNormal, 1, 0);

/* Variables -----------------------------------------------------------------*/
int display_angle_counter; 

/* Functions -----------------------------------------------------------------*/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

/*----------------------------------------------------------------------------
 *      Create the thread within RTOS context
 *---------------------------------------------------------------------------*/
int start_Thread_angle (void) {

  tid_Thread_angle = osThreadCreate(osThread(Thread_angle), NULL); // Start calculation_Thread
  if (!tid_Thread_angle) return(-1); 
  return(0);
} 

 /*----------------------------------------------------------------------------
*      Thread  'Angle_Thread'
 *---------------------------------------------------------------------------*/
void Thread_angle (void const *argument) {
	//float temperature = 0.0f;
	display_temp = 0.0f;
	display_pitch = 0.0f;
	display_roll = 0.0f;
	display_angle_counter = 0;

	while(1){
		osSignalWait(0x01, osWaitForever);
		osMutexWait(angle_mutex, osWaitForever);
		if (display_mode == 1) {		
			//If the EXTI0 callback function is called and flag is set to active, read accelerometer values
			if(EXTI0_flag_value == INTERRUPT_ACTIVE_FLAG) {
					//Get the accelerometer readings
				get_calibrated_acceleration();
					//Reset the flag
				EXTI0_flag_value = 0;
			}
			
			//Store pitch angle for display
			if ( tilt_selection == 0) {
				display_pitch = pitch;
			}
				
			//Store roll angle for display
			if (tilt_selection == 1) {
				display_roll = roll;
			}		
			
			if(display_angle_counter % 5 == 0) {
				//check_temperature_status(display_temp);
				display = tilt_selection == 0 ?  display_pitch: display_roll;
				display_angle_counter = 0;
			}
		}
		osMutexRelease(angle_mutex);
		//osSignalSet(main_thread_id, 0x01);
	}
}

/**
   * @brief Sets the EXTI0_flag_value flag to 1
   * @param uint16_t GPIO_Pin
   * @retval None
   */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(display_mode == 1)	display_angle_counter++;
	osSignalSet(tid_Thread_angle, 0x01);
	EXTI0_flag_value = 1;
}
