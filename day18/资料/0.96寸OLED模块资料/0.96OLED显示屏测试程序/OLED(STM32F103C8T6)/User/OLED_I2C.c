/************************************************************************************
*  Copyright (c), 2014, HelTec Automatic Technology co.,LTD.
*            All rights reserved.
*
* Http:    www.heltec.cn
* Email:   cn.heltec@gmail.com
* WebShop: heltec.taobao.com
*
* File name: OLED_I2C.c
* Project  : HelTec.uvprij
* Processor: STM32F103C8T6
* Compiler : MDK fo ARM
* 
* Author : Ð¡ÁÖ
* Version: 1.00
* Date   : 2014.4.8
* Email  : hello14blog@gmail.com
* Modification: none
* 
* Description:128*64µãÕóµÄOLEDÏÔÊ¾ÆÁÇý¶¯ÎÄ¼þ£¬½öÊÊÓÃÓÚ»ÝÌØ×Ô¶¯»¯(heltec.taobao.com)µÄSD1306Çý¶¯IICÍ¨ÐÅ·½Ê½ÏÔÊ¾ÆÁ
*
* Others: none;
*
* Function List:
*	1. void I2C_Configuration(void) -- ÅäÖÃCPUµÄÓ²¼þI2C
* 2. void I2C_WriteByte(uint8_t addr,uint8_t data) -- Ïò¼Ä´æÆ÷µØÖ·Ð´Ò»¸öbyteµÄÊý¾Ý
* 3. void WriteCmd(unsigned char I2C_Command) -- Ð´ÃüÁî
* 4. void WriteDat(unsigned char I2C_Data) -- Ð´Êý¾Ý
* 5. void OLED_Init(void) -- OLEDÆÁ³õÊ¼»¯
* 6. void OLED_SetPos(unsigned char x, unsigned char y) -- ÉèÖÃÆðÊ¼µã×ø±ê
* 7. void OLED_Fill(unsigned char fill_Data) -- È«ÆÁÌî³ä
* 8. void OLED_CLS(void) -- ÇåÆÁ
* 9. void OLED_ON(void) -- »½ÐÑ
* 10. void OLED_OFF(void) -- Ë¯Ãß
* 11. void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize) -- ÏÔÊ¾×Ö·û´®(×ÖÌå´óÐ¡ÓÐ6*8ºÍ8*16Á½ÖÖ)
* 12. void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N) -- ÏÔÊ¾ÖÐÎÄ(ÖÐÎÄÐèÒªÏÈÈ¡Ä££¬È»ºó·Åµ½codetab.hÖÐ)
* 13. void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[]) -- BMPÍ¼Æ¬
*
* History: none;
*
*************************************************************************************/

#include "OLED_I2C.h"
#include "delay.h"
#include "codetab.h"

void I2C_Configuration(void)			//iicÅäÖÃ
{
	I2C_InitTypeDef  I2C_InitStructure;                      //¶¨ÒåÁËÒ»¸ö½á¹¹ÌåI2C_InitStructure; 
	GPIO_InitTypeDef  GPIO_InitStructure; 										//ÓÃµÄÊÇtypeDef¶¨ÒåµÄ²Î¿¼  ÕâÁ©Ò»Ñù

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);        //Ê¹ÓÃiIc1ÍâÉèÊ±ÖÓ£¬
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);			 //GPIOB

	/*STM32F103C8T6Ð¾Æ¬µÄÓ²¼þI2C: PB6 -- SCL; PB7 -- SDA */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7 | GPIO_Pin_6;    //Ñ¡ÔñÒý½Å
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		       //Ñ¡ÔñËÙ¶È
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;            //I2C±ØÐë¿ªÂ©Êä³ö
	GPIO_Init(GPIOB, &GPIO_InitStructure);										 //³õÊ¼»¯gpio¸ù¾Ý GPIO_InitStructure Õâ¸ö

	I2C_DeInit(I2C1);                                         //Ê¹ÓÃI2C1
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;								//ÉèÖÃiicÎªiicÄ£Ê½
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;				//I2C¿ìËÙÄ£Ê½Tlow / Thigh = 2
	I2C_InitStructure.I2C_OwnAddress1 = 0x30;               //Ö÷»úµÄI2CµØÖ·,Ëæ±ãÐ´µÄ
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;								//Ê¹ÄÜÓ¦´ð(ACK)
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;     //Ó¦´ð7Î»µØÖ·
	I2C_InitStructure.I2C_ClockSpeed = 400000;								//Ê±ÖÓÆµÂÊ ²»ÄÜ¸ßÓÚ400k

	I2C_Cmd(I2C1, ENABLE);																	  // Ê¹ÄÜ»òÕßÊ§ÄÜI2CÍâÉè  ÕâÀïÊÇÊ¹ÄÜiicÍâÉè1    
	I2C_Init(I2C1, &I2C_InitStructure);												//³õÊ¼»¯ iic ¸ù¾ÝI2C_InitTypeDef  Ö¸¶¨²ÎÊý
}

