#ifndef __key_H
#define __key_H 			   
#include "stm32f10x.h"
#define key GPIO_Pin_9
#define Key GPIO_ReadInputDataBit(GPIOA,key)	
void key_init();	   

#endif

