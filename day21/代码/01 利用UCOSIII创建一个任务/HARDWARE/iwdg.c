//////////////////////////////////////////////////////////////////////////////////	 
//  ��������  : �������Ź���ʾ����(STM32F407ZET6ϵ��)
//              ˵��: 
//              ----------------------------------------------------------------
//              ----------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////
#include "iwdg.h"  

//�������Ź���ʼ��
void IWDG_Init(void)
{
	//1.�ж�ϵͳ��λ�Ƿ��ɿ��Ź�����
	if( RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET )
	{
		printf("System Reset Is By IWDG\n");
		RCC_ClearFlag();
	}
	else
	{
		printf("System Reset Is By User\n");
	}
		
	//2.���д����
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	
	//3.����ι��ʱ��  256����  32KHZ/256 = 125HZ 
	IWDG_SetPrescaler(IWDG_Prescaler_256);
	IWDG_SetReload(125-1);  //ι��ʱ�� = 1S
	
	//4.���ؼ���ֵ
	IWDG_ReloadCounter();

	//5.�������Ź�
	IWDG_Enable();

}
