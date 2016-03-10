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
GPIO_InitTypeDef GPIO_InitStructx;
kalman_state x_kstate;
kalman_state y_kstate;
kalman_state z_kstate;
float acceleration_reading[3];		
float acceleration_filtered[3];
int i;

/* Private function prototypes -----------------------------------------------*/
void print_filtered_acceleration(void);
int Kalmanfilter_C(float measured_acceleration, kalman_state* kstate);

/**
   * @brief A function used to configure and init the accelerometer
	 * @param none
	 * @retval none
   */
void configure_init_accelerometer(void)
{
	//Configure the accelerometer initialization struct
	LIS3DSH_InitStruct.Power_Mode_Output_DataRate = LIS3DSH_DATARATE_100;        /* Ppower down or /active mode with output data rate 3.125 / 6.25 / 12.5 / 25 / 50 / 100 / 400 / 800 / 1600 HZ */
  LIS3DSH_InitStruct.Axes_Enable = LIS3DSH_XYZ_ENABLE;                        /* Axes enable */
  LIS3DSH_InitStruct.Continous_Update = LIS3DSH_ContinousUpdate_Disabled;			/* Block or update Low/High registers of data until all data is read */
	LIS3DSH_InitStruct.AA_Filter_BW = LIS3DSH_AA_BW_50;													/* Choose anti-aliasing filter BW 800 / 400 / 200 / 50 Hz*/
  LIS3DSH_InitStruct.Full_Scale = LIS3DSH_FULLSCALE_2;                        /* Full scale 2 / 4 / 6 / 8 / 16 g */
  LIS3DSH_InitStruct.Self_Test = LIS3DSH_SELFTEST_NORMAL;                     /* Self test */
	
	//Configure the accelerometer interrupt struct
	LIS3DSH_IntConfigStruct.Dataready_Interrupt = LIS3DSH_DATA_READY_INTERRUPT_ENABLED;     /* Enable/Disable data ready interrupt */
  LIS3DSH_IntConfigStruct.Interrupt_signal = LIS3DSH_ACTIVE_HIGH_INTERRUPT_SIGNAL;         /* Interrupt Signal Active Low / Active High */
  LIS3DSH_IntConfigStruct.Interrupt_type = LIS3DSH_INTERRUPT_REQUEST_PULSED;							/* Interrupt type as latched or pulsed */
	
	//Enable the interrupt line
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_0);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
	HAL_NVIC_SetPriority(EXTI0_IRQn, (uint32_t) 0, (uint32_t) 1);
	
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
void configure_interrupt_line(void)
{
	//Configure GPIOE for the interrupt line
	GPIO_InitStructx.Pin = GPIO_PIN_0;
	GPIO_InitStructx.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStructx.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStructx.Pull = GPIO_PULLDOWN; 
	
	HAL_GPIO_Init(GPIOE, &GPIO_InitStructx);
}

/**
   * @brief A function used to initialize the kalman filter struct for all axis
	 * @param	none
	 * @retval none
   */
void Kalmanfilter_init(void) {
	
	//Kalman state struct to filter Ax values
	x_kstate.q = 0.05f; //process noise covariance
	x_kstate.r = 1.0f; //measurement noise covariance
	x_kstate.x = 0.0f; //estimated value
	x_kstate.p = 1.0f; //estimation error covariance
	x_kstate.k = 0.0f; //kalman gain
	
	//Kalman state struct to filter Ay values
	y_kstate.q = 0.1f; //process noise covariance
	y_kstate.r = 1.0f; //measurement noise covariance
	y_kstate.x = 0.0f; //estimated value
	y_kstate.p = 1.0f; //estimation error covariance
	y_kstate.k = 0.0f; //kalman gain
	
	//Kalman state struct to filter Az values
	z_kstate.q = 0.05f; //process noise covariance
	z_kstate.r = 1.0f; //measurement noise covariance
	z_kstate.x = 1.0f; //estimated value
	z_kstate.p = 1.0f; //estimation error covariance
	z_kstate.k = 0.0f; //kalman gain
}

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
   * @brief A function used to read and filter accelerometer readings
	 * @param none
   * @param none
	 * @retval 
   */
void get_filter_acceleration(void)
{	
	//Get the accelerometer readings
	LIS3DSH_ReadACC(acceleration_reading);
	
	//Filter Ax, Ay and Az values
	Kalmanfilter_C(acceleration_reading[0], &x_kstate);
	Kalmanfilter_C(acceleration_reading[1], &y_kstate);
	Kalmanfilter_C(acceleration_reading[2], &z_kstate);
	
	//Save the filtered values
	acceleration_filtered[0] = x_kstate.x;
	acceleration_filtered[1] = y_kstate.x;
	acceleration_filtered[2] = z_kstate.x;
	
	//Print the filtered values
	print_filtered_acceleration();
}

/**
   * @brief A function used to print filtered accelerometer readings
	 * @param none
   * @param none
	 * @retval 
   */
void print_filtered_acceleration(void)
{
	
	//Print the accelerometer readings
	//printf("The accelerometer values are\t");
	for(i = 0; i < 3; i++)
	{
		printf("%f,", acceleration_filtered[i]);
	}
}
