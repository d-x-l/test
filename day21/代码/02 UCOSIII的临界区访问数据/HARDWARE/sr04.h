//////////////////////////////////////////////////////////////////////////////////	 
//  功能描述   : HC-SR04超声波模块演示例程(STM32F407ZET6系列)
//              说明: 
//              ----------------------------------------------------------------
//              GND   电源地
//              VCC   接5V电源
//              TRIG  接PB6引脚
//              ECHO  接PE6引脚     
//              注意：引脚接线可以根据自己的实际需求来  不用纠结
//              ----------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////

#ifndef _SR04_H
#define _SR04_H


#include "stm32f4xx.h"
#include "systick.h"


//超声波传感器引脚定义
#define  TRIG_SET  	  GPIO_SetBits(GPIOB,GPIO_Pin_6)
#define  TRIG_RESET   GPIO_ResetBits(GPIOB,GPIO_Pin_6)
#define  ECHO_READ    GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_6)


//超声波传感器函数定义
void  SR04_Init(void);//超声波初始化 
uint32_t SR04_GetDistance(void);//超声波获取距离


#endif
