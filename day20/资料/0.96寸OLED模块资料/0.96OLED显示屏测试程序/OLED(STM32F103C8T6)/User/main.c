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
* Author : 小林
* Version: 1.00
* Date   : 2014.4.8
* Email  : hello14blog@gmail.com
* Modification: none
* 
* Description:128*64点阵的OLED显示屏测试文件，仅适用于惠特自动化(heltec.taobao.com)的SD1306驱动IIC通信方式显示屏
*
* Others: none;
*
* Function List:
*	1. int main(void);//主函数
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






//void key_pros()	  //按键处理函数
//{
//	unsigned int i=0;
//	if(Key==1)	  //判断按键k_up是否按下
//	{
//		DelayMs(10); //消抖处理
//		if(Key==1)	 //再次判断按键k_up是否按下
//		{
//				i++;
//				OLED_digit8x16(0,0,i);
//		}
//		while(Key); //等待按键松开
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
		
	OLED_ShowStr(0,3,"HelTec Automation",1);//测试6*8字符
//		key_pros();
//		OLED_digit8x16(0,0,i);
//		DelayS(1);		
//		i++;
//		GPIO_ResetBits(GPIOB,GPIO_Pin_8|GPIO_Pin_9);           //低电平
//	  DelayUs(i);
//		GPIO_SetBits(GPIOB, GPIO_Pin_9|GPIO_Pin_8);         //输出高电平
//		DelayUs(i);
//	

	}
}




/*原本历程*/


//int main(void)
//{
//	unsigned char i=0;
//	extern const unsigned char BMP1[];						//定义了 有个外部变量 且不能被修改的变量。
//	DelayInit();
//	I2C_Configuration();
//	OLED_Init();
//}

//while(1)
//{
//		OLED_Fill(0xFF);//全屏点亮
//		DelayS(2);
//		OLED_Fill(0x00);//全屏灭
//		DelayS(2);
//		for(i=0;i<5;i++)                               //几个字就写几个了
//		{
//			OLED_ShowCN(22+i*16,0,i);           //测试显示中文
//		}
//		DelayS(2);
//		OLED_ShowStr(0,3,"HelTec Automation",1);//测试6*8字符
//		OLED_ShowStr(0,4,"Hello Tech",1);				//测试8*16字符
//		DelayS(1);
//		OLED_CLS();//清屏
//		OLED_OFF();//测试OLED休眠
//		DelayS(1);
//		OLED_ON();//测试OLED休眠后唤醒
//		OLED_DrawBMP(0,0,128,8,(unsigned char *)BMP1);//测试BMP位图显示
//		DelayS(1);
//}
