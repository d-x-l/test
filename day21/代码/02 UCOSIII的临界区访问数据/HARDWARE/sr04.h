//////////////////////////////////////////////////////////////////////////////////	 
//  ��������   : HC-SR04������ģ����ʾ����(STM32F407ZET6ϵ��)
//              ˵��: 
//              ----------------------------------------------------------------
//              GND   ��Դ��
//              VCC   ��5V��Դ
//              TRIG  ��PB6����
//              ECHO  ��PE6����     
//              ע�⣺���Ž��߿��Ը����Լ���ʵ��������  ���þ���
//              ----------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////

#ifndef _SR04_H
#define _SR04_H


#include "stm32f4xx.h"
#include "systick.h"


//���������������Ŷ���
#define  TRIG_SET  	  GPIO_SetBits(GPIOB,GPIO_Pin_6)
#define  TRIG_RESET   GPIO_ResetBits(GPIOB,GPIO_Pin_6)
#define  ECHO_READ    GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_6)


//��������������������
void  SR04_Init(void);//��������ʼ�� 
uint32_t SR04_GetDistance(void);//��������ȡ����


#endif