void I2C_WriteByte(uint8_t addr,uint8_t data)
{
  while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));             //¼ì²â×ÜÏßÊÇ·ñÎªÃ¦Âµ×´Ì¬¡£ Æä×ÓÎÄ¼þÀïÃæÓÐ¶I2C_FLAG_BUSYµÄ¶¨Òå·ÇÁãÔ
	
	I2C_GenerateSTART(I2C1, ENABLE);                           //¿ªÆôI2C1  ¿ªÆô´«ÊäÎÄ¼þÄ£Ê½
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));/*EV5,Ö÷Ä£Ê½*/

	I2C_Send7bitAddress(I2C1, OLED_ADDRESS, I2C_Direction_Transmitter);//Æ÷¼þµØÖ· -- Ä¬ÈÏ0x78 ·¢ËÍ
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)); //¼ìÑé×î½üÒ»´ÎÊÂ¼þÊÇ·ñÎªÊäÈëÊÂ¼þ ÊÇµÄ»° ²Å¿ÉÒÔ¼ÌÐø

	I2C_SendData(I2C1, addr);                               //¼Ä´æÆ÷µØÖ· ·¢ËÍµØÖ·
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	I2C_SendData(I2C1, data);//·¢ËÍÊý¾Ý
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
	I2C_GenerateSTOP(I2C1, ENABLE);//¹Ø±ÕI2C1×ÜÏß
}

void WriteCmd(unsigned char I2C_Command)//Ð´ÃüÁî
{
	I2C_WriteByte(0x00, I2C_Command);
}

void WriteDat(unsigned char I2C_Data)//Ð´Êý¾Ý
{
	I2C_WriteByte(0x40, I2C_Data);
}

