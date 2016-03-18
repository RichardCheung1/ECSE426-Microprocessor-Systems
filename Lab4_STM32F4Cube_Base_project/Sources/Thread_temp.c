/*******************************************************************************
  * @file    Thread_temp.c
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

void Thread_temp (void const *argument);                 // thread function
osThreadId tid_Thread_temp;                              // thread id
osThreadDef(Thread_temp, osPriorityNormal, 1, 0);
int TIM_count_updated;
extern int display_counter; 
/*----------------------------------------------------------------------------
 *      Create the thread within RTOS context
 *---------------------------------------------------------------------------*/
int start_Thread_temp (void) {

  tid_Thread_temp = osThreadCreate(osThread(Thread_temp), NULL); // Start calculation_Thread
  if (!tid_Thread_temp) return(-1); 
  return(0);
}

/*----------------------------------------------------------------------------
*      Thread  'Thread_temp': provide values for temperatures
 *---------------------------------------------------------------------------*/
void Thread_temp (void const *argument) {
	//Variables to store temperature
	float temperature = 0.0f;
	display_temp = 0.0f;
	display_counter = 0;
	
	while(1){
		osSignalWait(0x01, osWaitForever);
		osMutexWait(temp_mutex, osWaitForever);
		if (display_mode == 0) {
			//Start the ADC
			if (HAL_ADC_Start(&ADC1_Handle) != HAL_OK) {
				Error_Handler(ADC_START_FAIL);
			} 
			else {
			//Check for successful conversion of the polled analog value to a digital reading
				if(HAL_ADC_PollForConversion(&ADC1_Handle, 1000000) != HAL_OK) {
					Error_Handler(ADC_POLL_FAIL);
				} 
				else {
					//Measure temperature from sensor 
					if (display_counter % 10 == 0) {
						temperature = get_data_from_sensor();
					}
				}
			__HAL_ADC_CLEAR_FLAG(&ADC1_Handle,ADC_FLAG_EOC);
			}
			//printf("FilteredTemp: %f\n", temperature);
			display_temp = temperature;
			
			//Check temperature status
			if (display_counter % 125 == 0) {
				check_temperature_status(display_temp);
			}
			
			if (display_counter % 250 == 0 ) {
				//check_temperature_status(display_temp);
				display = display_temp;			
			}
			
		}		
		osMutexRelease(temp_mutex);
		//osSignalSet(main_thread_id, 0x01);
		
		TIM_count_updated++;
	}
}

/**
   * @brief Sets the TIM3 interrupt flag to 1
   * @param *htim
   * @retval None
   */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	TIM3_counter ++;
	display_counter++; 
	osSignalSet(tid_Thread_temp, 0x01);
}
