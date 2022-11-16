//////////////////////////////////////////////////////////////////////////////////	 
//  ��������   : HC-SR04������ģ����ʾ����(STM32F407ZET6ϵ��)
//              ˵��: 
//              ----------------------------------------------------------------
//              GND   ��Դ��
//              VCC   ��5V��Դ
//              TRIG  ��PB6����
//              ECHO  ��PE6����     
//              ע�⣺���Ž��߿��Ը����Լ���ʵ��������  ���þ���
//              ----------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////
#include "sr04.h"


//��������ʼ�� 
void  SR04_Init(void)
{
	GPIO_InitTypeDef   GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOE, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_6;				//���ű��
	
	//TRIG���ģʽ
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_OUT;			//���ģʽ
	GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;			//�������
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_100MHz;		//�������
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;			//��������
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//ECHO����ģʽ
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_IN;				//����ģʽ
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;			//��������
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	TRIG_RESET;
}


//��������ȡ����
uint32_t SR04_GetDistance(void)
{
	uint32_t cnt=0;
	
	//1.TRIG�������10us�ĸߵ�ƽ
	TRIG_SET;
	Delay_us(15);
	TRIG_RESET;
	
	//2.�ȴ�ECHO���ų��ָߵ�ƽ
	while( ECHO_READ == 0 );
	
	//3.����ߵ�ƽ������ʱ��
	while( ECHO_READ == 1 )
	{
		Delay_us(9);
		cnt++;
	}
	
	cnt = cnt/2;
	
	return	cnt*3; //��λ��mm

}	
