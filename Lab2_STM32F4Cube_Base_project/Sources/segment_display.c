#include "stm32f4xx_hal.h"
#include "main.h"
#include "math.h"

void update_segment_display(float f);
void clear_segment_pin (void) ;
void clear_select_pin(void); 
void display_number(int number, int decimal);
void set_position(int pos) ;


//displays in the format of XY.Z
void update_segment_display(float f)
{	
	int number = f*10 ;
	int digit =0;
	int position = 0;

	if ( number < 1000)
	{		
		while (position != 3)
		{
			HAL_GPIO_WritePin( GPIOC, GPIO_PIN_3, GPIO_PIN_RESET); 
			switch (tick_count_gbl%2) 
			{
				case 0:
					display_number( number % 10, 0);
					HAL_GPIO_WritePin( GPIOC, GPIO_PIN_0, GPIO_PIN_SET) ;
					HAL_GPIO_WritePin( GPIOC, GPIO_PIN_1 , GPIO_PIN_RESET);
					HAL_GPIO_WritePin (GPIOC, GPIO_PIN_2 , GPIO_PIN_RESET); 
					position++; 
					number /= 10;
				break; 
				case 1:
					display_number( number % 10, 1);
					HAL_GPIO_WritePin( GPIOC, GPIO_PIN_1 , GPIO_PIN_SET) ;				
					HAL_GPIO_WritePin( GPIOC, GPIO_PIN_2 , GPIO_PIN_RESET);
					HAL_GPIO_WritePin (GPIOC, GPIO_PIN_0 , GPIO_PIN_RESET); 
					position++;
					number /= 10;		
					break;
				case 2: 
					display_number( number % 10, 0);				
					HAL_GPIO_WritePin( GPIOC, GPIO_PIN_2, GPIO_PIN_SET) ;
					HAL_GPIO_WritePin( GPIOC, GPIO_PIN_1 , GPIO_PIN_RESET);
					HAL_GPIO_WritePin (GPIOC, GPIO_PIN_0 , GPIO_PIN_RESET); 	
					position++; 
					number /= 10;
				break; 
			}
		}
	}
}

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


void clear_segment_pin () 
{
	HAL_GPIO_WritePin (GPIOA , GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2| GPIO_PIN_3 |GPIO_PIN_4 |GPIO_PIN_5 |GPIO_PIN_6 |GPIO_PIN_7 , GPIO_PIN_RESET); 			
}

void clear_select_pin () 
{
		HAL_GPIO_WritePin (GPIOC , GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2| GPIO_PIN_3, GPIO_PIN_RESET); 			
}
