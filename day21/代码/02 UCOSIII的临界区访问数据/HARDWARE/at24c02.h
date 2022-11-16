//////////////////////////////////////////////////////////////////////////////////	 
//  功能描述   : AT24C02芯片演示例程(STM32F407ZET6系列)
//              说明: 
//              ----------------------------------------------------------------
//              SCL   接PB8引脚
//              SDA   接PE9引脚     
//              ----------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////
#ifndef _AT24C02_H
#define _AT24C02_H

#include "stm32f4xx.h"
#include "systick.h"
#include "uart.h"

//AT24C02的IIC总线引脚定义
#define  AT24C02_IIC_SDA_WRITE(n)   (n)?GPIO_SetBits(GPIOB,GPIO_Pin_9):GPIO_ResetBits(GPIOB,GPIO_Pin_9)
#define  AT24C02_IIC_SCL_WRITE(n)   (n)?GPIO_SetBits(GPIOB,GPIO_Pin_8):GPIO_ResetBits(GPIOB,GPIO_Pin_8)

#define  AT24C02_IIC_SDA_READ       GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)


/************AT24C02关于IIC接口函数定义************/
//AT24C02初始化
void AT24C02_IIC_Init(void);

//AT24C02芯片SDA引脚输出模式
void AT24C02_SDAOutputMode(void);

//AT24C02芯片SDA引脚输入模式
void AT24C02_SDAInputMode(void);

//IIC总线开始信号
void AT24C02_IIC_StartSignal(void);

//IIC总线停止信号
void AT24C02_IIC_StopSignal(void);

//主机发送数据（从机读取数据）
void AT24C02_IIC_SendBytes(uint8_t Data);

//主机读取数据（从机发送数据）
uint8_t AT24C02_IIC_ReadBytes(void);

//主机等待从机应答  返回0 说明从机应答   返回1 说明从机没应答
uint8_t AT24C02_IIC_WaitAck(void);

//从机发送数据，主机接收数据并发送应答信号
void  AT24C02_IIC_MasterAck(uint8_t ack);



/************AT24C02关于数据读写函数定义************/

//AT24C02的字节写入  一次写入1字节
void AT24C02_WordWrite(uint8_t Address,uint8_t Data);

//AT24C02页写入      最多写入8字节
void AT24C02_PageWrite(uint8_t Page_Address,uint8_t *buf,uint8_t DataLen);
    
//AT24C02读取1字节数据
uint8_t AT24C02_WordRead(uint8_t Address);

//AT24C02读取n字节数据         存储地址      缓冲数组         数据个数
void AT24C02_RandomRead(uint8_t Address,uint8_t* RecvBuf,uint8_t DataLen);

//AT24C02当前地址读   存储地址指的是AT24C02内部地址计数器中的地址  
uint8_t AT24C02_CurrentAddressRead(void);



#endif
