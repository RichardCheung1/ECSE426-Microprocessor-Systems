/**
  ******************************************************************************
  * File Name          : alphanumeric_pad.c
  * Description        : This class provides functionality for configuring and 
												 controlling the alphanumeric keypad
	* Author						 : Richard Cheung, Taha Saifuddin
	* Version            : 2.0.0
	* Date							 : March 15th, 2016
  ******************************************************************************
  */
	
#include "alphanumeric_pad.h"

int column;
int row;
int keypad_state;
int input_flag;
//set to zero(A) for temp mode, one(B) for tilt angle mode, and minus one if no mode is selected
int display_mode = -1;

//zero represents pitch angle, and one represents tilt angle
int tilt_selection = 0;
char key = ' ';
const int IDLE = 0;
const int DEBOUNCING = 1;
const int PRESSED = 2;
const int RELEASED = 3;
char keys[4][4] = 
{  
 {'1', '2', '3', 'A'},
 {'4', '5', '6', 'B'},
 {'7', '8', '9', 'C'},
 {'*', '0', '#', 'D'},
};

/* Functions -----------------------------------------------------------------*/
/**
   * @brief function to set the input value with the keys pressed
	 * @param takes parameter key from get_key in order to do map the right char
   */
void set_input(char key)
{
	//printf("%c\n", key);
	switch (key) 
	{
		//Case for showing the Pitch angle
		case '1':
			if(display_mode == 1) tilt_selection = 0;
			osSignalSet(main_thread_id,0x01);

			break;
		//Case for showing the Roll angle
		case '2':
			if(display_mode == 1) tilt_selection = 1;
			osSignalSet(main_thread_id, 0x01);

			break;
		//Case for Temperature display mode selection
		case 'A':
			display_mode = 0;
			osSignalSet(main_thread_id, 0x01);

			break;
		//Case for Tilt Angle mode selection
		case 'B':
			display_mode = 1;
			osSignalSet(main_thread_id, 0x01);

			break;
		//No other dependencies from keypad
//		default:
//			if(display_mode == 0 || display_mode == 1) display_mode = -1;
//			break;
	}
}

/**
   * @brief function to get the key pressed
	 * @retval key: returns the key associated with the keypad mapping position
   */
char get_key() 
{
	column = get_col();
	row = get_row();
	
	debouncer();
	//printf("%c \n", key);
	return key;
}
/**
   * @brief function to handle debouncer
	 * @param 
   */
void debouncer()
{
	switch(keypad_state){
		case IDLE:
			TIM3_counter = 0;
			if(column != -1 && row != -1){
				keypad_state = DEBOUNCING;
			}
			break;
		case DEBOUNCING:
			TIM3_counter = 0;
			if(TIM3_counter % 1000 == 0){
				key = keys[row][column];
				set_input(key);
				keypad_state = PRESSED;
			}
			break;
		case PRESSED:
			if(column == -1 || row == -1){
				keypad_state = RELEASED;
			}
			break;
		case RELEASED:
			TIM3_counter = 0;
			if(TIM3_counter % 1000 == 0){
				keypad_state = IDLE;
				key = ' ';
			}
			break;
	}
}


/**
   * @brief function to determine the column that the key is pressed, where PIN0,PIN1,PIN2,PIN3
						are respectively column 1,2,3,4
	 * @param 
   */
int get_col() 
{
	//first 4 pins of the alphanumeric set to INPUT
	GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_1 | GPIO_PIN_5 | GPIO_PIN_6;
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
//		printf ("%d ", HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_1));
//		printf ("%d ", HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_3));
//		printf ("%d ", HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_5));
//		printf ("%d \n ", HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_6));
//	
	
	if ( HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_1) == GPIO_PIN_RESET ) {
		return 0;
	}
	else if ( HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_3) == GPIO_PIN_RESET ) {
		return 1;
	}
	else if ( HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_5) == GPIO_PIN_RESET ) {
		return 2;
	}
	else if ( HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_6) == GPIO_PIN_RESET ) {
		return 3;
	}
	else {
		return -1;
	}
}
/**
   * @brief function to determine the row that the key is pressed, where PIN4,PIN5,PIN6,PIN7
						are respectively row 1,2,3,4
	 * @param 
   */
int get_row() 
{
	//first 4 pins of the alphanumeric set to OUTPUT
	GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_1 | GPIO_PIN_5 | GPIO_PIN_6;
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
