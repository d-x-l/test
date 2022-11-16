/************************************************************************************
*  Copyright (c), 2014, HelTec Automatic Technology co.,LTD.
*            All rights reserved.
*
* Http:    www.heltec.cn
* Email:   cn.heltec@gmail.com
* WebShop: heltec.taobao.com
*
* File name: main.c
* Project  : HelTec.uvprij
* Processor: STM32F103C8T6
* Compiler : MDK fo ARM
* 
* Author : С��
* Version: 1.00
* Date   : 2014.4.8
* Email  : hello14blog@gmail.com
* Modification: none
* 
* Description:128*64�����OLED��ʾ�������ļ����������ڻ����Զ���(heltec.taobao.com)��SD1306����IICͨ�ŷ�ʽ��ʾ��
*
* Others: none;
*
* Function List:
*	1. int main(void);//������
*
* History: none;
*
*************************************************************************************/
#include "stm32f10x.h"
#include "OLED_I2C.h"
#include "delay.h"
#include "led.h"
#include "systick.h"
#include "key.h"






//void key_pros()	  //����������
//{
//	unsigned int i=0;
//	if(Key==1)	  //�жϰ���k_up�Ƿ���
//	{
//		DelayMs(10); //��������
//		if(Key==1)	 //�ٴ��жϰ���k_up�Ƿ���
//		{
//				i++;
//				OLED_digit8x16(0,0,i);
//		}
//		while(Key); //�ȴ������ɿ�
//	}
//}
int main(void)
{
	DelayInit();	
	I2C_Configuration();
//	key_init();
	OLED_Init();
  OLED_Fill(0xFF);
	DelayUs(1000);
	OLED_Fill(0x00);	
//	LED_GPIO_Config();
	while(1)
	{	
		
	OLED_ShowStr(0,3,"HelTec Automation",1);//����6*8�ַ�
//		key_pros();
//		OLED_digit8x16(0,0,i);
//		DelayS(1);		
//		i++;
//		GPIO_ResetBits(GPIOB,GPIO_Pin_8|GPIO_Pin_9);           //�͵�ƽ
//	  DelayUs(i);
//		GPIO_SetBits(GPIOB, GPIO_Pin_9|GPIO_Pin_8);         //����ߵ�ƽ
//		DelayUs(i);
//	

	}
}




/*ԭ������*/


//int main(void)
//{
//	unsigned char i=0;
//	extern const unsigned char BMP1[];						//������ �и��ⲿ���� �Ҳ��ܱ��޸ĵı�����
//	DelayInit();
//	I2C_Configuration();
//	OLED_Init();
//}

//while(1)
//{
//		OLED_Fill(0xFF);//ȫ������
//		DelayS(2);
//		OLED_Fill(0x00);//ȫ����
//		DelayS(2);
//		for(i=0;i<5;i++)                               //�����־�д������
//		{
//			OLED_ShowCN(22+i*16,0,i);           //������ʾ����
//		}
//		DelayS(2);
//		OLED_ShowStr(0,3,"HelTec Automation",1);//����6*8�ַ�
//		OLED_ShowStr(0,4,"Hello Tech",1);				//����8*16�ַ�
//		DelayS(1);
//		OLED_CLS();//����
//		OLED_OFF();//����OLED����
//		DelayS(1);
//		OLED_ON();//����OLED���ߺ���
//		OLED_DrawBMP(0,0,128,8,(unsigned char *)BMP1);//����BMPλͼ��ʾ
//		DelayS(1);
//}
