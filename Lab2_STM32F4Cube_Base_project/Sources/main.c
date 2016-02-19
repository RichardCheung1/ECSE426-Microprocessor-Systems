/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : A program which showcases ADC and TIM3 under the new firmware
	                       The ADC
	* Author						 : Richard Cheung, Taha Saifuddin
	* Version            : 1.0.0
	* Date							 : February 17th, 2016
  ******************************************************************************
  */
	
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "supporting_functions.h"
#include "main.h"
#include "init.c"
#include "segment_display.h"

/* Constants -----------------------------------------------------------------*/
#define ALARM_PERIOD	500
//#define VOLTAGE_CONVERSION(x)	(float)(x*(3.0/4096))
#define OVERHEAT_TEMP	(float)50
#define ALARM_RESET -1	


/* Structs -------------------------------------------------------------------*/
typedef struct stateInfo{
	float q, r, x, p, k;
}kalman_state;

/* Private variables ---------------------------------------------------------*/
ADC_InitTypeDef ADC_InitStruct;
ADC_ChannelConfTypeDef ADC_ChannelStruct;
GPIO_InitTypeDef GPIO_InitStruct;
GPIO_TypeDef GPIO_struct;

/* Global variables ----_-----------------------------------------------------*/
ADC_HandleTypeDef ADC1_Handle;

int tick_count_gbl;
int ticks = 0; 
int display_ticks; 

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config	(void);
void set_adc_channel (void);

float get_data_from_sensor(void);
float filter_sensor_data (float voltage);
float convert_voltage_to_celcius (float voltage);
void launch_overheat_alarm (int tick_cnt);
void reset_overheat_alarm (void);


int Kalmanfilter_C(float measured_voltage, kalman_state* kstate);

