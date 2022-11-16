//////////////////////////////////////////////////////////////////////////////////	 
//  ��������  : DHT11��ʪ��ģ����ʾ����(STM32F407ZET6ϵ��)
//              ˵��: 
//              ----------------------------------------------------------------
//               DQ  ��PG9              
//              ----------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////
#ifndef _DHT11_H
#define _DHT11_H

#include "stm32f4xx.h"
#include "delay.h"

#define  DHT_READ	  GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_9)


//DHT11�����������ģʽ
void DHT_SetOutputMode(void);

//DHT11������������ģʽ
void DHT_SetInputMode(void);

//DHT��ʼ�ź�
void  DHT_Start(void);

//DHT11�ȴ���Ӧ ����1  ��ʾʧ��   ����0 ��ʾ��Ӧ
uint32_t DHT_Check(void);

//DHT11��ȡ1bit����   ����1  ��ʾ����1   ����0  ��ʾ����0
uint32_t DHT_ReadBit(void);

//DHT11��ȡһ���ֽ�����
uint8_t  DHT_ReadByte(void);

//DHT11��ȡ��ʪ������  ����1  ˵��ʧ��  ����0  ˵���ɹ�
uint32_t  DHT_GetData(uint8_t *buf);


#endif
