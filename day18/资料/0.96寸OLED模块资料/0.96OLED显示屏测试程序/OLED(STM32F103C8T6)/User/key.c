#include "key.h"

void key_init()	   //
{
	GPIO_InitTypeDef GPIO_InitStructure;	  //����һ���ṹ�������������ʼ��GPIO
	SystemInit();
	/* ����GPIOʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

	/*  ����GPIO��ģʽ��IO�� */
	GPIO_InitStructure.GPIO_Pin= key;	   //ѡ����Ҫ���õ�IO��
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;//��������  
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	   //���ô�������
	GPIO_Init(GPIOB,&GPIO_InitStructure);		  /* ��ʼ��GPIO */
				
	GPIO_ResetBits(GPIOA,key);	//��K_UP��ʼ�����0
}


