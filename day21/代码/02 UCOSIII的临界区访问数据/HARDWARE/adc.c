//////////////////////////////////////////////////////////////////////////////////	 
//  ��������  : ADC��ģת����ʾ����(STM32F407ZET6ϵ��)
//              ˵��: 
//              ----------------------------------------------------------------
//              ��������    ��  
//              �ɵ�����    ��  PA5  ASC1_IN5 
//              ----------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////
#include "adc.h"  


//��λ����ʼ��
void  Res_ADC_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;

	//1.��ADC����ʱ�� +GPIO����ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	//2.����GPIO
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_5;				//���ű��
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AN;				//ģ��ģʽ
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;			//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//3.����ADC����Ĳ���
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;						//ͨ��ģʽ  ����ģʽ  ֻ����һ��ADC 
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;						//ʱ�ӷ�Ƶ  84MHZ/2 = 42MHZ
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;			//�ر�DMA
	ADC_CommonInit(&ADC_CommonInitStructure);
	
	//4.����ADC��ͨ���Ĳ���
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;							//ת������  12bit
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;									//�ر�ɨ��ģʽ  ����ת��
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;								//��ADCת����
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;		//��ʹ���ⲿ����Դ  ʹ���ڲ��������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;							//���ݶ���  �Ҷ���
	ADC_InitStructure.ADC_NbrOfConversion = 1;										//����ת��
	ADC_Init(ADC1, &ADC_InitStructure);
	
	//5.ָ��ADC��ͨ��
	 ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1, ADC_SampleTime_3Cycles);	   

	//6.ʹ��ADC1
	 ADC_Cmd(ADC1, ENABLE);
}