void OLED_Init(void)
{
	DelayMs(100); //ÕâÀïµÄÑÓÊ±ºÜÖØÒª
	
	WriteCmd(0xAE); //display off
	WriteCmd(0x20);	//Set Memory Addressing Mode	
	WriteCmd(0x10);	//00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
	WriteCmd(0xb0);	//Set Page Start Address for Page Addressing Mode,0-7
	WriteCmd(0xc8);	//Set COM Output Scan Direction
	WriteCmd(0x00); //---set low column address
	WriteCmd(0x10); //---set high column address
	WriteCmd(0x40); //--set start line address
	WriteCmd(0x81); //--set contrast control register
	WriteCmd(0xff); //ÁÁ¶Èµ÷½Ú 0x00~0xff
	WriteCmd(0xa1); //--set segment re-map 0 to 127
	WriteCmd(0xa6); //--set normal display
	WriteCmd(0xa8); //--set multiplex ratio(1 to 64)
	WriteCmd(0x3F); //
	WriteCmd(0xa4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
	WriteCmd(0xd3); //-set display offset
	WriteCmd(0x00); //-not offset
	WriteCmd(0xd5); //--set display clock divide ratio/oscillator frequency
	WriteCmd(0xf0); //--set divide ratio
	WriteCmd(0xd9); //--set pre-charge period
	WriteCmd(0x22); //
	WriteCmd(0xda); //--set com pins hardware configuration
	WriteCmd(0x12);
	WriteCmd(0xdb); //--set vcomh
	WriteCmd(0x20); //0x20,0.77xVcc
	WriteCmd(0x8d); //--set DC-DC enable
	WriteCmd(0x14); //
	WriteCmd(0xaf); //--turn on oled panel
}

void OLED_SetPos(unsigned char x, unsigned char y) //ÉèÖÃÆðÊ¼µã×ø±ê
{ 
	WriteCmd(0xb0+y);
	WriteCmd(((x&0xf0)>>4)|0x10);
	WriteCmd((x&0x0f)|0x01);
}

void OLED_Fill(unsigned char fill_Data)//È«ÆÁÌî³ä
{
	unsigned char m,n;
	for(m=0;m<8;m++)
	{
		WriteCmd(0xb0+m);		//page0-page1
		WriteCmd(0x00);		//low column start address
		WriteCmd(0x10);		//high column start address
		for(n=0;n<128;n++)
			{
				WriteDat(fill_Data);
			}
	}
}

void OLED_CLS(void)//ÇåÆÁ
{
	OLED_Fill(0x00);
}

//--------------------------------------------------------------
// Prototype      : void OLED_ON(void)
// Calls          : 
// Parameters     : none
// Description    : ½«OLED´ÓÐÝÃßÖÐ»½ÐÑ
//--------------------------------------------------------------
void OLED_ON(void)
{
	WriteCmd(0X8D);  //ÉèÖÃµçºÉ±Ã
	WriteCmd(0X14);  //¿ªÆôµçºÉ±Ã
	WriteCmd(0XAF);  //OLED»½ÐÑ
}

//--------------------------------------------------------------
// Prototype      : void OLED_OFF(void)
// Calls          : 
// Parameters     : none
// Description    : ÈÃOLEDÐÝÃß -- ÐÝÃßÄ£Ê½ÏÂ,OLED¹¦ºÄ²»µ½10uA
//--------------------------------------------------------------
void OLED_OFF(void)
{
	WriteCmd(0X8D);  //ÉèÖÃµçºÉ±Ã
	WriteCmd(0X10);  //¹Ø±ÕµçºÉ±Ã
	WriteCmd(0XAE);  //OLEDÐÝÃß
}

//--------------------------------------------------------------
// Prototype      : void OLED_ShowChar(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize)
// Calls          : 
// Parameters     : x,y -- ÆðÊ¼µã×ø±ê(x:0~127, y:0~7); ch[] -- ÒªÏÔÊ¾µÄ×Ö·û´®; TextSize -- ×Ö·û´óÐ¡(1:6*8 ; 2:8*16)
// Description    : ÏÔÊ¾codetab.hÖÐµÄASCII×Ö·û,ÓÐ6*8ºÍ8*16¿ÉÑ¡Ôñ
//--------------------------------------------------------------
void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize)
{
	unsigned char c = 0,i = 0,j = 0;
	switch(TextSize)
	{
		case 1:
		{
			while(ch[j] != '\0')
			{
				c = ch[j] - 32;
				if(x > 126)
				{
					x = 0;
					y++;
				}
				OLED_SetPos(x,y);
				for(i=0;i<6;i++)
					WriteDat(F6x8[c][i]);
				x += 6;
				j++;
			}
		}break;
		case 2:
		{
			while(ch[j] != '\0')
			{
				c = ch[j] - 32;
				if(x > 120)
				{
					x = 0;
					y++;
				}
				OLED_SetPos(x,y);
				for(i=0;i<8;i++)
					WriteDat(F8X16[c*16+i]);
				OLED_SetPos(x,y+1);
				for(i=0;i<8;i++)
					WriteDat(F8X16[c*16+i+8]);
				x += 8;
				j++;
			}
		}break;
	}
}

//--------------------------------------------------------------
// Prototype      : void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N)
// Calls          : 
// Parameters     : x,y -- ÆðÊ¼µã×ø±ê(x:0~127, y:0~7); N:ºº×ÖÔÚcodetab.hÖÐµÄË÷Òý
// Description    : ÏÔÊ¾codetab.hÖÐµÄºº×Ö,16*16µãÕó
//--------------------------------------------------------------
void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N)
{
	unsigned char wm=0;
	unsigned int  adder=32*N;
	OLED_SetPos(x , y);
	for(wm = 0;wm < 16;wm++)
	{
		WriteDat(F16x16[adder]);
		adder += 1;
	}
	OLED_SetPos(x,y + 1);
	for(wm = 0;wm < 16;wm++)
	{
		WriteDat(F16x16[adder]);
		adder += 1;
	}
}

