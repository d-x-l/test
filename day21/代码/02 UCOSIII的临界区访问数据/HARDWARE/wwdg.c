//////////////////////////////////////////////////////////////////////////////////	 
//  ��������  : ���ڿ��Ź���ʾ����(STM32F407ZET6ϵ��)
//              ˵��: 
//              ----------------------------------------------------------------
//              ----------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////
#include "wwdg.h"  


//���ڿ��Ź���ʼ��
void WWDG_Init(void)
{
	NVIC_InitTypeDef   NVIC_InitStructure;
	
	//1.�ж�ϵͳ��λ�Ƿ���WWDG���Ź�����
	if( RCC_GetFlagStatus(RCC_FLAG_WWDGRST) != RESET )
	{
		printf("System Reset Is By WWDG\n");
		RCC_ClearFlag();
	}
	else
	{
		printf("System Reset Is By User\n");
	}

	//2.��WWDG�����ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);

	//3.����WWDG��ʱ�ӷ�Ƶ
	//WWDG clock counter = (PCLK1 (42MHz)/4096)/8 = 1281 Hz (~780 us)
	WWDG_SetPrescaler(WWDG_Prescaler_8);

	//4.����WWDG�Ĵ������� ��ֵ����С��128�����ֵΪ127
	WWDG_SetWindowValue(80);
	
	//5.ʹ��WWDG���貢���ü�����ֵ  between 0x40 and 0x7F
	//˼���������û���ι��������ʱ����ʲô��Χ֮�䣿  
	// ~780 * (127-80) = 36.6ms < refresh window < ~780 * 64 = 49.9ms  �����ڸ��ڼ��ڽ���ι���Ķ���
	//����ֵ127  ��������Ϊ80   127-80 = 47��   36.66ms
	//����ֵ127  ��������Ϊ80   127-63 = 64��   49.92ms  (ʱ������ֵ)
	WWDG_Enable(127);
	
	//6.����WWDG�жϵĲ���  NVIC
	NVIC_InitStructure.NVIC_IRQChannel = WWDG_IRQn;				//�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;		//��Ӧ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//ʹ��ͨ��
	NVIC_Init(&NVIC_InitStructure);	
	
	//7.����жϻ��ѱ�־
	WWDG_ClearFlag();
	
	//8.��WWDG�жϻ��ѹ���
	WWDG_EnableIT();
}

//WWDG�жϷ�����
void  WWDG_IRQHandler(void)
{
	if( WWDG_GetFlagStatus() != RESET )
	{
		WWDG_SetCounter(127); 	//ִ��ι��
		WWDG_ClearFlag();		//����жϻ��ѱ�־
	}
}

