/**
  ******************************************************************************
  * File Name          : alphanumeric_pad.c
  * Description        : 
	* Author						 : Richard Cheung, Taha Saifuddin
	* Version            : 1.0.0
	* Date							 : February 26th, 2016
  ******************************************************************************
  */
	
#include "alphanumeric_pad.h"

/* function to determine the column that the key is pressed, where PIN0,PIN1,PIN2,PIN3
are respectively column 1,2,3,4
*/
int get_col(void) {
	//first 4 pins of the alphanumeric set to INPUT
	GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
	GPIO_InitStruct.Pull = GPIO_PULLUP; 
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
	
	//last 4 pins of the alphanumeric set to OUTPUT
	GPIO_InitStruct.Pin = GPIO_PIN_15 | GPIO_PIN_14 | GPIO_PIN_13 | GPIO_PIN_12;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
	GPIO_InitStruct.Pull = GPIO_NOPULL; 
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	
	if ( HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_0) == GPIO_PIN_RESET ) {
		return 0;
	}
	else if ( HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_1) == GPIO_PIN_RESET ) {
		return 1;
	}
	else if ( HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_2) == GPIO_PIN_RESET ) {
		return 2;
	}
	else if ( HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_3) == GPIO_PIN_RESET ) {
		return 3;
	}
	else {
		return -1;
	}
}

/* function to determine the row that the key is pressed, where PIN4,PIN5,PIN6,PIN7
are respectively row 1,2,3,4
*/
int get_row(void) {
	//first 4 pins of the alphanumeric set to OUTPUT
	GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
	GPIO_InitStruct.Pull = GPIO_NOPULL; 
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
	
	//last 4 pins of the alphanumeric set to INPUT
	GPIO_InitStruct.Pin = GPIO_PIN_15 | GPIO_PIN_14 | GPIO_PIN_13 | GPIO_PIN_12;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
	GPIO_InitStruct.Pull = GPIO_PULLUP; 
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

	if ( HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_12) == GPIO_PIN_RESET ) {
		return 0;
	}
	else if ( HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_13) == GPIO_PIN_RESET ) {
		return 1;
	}
	else if ( HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_14) == GPIO_PIN_RESET ) {
		return 2;
	}
	else if ( HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_15) == GPIO_PIN_RESET ) {
		return 3;
	}
	else {
		return -1;
	}
}
