//////////////////////////////////////////////////////////////////////////////////	 
//  功能描述  : UART串口演示例程(STM32F407ZET6系列)
//              说明: 
//              ----------------------------------------------------------------
//              USART1_TX  接PA9
//              USART1_RX  接PA10
//              USART3_TX  接PB10
//              USART3_RX  接PB11
//              ----------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////

#ifndef _UART_H
#define _UART_H

#include "stm32f4xx.h"
#include "systick.h"
#include <stdio.h>

static  volatile  char  USART1_RecvBuf[128] = {0}; //保存USART1接收的数据
static  volatile  char  USART3_RecvBuf[128] = {0}; //保存USART3接收的数据

static  volatile  uint32_t  USART1_RecvCnt = 0;  //记录USART1接收的数据个数
static  volatile  uint32_t  USART3_RecvCnt = 0;  //记录USART3接收的数据个数

static  volatile  uint32_t  USARt1_RecvFlag = 0; //USART1的接收标志  1 有效  0 无效
static  volatile  uint32_t  USARt3_RecvFlag = 0; //USART3的接收标志  1 有效  0 无效


void USART1_Init(uint32_t  baud);//串口1的初始化
void USART3_Init(uint32_t  baud);//串口3的初始化


void USART_SendString(USART_TypeDef* USARTx,char *str);//通过串口发送字符串
void USART_SendBytes(USART_TypeDef* USARTx,char *str,uint32_t len);//通过串口发送n个字节


#endif

