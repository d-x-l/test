//////////////////////////////////////////////////////////////////////////////////	 
//  功能描述  : 独立看门狗演示例程(STM32F407ZET6系列)
//              说明: 
//              ----------------------------------------------------------------
//              ----------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////
#include "iwdg.h"  

//独立看门狗初始化
void IWDG_Init(void)
{
	//1.判断系统复位是否由看门狗导致
	if( RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET )
	{
		printf("System Reset Is By IWDG\n");
		RCC_ClearFlag();
	}
	else
	{
		printf("System Reset Is By User\n");
	}
		
	//2.解除写保护
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	
	//3.设置喂狗时间  256分配  32KHZ/256 = 125HZ 
	IWDG_SetPrescaler(IWDG_Prescaler_256);
	IWDG_SetReload(125-1);  //喂狗时间 = 1S
	
	//4.重载计数值
	IWDG_ReloadCounter();

	//5.启动看门狗
	IWDG_Enable();

}
