//////////////////////////////////////////////////////////////////////////////////	 
//  功能描述   : HC-SR04超声波模块演示例程(STM32F407ZET6系列)
//              说明: 
//              ----------------------------------------------------------------
//              GND   电源地
//              VCC   接5V电源
//              TRIG  接PB6引脚
//              ECHO  接PE6引脚     
//              注意：引脚接线可以根据自己的实际需求来  不用纠结
//              ----------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////
#include "sr04.h"


//超声波初始化 
void  SR04_Init(void)
{
	GPIO_InitTypeDef   GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOE, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_6;				//引脚编号
	
	//TRIG输出模式
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_OUT;			//输出模式
	GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;			//推挽输出
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_100MHz;		//输出速率
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;			//无上下拉
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//ECHO输入模式
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_IN;				//输入模式
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;			//无上下拉
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	TRIG_RESET;
}


//超声波获取距离
uint32_t SR04_GetDistance(void)
{
	uint32_t cnt=0;
	
	//1.TRIG输出至少10us的高电平
	TRIG_SET;
	Delay_us(15);
	TRIG_RESET;
	
	//2.等待ECHO引脚出现高电平
	while( ECHO_READ == 0 );
	
	//3.计算高电平持续的时间
	while( ECHO_READ == 1 )
	{
		Delay_us(9);
		cnt++;
	}
	
	cnt = cnt/2;
	
	return	cnt*3; //单位是mm

}	
