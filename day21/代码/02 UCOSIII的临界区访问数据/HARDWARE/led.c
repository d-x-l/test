//////////////////////////////////////////////////////////////////////////////////	 
//  ��������   :LED��������ʾ����(STM32F407ZET6ϵ��)
//              ˵��: 
//              ----------------------------------------------------------------
//              LED0   ��PF9����
//              LED1   ��PF10����
//              LED2   ��PE13����
//              LED3   ��PE14����
//              ----------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////

#include "led.h"

//LED�Ƶĳ�ʼ��
void LED_Init(void)
{
	GPIO_InitTypeDef   GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF|RCC_AHB1Periph_GPIOE, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_OUT;			//���ģʽ
	GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;			//�������
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_100MHz;		//�������
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;			//��������
    
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_9|GPIO_Pin_10;	//���ű��
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	
    GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_13|GPIO_Pin_14;	//���ű��
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
    
	GPIO_SetBits(GPIOF,GPIO_Pin_9|GPIO_Pin_10); //Ĭ�ϲ���
    GPIO_SetBits(GPIOE,GPIO_Pin_13|GPIO_Pin_14); //Ĭ�ϲ���
}


