/**
  ******************************************************************************
  * File Name          : segment_display.c
  * Description        : This class provides functionality for controlling the
												 7 segment display
	* Author						 : Richard Cheung, Taha Saifuddin
	* Version            : 1.0.0
	* Date							 : February 19th, 2016
  ******************************************************************************
  */
	
#include "segment_display.h"

//READ & WRITE_
//GPIO_PIN_0 
//
//Register select
//GPIO_PIN_1 
//
//GPIO_PIN_2  
//Enable
//
//DATA_PINS:
// GPIO_PIN_7 | GPIO_PIN_8  | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14
// data 0							1								2						3							4							5								6					7
void LCD_init(void) 
{
	//set  E & RS bit to LOW
	HAL_GPIO_WritePin( GPIOE, GPIO_PIN_2| GPIO_PIN_1, GPIO_PIN_RESET); 
	
	//display all + blick and underline
	HAL_GPIO_WritePin (GPIOE, GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14, GPIO_PIN_RESET);
	HAL_GPIO_WritePin (GPIOE, GPIO_PIN_7 | GPIO_PIN_8  |GPIO_PIN_9 |GPIO_PIN_10 , GPIO_PIN_SET);


}


//updates the segment display in the format of XY.Z by using the multiplexing
void update_segment_display(float f)
{	
	int number = f*10 ;
	int position = 0;
	if ( number < 1000)
	{		
		while( position != 3){
			// turn off all selected pins
			clear_select_pin();
			switch (position) 
			{
				// first digit will be set and light while the rest is off
				case 0:
					display_number( number % 10, 0);
					HAL_GPIO_WritePin( GPIOC, GPIO_PIN_0, GPIO_PIN_SET) ;
					HAL_GPIO_WritePin( GPIOC, GPIO_PIN_1 , GPIO_PIN_RESET);
					HAL_GPIO_WritePin( GPIOC, GPIO_PIN_2 , GPIO_PIN_RESET);
					HAL_GPIO_WritePin( GPIOC, GPIO_PIN_3 , GPIO_PIN_RESET);
				break; 
				// first digit will shut off and 2nd digit will light, other digits are still shut off
				case 1:
					HAL_GPIO_WritePin (GPIOC, GPIO_PIN_0 , GPIO_PIN_RESET); 
					display_number( number % 10, 1);
					HAL_GPIO_WritePin( GPIOC, GPIO_PIN_1 , GPIO_PIN_SET) ;				
					HAL_GPIO_WritePin( GPIOC, GPIO_PIN_2 , GPIO_PIN_RESET);
					HAL_GPIO_WritePin( GPIOC, GPIO_PIN_3 , GPIO_PIN_RESET);
					break;
				
				// third digit will light and others are shut off
				case 2: 
					HAL_GPIO_WritePin( GPIOC, GPIO_PIN_1 , GPIO_PIN_RESET);
					display_number( number % 10, 0);				
					HAL_GPIO_WritePin( GPIOC, GPIO_PIN_2, GPIO_PIN_SET) ;
					HAL_GPIO_WritePin (GPIOC, GPIO_PIN_0 , GPIO_PIN_RESET);
					HAL_GPIO_WritePin( GPIOC, GPIO_PIN_3 , GPIO_PIN_RESET);

				break; 
			}
			position ++; 
			number /= 10; 
		}
	}
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
//SEGMENT DT - GPIO_PIN_7
//
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

//set all segments to GPIO_PIN_RESET
void clear_segment_pin () 
{
	HAL_GPIO_WritePin (GPIOA , GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2| GPIO_PIN_3 |GPIO_PIN_4 |GPIO_PIN_5 |GPIO_PIN_6 |GPIO_PIN_7 , GPIO_PIN_RESET); 			
}

//set all select digit to GPIO_PIN_RESET
void clear_select_pin () 
{
	HAL_GPIO_WritePin (GPIOC , GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2| GPIO_PIN_3, GPIO_PIN_RESET); 			
}
