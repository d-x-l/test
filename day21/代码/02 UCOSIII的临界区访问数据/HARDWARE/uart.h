//////////////////////////////////////////////////////////////////////////////////	 
//  ��������  : UART������ʾ����(STM32F407ZET6ϵ��)
//              ˵��: 
//              ----------------------------------------------------------------
//              USART1_TX  ��PA9
//              USART1_RX  ��PA10
//              USART3_TX  ��PB10
//              USART3_RX  ��PB11
//              ----------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////

#ifndef _UART_H
#define _UART_H

#include "stm32f4xx.h"
#include "systick.h"
#include <stdio.h>

static  volatile  char  USART1_RecvBuf[128] = {0}; //����USART1���յ�����
static  volatile  char  USART3_RecvBuf[128] = {0}; //����USART3���յ�����

static  volatile  uint32_t  USART1_RecvCnt = 0;  //��¼USART1���յ����ݸ���
static  volatile  uint32_t  USART3_RecvCnt = 0;  //��¼USART3���յ����ݸ���

static  volatile  uint32_t  USARt1_RecvFlag = 0; //USART1�Ľ��ձ�־  1 ��Ч  0 ��Ч
static  volatile  uint32_t  USARt3_RecvFlag = 0; //USART3�Ľ��ձ�־  1 ��Ч  0 ��Ч


void USART1_Init(uint32_t  baud);//����1�ĳ�ʼ��
void USART3_Init(uint32_t  baud);//����3�ĳ�ʼ��


void USART_SendString(USART_TypeDef* USARTx,char *str);//ͨ�����ڷ����ַ���
void USART_SendBytes(USART_TypeDef* USARTx,char *str,uint32_t len);//ͨ�����ڷ���n���ֽ�


#endif

