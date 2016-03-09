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

int column;
int row;
int keypad_state;
char key = ' ';
const int IDLE = 0;
const int DEBOUNCING = 1;
const int PRESSED = 2;
const int RELEASED = 3;
const int delay_offset = 20;
int debouncing_delay = 0;
char keys[4][4] = 
{  
 {'1', '2', '3', 'A'},
 {'4', '5', '6', 'B'},
 {'7', '8', '9', 'C'},
 {'*', '0', '#', 'D'},
};


char get_key() 
{
	column = get_col();
	row = get_row();
	
	debouncer();
	//printf("%c %d%d\n", key, row, column);
	return key;
}

void debouncer()
{
	//printf("%d\n", keypad_state);
	switch(keypad_state){
		case IDLE:
			if(column != -1 && row != -1){
				debouncing_delay = __HAL_TIM_GetCounter(&TIM3_Handle);
				keypad_state = DEBOUNCING;
			}
			break;
		case DEBOUNCING:
			if((debouncing_delay + delay_offset) % 150 == __HAL_TIM_GetCounter(&TIM3_Handle)){
				key = keys[row][column];
				keypad_state = PRESSED;
			}
			break;
		case PRESSED:
			if(column == -1 || row == -1){
				debouncing_delay = __HAL_TIM_GetCounter(&TIM3_Handle);
				keypad_state = RELEASED;
			}
			break;
		case RELEASED:
			if((debouncing_delay + delay_offset) % 150 == __HAL_TIM_GetCounter(&TIM3_Handle)){
				keypad_state = IDLE;
				key = ' ';
			}
			break;
	
	}
}

/* function to determine the column that the key is pressed, where PIN0,PIN1,PIN2,PIN3
are respectively column 1,2,3,4
*/
int get_col() 
{
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
int get_row() 
{
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
	
//		printf ("%d ", HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_12));
//		printf ("%d ", HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_13));
//		printf ("%d ", HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_14));
//		printf ("%d \n ", HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_15));
	
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
