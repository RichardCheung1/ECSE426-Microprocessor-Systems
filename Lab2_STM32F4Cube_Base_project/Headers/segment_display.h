#ifndef __SEGMENT_DISPLAY_H
#define __SEGMENT_DISPLAY_H

#include "stm32f4xx_hal.h"
#include "main.h"

/* Exported functions ------------------------------------------------------- */
extern void update_segment_display(float f);
extern void clear_segment_pin(void);
extern void clear_select_pin(void); 
extern void LCD_init(void); 

/* Private functions -------------------------------------------------------- */
void display_number(int number, int decimal);
void set_position(int pos);

#endif /* __SEGMENT_DISPLAY_H */
