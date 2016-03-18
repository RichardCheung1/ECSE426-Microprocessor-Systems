/**
  ******************************************************************************
  * File Name          : alphanumeric_pad.h
  * Description        : 
	* Author						 : Taha Saifuddin, Richard Cheung
	* Version            : 2.0.0
	* Date							 : March 15th, 2016
  ******************************************************************************
  */

#ifndef __ALPHANUMERIC_PAD_H
#define __ALPHANUMERIC_PAD_H

#include "stm32f4xx_hal.h"
#include "main.h"
#include "timer.h"
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX


/* Exported Variables ------------------------------------------------------- */
extern int input_value, input_flag, tilt_selection, display_mode; 
/* Exported functions ------------------------------------------------------- */
extern char get_key(void) ;
extern void set_input(char key);
/* Private functions -------------------------------------------------------- */
int get_col(void);
int get_row(void);
void debouncer(void);


#endif /* __ALPHANUMERIC_PAD_H */
