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
int EXTI0_flag_value;
int TIM3_flag_value;
int loop_count_gbl;
float display_temp, display_pitch, display_roll;

/* Functions -----------------------------------------------------------------*/
void Thread_calculation (void const *argument);                 // thread function
int start_Thread_calculation (void);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

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
	
	//Variables to store temperature
	float temperature = 0.0f;
	display_temp = 0.0f;
	display_pitch = 0.0f;
	display_roll = 0.0f;
	
	//Variable to keep track of alarm status
	//int is_alarm_on = 0;
 
	loop_count_gbl = 0;
	
	while(1){
		
		//Start the ADC
		if (HAL_ADC_Start(&ADC1_Handle) != HAL_OK) {
			Error_Handler(ADC_START_FAIL);
			
		} else {
			
			//Check for successful conversion of the polled analog value to a digital reading
			if(HAL_ADC_PollForConversion(&ADC1_Handle, 1000000) != HAL_OK) {
				Error_Handler(ADC_POLL_FAIL);
				
			} else {
				
				//Measure temperature from sensor every 10ms -> 100Hz frequency
				if (loop_count_gbl % 10 == 0) {
					temperature = get_data_from_sensor();
				}
				
			}
			
			__HAL_ADC_CLEAR_FLAG(&ADC1_Handle,ADC_FLAG_EOC);
		}
		
		//--------RICHARD LOOK HERE--------------------
		/*Check if the keypad has any input - do this in the keypad thread maybe?*/
		//get_key();
		//---------------------------------------------
		
		//If the EXTI0 callback function is called and flag is set to active, read accelerometer values
		if(EXTI0_flag_value == INTERRUPT_ACTIVE_FLAG) {
			
			//Get the accelerometer readings
			get_calibrated_acceleration();
			
			//Reset the flag
			EXTI0_flag_value = 0;
		}
		
		//Capture temperature and angle values to display every 250ms
		if(loop_count_gbl % 250 == 0) {
			
			printf("FilteredTemp: %f\n", temperature);
			display_temp = temperature;
			display_pitch = pitch;
			display_roll = roll;
			
			
		}
		
		loop_count_gbl++;
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
