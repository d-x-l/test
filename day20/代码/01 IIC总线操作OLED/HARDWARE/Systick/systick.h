#ifndef _SYSTICK_H
#define _SYSTICK_H

#include "stm32f4xx.h"  


void  SysTick_Init(void);
void Delay_us(uint16_t nus);
void Delay_ms(uint16_t nms);



#endif
