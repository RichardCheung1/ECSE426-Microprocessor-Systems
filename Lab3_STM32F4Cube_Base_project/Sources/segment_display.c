/**
  ******************************************************************************
  * File Name          : segment_display.c
  * Description        : This class provides functionality for controlling the
												 7 segment display
	* Author						 : Richard Cheung, Taha Saifuddin
	* Version            : 1.0.0
	* Date							 : February 25th, 2016
  ******************************************************************************
  */
	
#include "segment_display.h"

/**
   * @brief A function used to update the seven segment display with the temp value
	 * @param float f: float containing the temperature value in celcius
   */
//updates the segment display in the format of XY.Z by using the multiplexing
void update_segment_display(float f)
{	
	int decimal_position_one, decimal_position_ten, number;
	int position = 0;
	
	if (f < 10){
		decimal_position_ten =0;
		decimal_position_one =1;
		number = f*100;
	}
	else if (f < 100) {
		decimal_position_ten =1;
		decimal_position_one =0;
		number = f*10;
	}
	else{
		decimal_position_ten =0;
		decimal_position_one =0;
	}
	
	while( position != 3){
		// turn off all selected pins
		clear_select_pin();
		switch (position) 
		{
			// first digit will be set and light while the rest is off
			case 0:
				display_number( number % 10, decimal_position_one);
				HAL_GPIO_WritePin( GPIOC, GPIO_PIN_0, GPIO_PIN_SET) ;
				delay(250);
			break; 
			// first digit will shut off and 2nd digit will light, other digits are still shut off
			case 1:
				display_number( number % 10, decimal_position_ten);
				HAL_GPIO_WritePin( GPIOC, GPIO_PIN_1 , GPIO_PIN_SET) ;			
				delay(250);
				break;
			
			// third digit will light and others are shut off
			case 2: 
				display_number( number % 10, 0);				
				HAL_GPIO_WritePin( GPIOC, GPIO_PIN_2, GPIO_PIN_SET) ;
				delay(250);

			break; 
		}
		clear_select_pin();
		clear_segment_pin();
		HAL_GPIO_WritePin( GPIOC, GPIO_PIN_3, GPIO_PIN_SET) ;
		HAL_GPIO_WritePin( GPIOA, GPIO_PIN_2, GPIO_PIN_SET) ;
		position ++; 
		number /= 10; 
	}
}

void delay(int i){
	int j;
	for (j = 0; j<i ; j++) {}
}

int get_decimal_position(float f) 
{
	int i;
	char c[10];
	sprintf(c, "%f", f);
	for (  i = 0 ; i < 4; i++) {
		if (c[i] == '.') {
			 return i; 
		} 
	}
	return 0; 
}	


// function that takes parameters number and decimal which turns on the appropriate pins to generate the right
// number for the standard 7 segment display. The decimal parameter is a flag to determine if that digit have decimal. 
//
//SEGMENT A - GPIO_PIN_0
//SEGMENT B - GPIO_PIN_1
//SEGMENT C - GPIO_PIN_2
//SEGMENT D - GPIO_PIN_3
//SEGMENT E - GPIO_PIN_4
//SEGMENT F - GPIO_PIN_5
//SEGMENT G - GPIO_PIN_6
//SEGMENT DP - GPIO_PIN_7

/**
   * @brief A function used to display a specific number using the segment pins
	 * @retval none
	 * @param decimal defines whether there is a decimal point or not
	 * @param number is the value of the digit
   */
void display_number(int number, int decimal)
{	
	clear_segment_pin();
	if (decimal) 
	{
		HAL_GPIO_WritePin (GPIOA, GPIO_PIN_7, GPIO_PIN_SET) ;
	}
	switch (number) 
	{
		case 0:
			HAL_GPIO_WritePin (GPIOA, GPIO_PIN_0 |GPIO_PIN_1 |GPIO_PIN_2 |GPIO_PIN_3 |GPIO_PIN_4 | GPIO_PIN_5, GPIO_PIN_SET);
			break;
		case 1:
			HAL_GPIO_WritePin (GPIOA , GPIO_PIN_1 | GPIO_PIN_2, GPIO_PIN_SET); 
			break;
		case 2:
			HAL_GPIO_WritePin (GPIOA , GPIO_PIN_0 | GPIO_PIN_1 |GPIO_PIN_6 | GPIO_PIN_4 | GPIO_PIN_3 , GPIO_PIN_SET); 
			break;
		case 3:
			HAL_GPIO_WritePin (GPIOA , GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_6, GPIO_PIN_SET); 			
			break;
		case 4:
			HAL_GPIO_WritePin (GPIOA , GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_1 | GPIO_PIN_2, GPIO_PIN_SET); 			
			break;
		case 5:
			HAL_GPIO_WritePin (GPIOA ,GPIO_PIN_0 | GPIO_PIN_5 | GPIO_PIN_6| GPIO_PIN_2| GPIO_PIN_3, GPIO_PIN_SET); 			
			break;
		case 6:
			HAL_GPIO_WritePin (GPIOA , GPIO_PIN_0 | GPIO_PIN_5| GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_3 | GPIO_PIN_2, GPIO_PIN_SET); 			
			break;
		case 7:
			HAL_GPIO_WritePin (GPIOA ,GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2, GPIO_PIN_SET); 			
			break;
		case 8:
			HAL_GPIO_WritePin (GPIOA , GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2| GPIO_PIN_3 |GPIO_PIN_4 |GPIO_PIN_5 |GPIO_PIN_6, GPIO_PIN_SET); 			
			break;
		case 9:
			HAL_GPIO_WritePin (GPIOA , GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 |GPIO_PIN_5 |GPIO_PIN_6, GPIO_PIN_SET); 			
			break;
	}
}

/**
   * @brief A function used to set all segments to GPIO_PIN_RESET
	 * @retval none
   */
void clear_segment_pin (void) 
{
	HAL_GPIO_WritePin (GPIOA , GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2| GPIO_PIN_3 |GPIO_PIN_4 |GPIO_PIN_5 |GPIO_PIN_6 |GPIO_PIN_7 , GPIO_PIN_RESET); 			
}

/**
   * @brief A function used to reset the select lines
	 * @retval none
   */
//set all select digit to GPIO_PIN_RESET
void clear_select_pin (void) 
{
	HAL_GPIO_WritePin (GPIOC , GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2| GPIO_PIN_3, GPIO_PIN_RESET); 			
}

//code this later, calling this method will overwrite the corresponding function in stm32f4xx_hal_tim.c
//void HAL_TIM_IRQHandler(TIM_HandleTypeDef *htim)
//{
//	
//}