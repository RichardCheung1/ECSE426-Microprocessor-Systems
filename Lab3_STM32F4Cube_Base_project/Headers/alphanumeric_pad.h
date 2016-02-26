/**
  ******************************************************************************
  * File Name          : segment_display.h
  * Description        : Segment display functions that can be used by other classes
	* Author						 : Taha Saifuddin, Richard Cheung
	* Version            : 1.0.0
	* Date							 : February 19th, 2016
  ******************************************************************************
  */

#ifndef __ALPHANUMERIC_PAD_H
#define __ALPHANUMERIC_PAD_H

#include "stm32f4xx_hal.h"
#include "main.h"


/* Exported functions ------------------------------------------------------- */


/* Private functions -------------------------------------------------------- */
int get_col(void);
int get_row(void);

#endif /* __ALPHANUMERIC_PAD_H */
