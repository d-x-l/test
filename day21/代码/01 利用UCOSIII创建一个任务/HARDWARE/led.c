//////////////////////////////////////////////////////////////////////////////////	 
//  功能描述   :LED灯引脚演示例程(STM32F407ZET6系列)
//              说明: 
//              ----------------------------------------------------------------
//              LED0   接PF9引脚
//              LED1   接PF10引脚
//              LED2   接PE13引脚
//              LED3   接PE14引脚
//              ----------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////

#include "led.h"

//LED灯的初始化
void LED_Init(void)
{
	GPIO_InitTypeDef   GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF|RCC_AHB1Periph_GPIOE, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_OUT;			//输出模式
	GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;			//推挽输出
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_100MHz;		//输出速率
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;			//无上下拉
    
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_9|GPIO_Pin_10;	//引脚编号
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	
    GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_13|GPIO_Pin_14;	//引脚编号
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
    
	GPIO_SetBits(GPIOF,GPIO_Pin_9|GPIO_Pin_10); //默认不亮
    GPIO_SetBits(GPIOE,GPIO_Pin_13|GPIO_Pin_14); //默认不亮
}


