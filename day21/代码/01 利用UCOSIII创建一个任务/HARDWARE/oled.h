//////////////////////////////////////////////////////////////////////////////////	 
//  功能描述   : 0.96寸OLED 接口演示例程(STM32F407ZET6系列IIC)
//              说明: 
//              ----------------------------------------------------------------
//              GND   电源地
//              VCC   接5V或3.3v电源
//              SCL   接PB15（SCL）
//              SDA   接PD10（SDA）            
//              ----------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////
#ifndef __OLED_H
#define __OLED_H			  	 
#include "stm32f4xx.h"  
#include "stdlib.h"	    	
#define OLED_MODE 0
#define SIZE 8
#define XLevelL		0x00
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xFF 
#define X_WIDTH 	128
#define Y_WIDTH 	64	    						  
//-----------------OLED IIC端口定义----------------				   

#define  OLED_IIC_SDA_WRITE(n)   (n)?GPIO_SetBits(GPIOD,GPIO_Pin_10):GPIO_ResetBits(GPIOD,GPIO_Pin_10)
#define  OLED_IIC_SCL_WRITE(n)   (n)?GPIO_SetBits(GPIOB,GPIO_Pin_15):GPIO_ResetBits(GPIOB,GPIO_Pin_15)

#define  OLED_IIC_SDA_READ       GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_10)

 		     
#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据


//OLED控制函数
void OLED_WR_Byte(unsigned dat,unsigned cmd);  
void OLED_Display_On(void);
void OLED_Display_Off(void);	   							   		    
void OLED_Init(void);
void OLED_Clear(void);
void OLED_DrawPoint(u8 x,u8 y,u8 t);
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot);
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 Char_Size);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size);
void OLED_ShowString(u8 x,u8 y, u8 *p,u8 Char_Size);	 
void OLED_Set_Pos(unsigned char x, unsigned char y);
void OLED_ShowCHinese(u8 x,u8 y,u8 no);
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[]);
void fill_picture(unsigned char fill_Data);
void Picture(void);

//OLED关于IIC总线通讯函数
void OELD_IIC_Start(void);
void OLED_IIC_Stop(void);
void OLED_IIC_SDAOutputMode(void);
void OLED_IIC_SDAInputMode(void);
void OLED_Write_IIC_Byte(unsigned char IIC_Byte);
void OLED_Write_IIC_Command(unsigned char IIC_Command);
void OLED_Write_IIC_Data(unsigned char IIC_Data);


uint8_t OLED_IIC_WaitAck(void);
#endif  
	 



