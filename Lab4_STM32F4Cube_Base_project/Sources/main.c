/*******************************************************************************
  * @file    main.c
  * @author  Ashraf Suyyagh
	* @version V1.2.0
  * @date    17-January-2016
  * @brief   This file demonstrates flasing one LED at an interval of one second
	*          RTX based using CMSIS-RTOS 
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"              // Keil::Device:STM32Cube HAL:Common
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "RTE_Components.h"             // Component selection
#include "main.h"

/* Variables -----------------------------------------------------------------*/
GPIO_InitTypeDef GPIO_InitStruct;
kalman_state x_kstate, y_kstate, z_kstate, temp_kstate;
extern void initializeLED_IO			(void);
//extern void start_Thread_LED			(void);
//extern void Thread_LED(void const *argument);
extern void start_Thread_LED			(void);
extern void Thread_LED(void const *argument);
extern osThreadId tid_Thread_LED;
int TIM3_counter;

/**
	These lines are mandatory to make CMSIS-RTOS RTX work with te new Cube HAL
*/
#ifdef RTE_CMSIS_RTOS_RTX
extern uint32_t os_time;

uint32_t HAL_GetTick(void) { 
  return os_time; 
}
#endif

/**
  * System Clock Configuration
  */
void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the
     device is clocked below the maximum system frequency (see datasheet). */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 |
                                RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
}

/**
  * Main function
  */
//int main (void) {

//  osKernelInitialize();                     /* initialize CMSIS-RTOS          */

//  HAL_Init();                               /* Initialize the HAL Library     */

//  SystemClock_Config();                     /* Configure the System Clock     */

//	/* User codes goes here*/
//  initializeLED_IO();                       /* Initialize LED GPIO Buttons    */
//  start_Thread_LED();                       /* Create LED thread              */
//	/* User codes ends here*/
//  
//	osKernelStart();                          /* start thread execution         */
//}

//---------------------------------------------------------------
int main(void)
 {	
	//int i; 
  /* MCU Configuration----------------------------------------------------------*/
  
	HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();
	
  /* Initialize all configured peripherals */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	
	 //Enable the ADC clock
	__HAL_RCC_ADC1_CLK_ENABLE();
	 
	//Initialize the Accelerometer and the external interrup line 0
	configure_init_accelerometer();
	configure_interrupt_line();
	
	//Configure and initialize the ADC
	configure_init_temp_sensor();
	
	//Initialize the 4 digit 7segment Display
	segment_display_init();
	
	//configure tim3
	TIM_Init(); 
	
	//Initialize the kalman filter structs
	Kalmanfilter_init();
}
//------------------------------------------------------------------------------
/**
   * @brief A function used to filter the noisy acceleration readings
	 * @param 
   * @param kstate: pointer to the kalman filter configuration struct
	 * @retval 
   */
int Kalmanfilter_C(float measured_acceleration, kalman_state* kstate) {	
	kstate->p = kstate->p + kstate->q;
	kstate->k = kstate->p / (kstate->p + kstate->r);
	kstate->x = kstate->x + kstate->k * (measured_acceleration-kstate->x);

	//The following conditional checks if kstate->x is not a number
	if (kstate->x != kstate->x) {
		printf("An error has occured -NaN value was found"); 
		return 1;
	}
	
	kstate->p = (1-kstate->k) * kstate->p;
	return 0; 
}

/**
   * @brief A function used to initialize the kalman filter struct for all kalman state objects
	 * @param	none
	 * @retval none
   */
void Kalmanfilter_init(void) {
	
	//Kalman state struct to filter Ax values
	x_kstate.q = 0.10f; //process noise covariance
	x_kstate.r = 2.00f; //measurement noise covariance
	x_kstate.x = 0.0f; //estimated value
	x_kstate.p = 2.0f; //estimation error covariance
	x_kstate.k = 0.0f; //kalman gain
	
	//Kalman state struct to filter Ay values
	y_kstate.q = 0.10f; //process noise covariance
	y_kstate.r = 2.00f; //measurement noise covariance
	y_kstate.x = 0.0f; //estimated value
	y_kstate.p = 2.0f; //estimation error covariance
	y_kstate.k = 0.0f; //kalman gain
	
	//Kalman state struct to filter Az values
	z_kstate.q = 0.10f; //process noise covariance
	z_kstate.r = 2.00f; //measurement noise covariance
	z_kstate.x = 1000.0f; //estimated value
	z_kstate.p = 2.0f; //estimation error covariance
	z_kstate.k = 0.0f; //kalman gain
	
	//Kalman state struct to filter Temp values
	temp_kstate.q = 0.02f; //process noise covariance
	temp_kstate.r = 0.5f; //measurement noise covariance
	temp_kstate.x = 28.0f; //estimated value
	temp_kstate.p = 2.0f; //estimation error covariance
	temp_kstate.k = 0.00f; //kalman gain
}
