//////////////////////////////////////////////////////////////////////////////////	 
//  功能描述  : 窗口看门狗演示例程(STM32F407ZET6系列)
//              说明: 
//              ----------------------------------------------------------------
//              ----------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////
#include "wwdg.h"  


//窗口看门狗初始化
void WWDG_Init(void)
{
	NVIC_InitTypeDef   NVIC_InitStructure;
	
	//1.判断系统复位是否由WWDG看门狗导致
	if( RCC_GetFlagStatus(RCC_FLAG_WWDGRST) != RESET )
	{
		printf("System Reset Is By WWDG\n");
		RCC_ClearFlag();
	}
	else
	{
		printf("System Reset Is By User\n");
	}

	//2.打开WWDG外设的时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);

	//3.设置WWDG的时钟分频
	//WWDG clock counter = (PCLK1 (42MHz)/4096)/8 = 1281 Hz (~780 us)
	WWDG_SetPrescaler(WWDG_Prescaler_8);

	//4.设置WWDG的窗口上限 该值必须小于128，最大值为127
	WWDG_SetWindowValue(80);
	
	//5.使能WWDG外设并设置计数初值  between 0x40 and 0x7F
	//思考：现在用户的喂狗操作的时间在什么范围之间？  
	// ~780 * (127-80) = 36.6ms < refresh window < ~780 * 64 = 49.9ms  必须在该期间内进行喂狗的动作
	//计数值127  窗口上限为80   127-80 = 47次   36.66ms
	//计数值127  窗口下限为80   127-63 = 64次   49.92ms  (时间的最大值)
	WWDG_Enable(127);
	
	//6.设置WWDG中断的参数  NVIC
	NVIC_InitStructure.NVIC_IRQChannel = WWDG_IRQn;				//中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;		//响应优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能通道
	NVIC_Init(&NVIC_InitStructure);	
	
	//7.清空中断唤醒标志
	WWDG_ClearFlag();
	
	//8.打开WWDG中断唤醒功能
	WWDG_EnableIT();
}

//WWDG中断服务函数
void  WWDG_IRQHandler(void)
{
	if( WWDG_GetFlagStatus() != RESET )
	{
		WWDG_SetCounter(127); 	//执行喂狗
		WWDG_ClearFlag();		//清空中断唤醒标志
	}
}

