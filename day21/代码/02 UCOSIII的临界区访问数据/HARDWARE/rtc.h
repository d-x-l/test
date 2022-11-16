//////////////////////////////////////////////////////////////////////////////////	 
//  功能描述   :RTC实时时钟演示例程(STM32F407ZET6系列)
//              说明: 
//              ----------------------------------------------------------------
//              ----------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////
#ifndef _RTC_H
#define _RTC_H


#include "stm32f4xx.h"
#include "systick.h"


static volatile unsigned int RTC_WakeUpFlag = 0; //表示RTC是否唤醒  0 没唤醒  1 唤醒了

RTC_DateTypeDef  RTC_DateStructure;
RTC_TimeTypeDef  RTC_TimeStructure;

//RTC实时时钟的配置
void RTC_Config(void);

#endif

