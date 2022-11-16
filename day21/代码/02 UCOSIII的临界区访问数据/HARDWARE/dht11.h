//////////////////////////////////////////////////////////////////////////////////	 
//  功能描述  : DHT11温湿度模块演示例程(STM32F407ZET6系列)
//              说明: 
//              ----------------------------------------------------------------
//               DQ  接PG9              
//              ----------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////
#ifndef _DHT11_H
#define _DHT11_H

#include "stm32f4xx.h"
#include "delay.h"

#define  DHT_READ	  GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_9)


//DHT11设置引脚输出模式
void DHT_SetOutputMode(void);

//DHT11设置引脚输入模式
void DHT_SetInputMode(void);

//DHT开始信号
void  DHT_Start(void);

//DHT11等待响应 返回1  表示失败   返回0 表示响应
uint32_t DHT_Check(void);

//DHT11读取1bit数据   返回1  表示数字1   返回0  表示数字0
uint32_t DHT_ReadBit(void);

//DHT11读取一个字节数据
uint8_t  DHT_ReadByte(void);

//DHT11获取温湿度数据  返回1  说明失败  返回0  说明成功
uint32_t  DHT_GetData(uint8_t *buf);


#endif
