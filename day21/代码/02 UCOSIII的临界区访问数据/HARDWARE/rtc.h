//////////////////////////////////////////////////////////////////////////////////	 
//  ��������   :RTCʵʱʱ����ʾ����(STM32F407ZET6ϵ��)
//              ˵��: 
//              ----------------------------------------------------------------
//              ----------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////
#ifndef _RTC_H
#define _RTC_H


#include "stm32f4xx.h"
#include "systick.h"


static volatile unsigned int RTC_WakeUpFlag = 0; //��ʾRTC�Ƿ���  0 û����  1 ������

RTC_DateTypeDef  RTC_DateStructure;
RTC_TimeTypeDef  RTC_TimeStructure;

//RTCʵʱʱ�ӵ�����
void RTC_Config(void);

#endif

