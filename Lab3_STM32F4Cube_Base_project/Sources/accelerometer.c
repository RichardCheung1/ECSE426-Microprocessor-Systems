/**
  ******************************************************************************
  * File Name          : accelerometer.c
  * Description        : This class provides functionality for configuring and 
												 controlling the accelerometer
	* Author						 : Taha Saifuddin, Richard Cheung
	* Version            : 1.0.0
	* Date							 : March 9th, 2016
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "accelerometer.h"

/* Private variables ---------------------------------------------------------*/
LIS3DSH_InitTypeDef LIS3DSH_InitStruct;
LIS3DSH_DRYInterruptConfigTypeDef LIS3DSH_IntConfigStruct;

/* Private function prototypes -----------------------------------------------*/

/**
   * @brief A function used to configure and init the accelerometer
	 * @param none
	 * @retval none
   */
void configure_init_accelerometer(void)
{
	//Configure the accelerometer initialization struct
	LIS3DSH_InitStruct.Power_Mode_Output_DataRate = LIS3DSH_DATARATE_25;        /* Ppower down or /active mode with output data rate 3.125 / 6.25 / 12.5 / 25 / 50 / 100 / 400 / 800 / 1600 HZ */
  LIS3DSH_InitStruct.Axes_Enable = LIS3DSH_XYZ_ENABLE;                        /* Axes enable */
  LIS3DSH_InitStruct.Continous_Update = LIS3DSH_ContinousUpdate_Disabled;			/* Block or update Low/High registers of data until all data is read */
	LIS3DSH_InitStruct.AA_Filter_BW = LIS3DSH_AA_BW_50;													/* Choose anti-aliasing filter BW 800 / 400 / 200 / 50 Hz*/
  LIS3DSH_InitStruct.Full_Scale = LIS3DSH_FULLSCALE_2;                        /* Full scale 2 / 4 / 6 / 8 / 16 g */
  LIS3DSH_InitStruct.Self_Test = LIS3DSH_SELFTEST_NORMAL;                     /* Self test */
	
	//Configure the accelerometer interrupt struct
	LIS3DSH_IntConfigStruct.Dataready_Interrupt = LIS3DSH_DATA_READY_INTERRUPT_ENABLED;     /* Enable/Disable data ready interrupt */
  LIS3DSH_IntConfigStruct.Interrupt_signal = LIS3DSH_ACTIVE_HIGH_INTERRUPT_SIGNAL;         /* Interrupt Signal Active Low / Active High */
  LIS3DSH_IntConfigStruct.Interrupt_type = LIS3DSH_INTERRUPT_REQUEST_PULSED;							/* Interrupt type as latched or pulsed */
	
	//Initialize the accelerometer
	LIS3DSH_Init(&LIS3DSH_InitStruct);
	
	//Setting the interrupt configuration of the accelerometer
	LIS3DSH_DataReadyInterruptConfig(&LIS3DSH_IntConfigStruct);	
}

/**
   * @brief A function used to configure and init the external interrupt line 0
						for use with the accelerometer
	 * @param none
	 * @retval none
   */
void configure_enable_EXTI0_interrupt_line(void)
{
	//Configure GPIOE for the interrupt line
	GPIO_InitStruct.Pin = GPIO_PIN_0;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN; 
	
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	
	//Enable the interrupt line
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_0);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
	HAL_NVIC_SetPriority(EXTI0_IRQn, (uint32_t) 1, (uint32_t) 1); 
}