/* Private function prototypes -----------------------------------------------*/
int main(void)
{
	//Temp vars
	float temperature;
	float filteredTemp = 55.0f;
	
	//Temp delay var
	int i = 0;

	//Variable to keep track of alarm status
	int is_alarm_on = 0;
	
	int p,u ;

	//Global 
	tick_count_gbl = 0;
	
  /* MCU Configuration----------------------------------------------------------*/
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();
	
  /* Configure the system clock */
  SystemClock_Config();
	
	//Enable GPIO CLK
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	__HAL_RCC_ADC1_CLK_ENABLE();
	
	/* Configure the GPIO struct for LED */
	GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15; 
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; 
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FAST; 

	
	//initialize GPIO
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
	
	ADC1_Handle.Instance = ADC1 ;

	//set ADC_InitTypeDef parameters
	ADC_InitStruct.DataAlign = ADC_DATAALIGN_RIGHT;
	ADC_InitStruct.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV4; 
	ADC_InitStruct.Resolution = ADC_RESOLUTION_12B; 
	ADC_InitStruct.ContinuousConvMode = ENABLE; 
	ADC_InitStruct.DiscontinuousConvMode = DISABLE; 
	ADC_InitStruct.NbrOfConversion = 1; 
	ADC_InitStruct.ScanConvMode = DISABLE; 
	ADC_InitStruct.DMAContinuousRequests = DISABLE; 
	
	//set ADC_HandleTypeDef parameters
	ADC1_Handle.Init = ADC_InitStruct; 

	//Initialize the ADC1
	if (HAL_ADC_Init(&ADC1_Handle) != HAL_OK){
		Error_Handler(ADC_INIT_FAIL);
	}
	
	//set the channel for the ADC 
	set_adc_channel(); 
	
	/* Setting up polling */
	if (HAL_ADC_Start(&ADC1_Handle) != HAL_OK) {
		Error_Handler(ADC_INIT_FAIL);
	}
	
	//	Configure GPIOC for the 4 select lines
	GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
	GPIO_InitStruct.Pull = GPIO_NOPULL; 
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	//	Configure GPIOA for the segments
	GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
	GPIO_InitStruct.Pull = GPIO_NOPULL; 
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	while (1){

	while(!ticks);
		
		update_segment_display(temperature);

		//Check to see if the conversion to digital value is operational or not
		if(HAL_ADC_PollForConversion(&ADC1_Handle, 1000000) != HAL_OK) {
			Error_Handler(ADC_INIT_FAIL);
		}
		
		//printf("The tick count is %d\n", tick_count_gbl);

		//Resets counter to 0 when needed in keeping with the LED rotation tracking needs
		if(tick_count_gbl >= (ALARM_PERIOD*4)) {
			tick_count_gbl = 0;
		}
		
		//Measures temperature from sensor every 10ms -> 100Hz frequency
		if (tick_count_gbl % 10 == 0) {
			temperature = get_data_from_sensor();
		}
		
		//Outputs temperature to display every 500ms to stabilize the 7seg display
		if(tick_count_gbl % 500 == 0) {
			update_segment_display(temperature); 
		}
		
		//Launches overheating alarm if the temperature is greater than the upper threshold
		if(filteredTemp > OVERHEAT_TEMP) {
			launch_overheat_alarm(tick_count_gbl);
			
			//Toggles the alarm status
			if(!is_alarm_on) {
				is_alarm_on = 1;
			}
			
		//Turn off alarm if temp is back to normal
		} else {
			
			//Turns off the alarm and toggles the alarm status boolean;
			if(is_alarm_on) {
				launch_overheat_alarm(ALARM_RESET);
				is_alarm_on = 0;
			}
			
		}
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
	ADC_ChannelStruct.Offset = 0;
	
	if (HAL_ADC_ConfigChannel(&ADC1_Handle, &ADC_ChannelStruct) != HAL_OK )
	{
		Error_Handler(ADC_CH_CONFIG_FAIL);
	}		
}

/**
   * @brief A function used to dictate the rotation of the LED alarm when the processor overheats
   * @retval None
   */
void launch_overheat_alarm (int tick_cnt) {

	if(tick_cnt == ALARM_RESET) {
		HAL_GPIO_WritePin(&GPIO_struct, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, GPIO_PIN_RESET);
	} else {
		if(tick_cnt >= 0 && tick_cnt < ALARM_PERIOD) {
			//printf("LED 1\n");
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
		} //for(i = 0; i < ALARM_PERIOD; i++);
		
		if(tick_cnt >= ALARM_PERIOD && tick_cnt < (ALARM_PERIOD*2)) {
			//printf("LED 2\n");
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
		} //(i = 0; i < ALARM_PERIOD; i++);
		
		if(tick_cnt >= (ALARM_PERIOD*2) && tick_cnt < (ALARM_PERIOD*3)) {
			//printf("LED 3\n");
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
		}//for(i = 0; i < ALARM_PERIOD; i++);
		
		if(tick_cnt >= (ALARM_PERIOD*3) && tick_cnt < (ALARM_PERIOD*4)) {
			//printf("LED 4\n");
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
		} //for(i = 0; i < ALARM_PERIOD; i++);
	}
}

/**
   * @brief A function used to get the sensor data from ADC
	 * @retval temperature: float containing the temperature value
   */
float get_data_from_sensor (void) {
	
	float voltage;
	float temperature;
	
	//Get the raw data from the internal temperature sensor
	voltage = HAL_ADC_GetValue(&ADC1_Handle) * 1.0f;
	
	//printf("The voltage from sensor is %f\n", voltage);
	
	//Apply the proper scaling to convert to voltage from base 12 ADC equivalent
	voltage = (3.0f * voltage) / 4096.0f;
	
	//Convert the voltage to celcius
	temperature = convert_voltage_to_celcius(voltage);
	
	//Filter the sensor data
	//temperature = filter_sensor_data(temperature);
	
	printf("%f;", temperature);
	return temperature;
}

/**
   * @brief A function used to filter the sensor data from ADC
   * @retval None
   */
float filter_sensor_data (float voltage) {
	float filtered_voltage;
	
	return filtered_voltage;
}

/**
   * @brief A function used to convert the sensor data to temperature in celcius
	 * @retval temperature_celcius: float containing the temperature value in celcius
   */
float convert_voltage_to_celcius (float voltage) {
	float temperature_celcius;
	
	/** Temperature(°C) = ((V_SENSE - V_25) / Avg_Slope) + 25
		* where V_SENSE is voltage read from the internal temp sensor
		*				V_25 is the reference voltage at 25°C -> 0.76V
		*				Avg_Slope is the average slope of temperature vs V_SENSE curve -> 2.5mV/°C
	**/
	temperature_celcius = (((voltage - 0.76f) / 0.0025f) + 25.0f);
	
	return temperature_celcius;
}

//The following method takes an array of measurements and filters the values using the Kalman Filter
int Kalmanfilter_C(float measured_voltage, kalman_state* kstate) {	
	kstate->p = kstate->p + kstate->q;
	kstate->k = kstate->p / (kstate->p + kstate->r);
	kstate->x = kstate->x + kstate->k * (measured_voltage-kstate->x);

	//The following conditional checks if kstate->x is not a number
	if (kstate->x != kstate->x) {
		printf("An error has occured -NaN value was found"); 
		return 1;
	}

	kstate->p = (1-kstate->k) * kstate->p;
	return 0; 
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
