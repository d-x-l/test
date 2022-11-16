//////////////////////////////////////////////////////////////////////////////////	 
//  ��������   : AT24C02оƬ��ʾ����(STM32F407ZET6ϵ��)
//              ˵��: 
//              ----------------------------------------------------------------
//              SCL   ��PB8����
//              SDA   ��PE9����     
//              ----------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////
#ifndef _AT24C02_H
#define _AT24C02_H

#include "stm32f4xx.h"
#include "systick.h"
#include "uart.h"

//AT24C02��IIC�������Ŷ���
#define  AT24C02_IIC_SDA_WRITE(n)   (n)?GPIO_SetBits(GPIOB,GPIO_Pin_9):GPIO_ResetBits(GPIOB,GPIO_Pin_9)
#define  AT24C02_IIC_SCL_WRITE(n)   (n)?GPIO_SetBits(GPIOB,GPIO_Pin_8):GPIO_ResetBits(GPIOB,GPIO_Pin_8)

#define  AT24C02_IIC_SDA_READ       GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)


/************AT24C02����IIC�ӿں�������************/
//AT24C02��ʼ��
void AT24C02_IIC_Init(void);

//AT24C02оƬSDA�������ģʽ
void AT24C02_SDAOutputMode(void);

//AT24C02оƬSDA��������ģʽ
void AT24C02_SDAInputMode(void);

//IIC���߿�ʼ�ź�
void AT24C02_IIC_StartSignal(void);

//IIC����ֹͣ�ź�
void AT24C02_IIC_StopSignal(void);

//�����������ݣ��ӻ���ȡ���ݣ�
void AT24C02_IIC_SendBytes(uint8_t Data);

//������ȡ���ݣ��ӻ��������ݣ�
uint8_t AT24C02_IIC_ReadBytes(void);

//�����ȴ��ӻ�Ӧ��  ����0 ˵���ӻ�Ӧ��   ����1 ˵���ӻ�ûӦ��
uint8_t AT24C02_IIC_WaitAck(void);

//�ӻ��������ݣ������������ݲ�����Ӧ���ź�
void  AT24C02_IIC_MasterAck(uint8_t ack);



/************AT24C02�������ݶ�д��������************/

//AT24C02���ֽ�д��  һ��д��1�ֽ�
void AT24C02_WordWrite(uint8_t Address,uint8_t Data);

//AT24C02ҳд��      ���д��8�ֽ�
void AT24C02_PageWrite(uint8_t Page_Address,uint8_t *buf,uint8_t DataLen);
    
//AT24C02��ȡ1�ֽ�����
uint8_t AT24C02_WordRead(uint8_t Address);

//AT24C02��ȡn�ֽ�����         �洢��ַ      ��������         ���ݸ���
void AT24C02_RandomRead(uint8_t Address,uint8_t* RecvBuf,uint8_t DataLen);

//AT24C02��ǰ��ַ��   �洢��ַָ����AT24C02�ڲ���ַ�������еĵ�ַ  
uint8_t AT24C02_CurrentAddressRead(void);



#endif