//--------------------------------------------------------------
// Prototype      : void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[]);
// Calls          : 
// Parameters     : x0,y0 -- ÆðÊ¼µã×ø±ê(x0:0~127, y0:0~7); x1,y1 -- Æðµã¶Ô½ÇÏß(½áÊøµã)µÄ×ø±ê(x1:1~128,y1:1~8)
// Description    : ÏÔÊ¾BMPÎ»Í¼
//--------------------------------------------------------------
void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[])
{
	unsigned int j=0;
	unsigned char x,y;

  if(y1%8==0)
		y = y1/8;
  else
		y = y1/8 + 1;
	for(y=y0;y<y1;y++)
	{
		OLED_SetPos(x0,y);
    for(x=x0;x<x1;x++)
		{
			WriteDat(BMP[j++]);
		}
	}
}





void OLED_digit6x8(unsigned char x, unsigned char y,unsigned int s)
{ 
    unsigned char i=0,a=0,b=0,c=0,d=0,e=0;
		a=s/10000;
		b=s%10000/1000;
		c=s%10000%1000/100;
		d=s%10000%1000%100/10;
		e=s%10;
	  a=a+16;
		OLED_SetPos(x,y);
	  for(i=0;i<6;i++)
	  WriteDat(F6x8[a][i]);
	  b=b+16;
	  x=x+6;
	  OLED_SetPos(x,y);
	  for(i=0;i<6;i++)
	  WriteDat(F6x8[b][i]);
	  
	  c=c+16;
	  x=x+6;
	  OLED_SetPos(x,y);
	  for(i=0;i<6;i++)
	  WriteDat(F6x8[c][i]);
	  
	  d=d+16;
	  x=x+6;
	  OLED_SetPos(x,y);
	  for(i=0;i<6;i++)
	  WriteDat(F6x8[d][i]);
	  
	  e=e+16;
	  x=x+6;
	  OLED_SetPos(x,y);
	  for(i=0;i<6;i++)
	  WriteDat(F6x8[e][i]);
}
void OLED_digit8x16(unsigned char x,unsigned char y,unsigned int s)
{ 
	unsigned char i=0,a=0,b=0,c=0,d=0,e=0;
	a=s/10000;
	b=s%10000/1000;
	c=s%10000%1000/100;
	d=s%10000%1000%100/10;
	e=s%10;
	a=a+16;
	OLED_SetPos(x,y);
	for(i=0;i<8;i++)
	WriteDat(F8X16[a*16+i]);
	OLED_SetPos(x,y+1);
	for(i=0;i<8;i++)
	WriteDat(F8X16[a*16+i+8]);

	b=b+16;
	x=x+8;
	OLED_SetPos(x,y);
	for(i=0;i<8;i++)
	WriteDat(F8X16[b*16+i]);
	OLED_SetPos(x,y+1);
	for(i=0;i<8;i++)
	WriteDat(F8X16[b*16+i+8]);
	
	c=c+16;
	x=x+8;
	OLED_SetPos(x,y);
	for(i=0;i<8;i++)
	WriteDat(F8X16[c*16+i]);
	OLED_SetPos(x,y+1);
	for(i=0;i<8;i++)
	WriteDat(F8X16[c*16+i+8]);
	
	d=d+16;
	x=x+8;
	OLED_SetPos(x,y);
	for(i=0;i<8;i++)
	WriteDat(F8X16[d*16+i]);
	OLED_SetPos(x,y+1);
	for(i=0;i<8;i++)
	WriteDat(F8X16[d*16+i+8]);
	
	e=e+16;
	x=x+8;
	OLED_SetPos(x,y);
	for(i=0;i<8;i++)
	WriteDat(F8X16[e*16+i]);
	OLED_SetPos(x,y+1);
	for(i=0;i<8;i++)
	WriteDat(F8X16[e*16+i+8]);

}
