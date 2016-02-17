/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : A program which showcases ADC and TIM3 under the new firmware
	                       The ADC
	* Author						 : Ashraf Suyyagh
	* Version            : 1.0.0
	* Date							 : January 14th, 2016
  ******************************************************************************
  */
	
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "supporting_functions.h"
#include "init.c"

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef ADC1_Handle;
ADC_InitTypeDef ADC_InitStruct;
ADC_ChannelConfTypeDef ADC_ChannelStruct; 
GPIO_InitTypeDef GPIO_InitStruct; 

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config	(void);
void set_adc_channel (void); 

int main(void)
{
  /* MCU Configuration----------------------------------------------------------*/
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();
	
  /* Configure the system clock */
  SystemClock_Config();
	
	//set GPIO_InitStruct parameters
	GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15; 
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; 
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FAST; 
	
	//set ADC_InitTypeDef parameters
	ADC_InitStruct.DataAlign = ADC_DATAALIGN_RIGHT;
	ADC_InitStruct.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV4; 
	ADC_InitStruct.Resolution = ADC_RESOLUTION12b; 
	ADC_InitStruct.ContinuousConvMode = DISABLE; 
	ADC_InitStruct.DiscontinuousConvMode = DISABLE; 
	ADC_InitStruct.NbrOfConversion = 1; 
	ADC_InitStruct.ScanConvMode = DISABLE; 
	ADC_InitStruct.DMAContinuousRequests = DISABLE; 
	
	//set ADC_HandleTypeDef parameters
	ADC1_Handle.Instance = ADC1 ;
	
	//Initialize the ADC1
	if (HAL_ADC_Init(&ADC1_Handle) != HAL_OK){
		Error_Handler(ADC_INIT_FAIL);
	};
	//set the channel for the ADC 
	set_adc_channel(); 
	
	//initialize GPIO
	HAL_GPIO_Init( GPIOD, &GPIO_InitStruct); 
	
	while (1){
	}
}



/** System Clock Configuration */
void SystemClock_Config(void){

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  __PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType 	= RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState 			 	= RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState 		= RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource 	= RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM 				= 8;
  RCC_OscInitStruct.PLL.PLLN 				= 336;
  RCC_OscInitStruct.PLL.PLLP 				= RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ 				= 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK){Error_Handler(RCC_CONFIG_FAIL);};

  RCC_ClkInitStruct.ClockType 			= RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource 		= RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider 	= RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider 	= RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider 	= RCC_HCLK_DIV2;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5)!= HAL_OK){Error_Handler(RCC_CONFIG_FAIL);};
	
	/*Configures SysTick to provide 1ms interval interrupts.*/
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

	/* This function sets the source clock for the internal SysTick Timer to be the maximum,
	   in our case, HCLK is now 168MHz*/
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/**
   * @brief A function used to setup the ADC to sample Channel 16
   * @retval None
   */
void set_adc_channel (void) {
	
	//sets ADC_ChannelConfTypeDef parameters
	ADC_ChannelStruct.Channel = ADC_CHANNEL_16;
	ADC_ChannelStruct.SamplingTime = ADC_SAMPLETIME_480CYCLES; 
	ADC_ChannelStruct.Rank = 1;
	
	HAL_ADC_ConfigChannel(&ADC1_Handle, &ADC_ChannelStruct); 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line){
}
#endif
