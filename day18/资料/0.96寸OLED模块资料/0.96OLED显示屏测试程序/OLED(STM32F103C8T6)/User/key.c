#include "key.h"

void key_init()	   //
{
	GPIO_InitTypeDef GPIO_InitStructure;	  //声明一个结构体变量，用来初始化GPIO
	SystemInit();
	/* 开启GPIO时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

	/*  配置GPIO的模式和IO口 */
	GPIO_InitStructure.GPIO_Pin= key;	   //选择你要设置的IO口
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;//下拉输入  
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	   //设置传输速率
	GPIO_Init(GPIOB,&GPIO_InitStructure);		  /* 初始化GPIO */
				
	GPIO_ResetBits(GPIOA,key);	//对K_UP初始化输出0
}


