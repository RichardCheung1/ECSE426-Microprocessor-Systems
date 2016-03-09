/**
  ******************************************************************************
  * File Name          : timer.c
  * Description        : This class provides functionality for initializing the
												 timer
	* Author						 : Richard Cheung, Taha Saifuddin
	* Version            : 1.0.0
	* Date							 : March 8th, 2016
  ******************************************************************************
  */

#include "timer.h"
TIM_HandleTypeDef TIM3_Handle;


void TIM_Init() 
{
	__TIM3_CLK_ENABLE();
	//configure TIM3_Handle
	TIM3_Handle.Instance = TIM3;
	TIM3_Handle.Init.Period= 2000;
	TIM3_Handle.Init.Prescaler = 500;
	TIM3_Handle.Init.CounterMode = TIM_COUNTERMODE_UP; 
	TIM3_Handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1; 
	TIM3_Handle.Init.RepetitionCounter = 0;	
	
	HAL_TIM_Base_Init(&TIM3_Handle);
	HAL_TIM_Base_Start_IT(&TIM3_Handle);
	
	
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_0);
	HAL_NVIC_EnableIRQ(TIM3_IRQn);
	HAL_NVIC_SetPriority(TIM3_IRQn, (uint32_t) 1, (uint32_t) 4); 
	
}


