#include "systick.h"


//ϵͳ��ʱ���ĳ�ʼ��
void  SysTick_Init(void)
{
	//��168MHZ����8��Ƶ �õ�21MHZ  1000000us��21000000��  ---  1us��21��
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
}


//��ʱ΢��  ���ܳ���798900us
void Delay_us(uint16_t nus)
{
	SysTick->CTRL &= ~(1<<0); 			// �رն�ʱ��
	SysTick->LOAD = (21*nus) - 1; 		// ������װ��ֵ value-1
	SysTick->VAL  = 0; 					// ��յ�ǰ����ֵ
	SysTick->CTRL |= (1<<0); 			// ������ʱ��  ��ʼ����
	while ( (SysTick->CTRL & 0x00010000)== 0 );// �ȴ��������
	SysTick->CTRL = 0;					// �رն�ʱ��
	SysTick->VAL  = 0; 					// ��յ�ǰ����ֵ
}

//��ʱ����  ���ܳ���798.9ms
void Delay_ms(uint16_t nms)
{
	SysTick->CTRL &= ~(1<<0); 			// �رն�ʱ��
	SysTick->LOAD = (21*1000*nms) - 1; 	// ������װ��ֵ value-1
	SysTick->VAL  = 0; 					// ��յ�ǰ����ֵ
	SysTick->CTRL |= (1<<0); 			// ������ʱ��  ��ʼ����
	while ( (SysTick->CTRL & 0x00010000)== 0 );// �ȴ��������
	SysTick->CTRL = 0;					// �رն�ʱ��
	SysTick->VAL  = 0; 					// ��յ�ǰ����ֵ
}
