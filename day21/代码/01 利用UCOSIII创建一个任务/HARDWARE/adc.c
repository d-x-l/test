//////////////////////////////////////////////////////////////////////////////////	 
//  功能描述  : ADC数模转换演示例程(STM32F407ZET6系列)
//              说明: 
//              ----------------------------------------------------------------
//              光敏电阻    接  
//              可调电阻    接  PA5  ASC1_IN5 
//              ----------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////
#include "adc.h"  


//电位器初始化
void  Res_ADC_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;

	//1.打开ADC外设时钟 +GPIO外设时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	//2.配置GPIO
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_5;				//引脚编号
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AN;				//模拟模式
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;			//无上下拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//3.配置ADC外设的参数
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;						//通道模式  独立模式  只用了一个ADC 
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;						//时钟分频  84MHZ/2 = 42MHZ
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;			//关闭DMA
	ADC_CommonInit(&ADC_CommonInitStructure);
	
	//4.配置ADC的通道的参数
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;							//转换精度  12bit
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;									//关闭扫描模式  单次转换
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;								//打开ADC转换器
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;		//不使用外部触发源  使用内部软件触发
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;							//数据对齐  右对齐
	ADC_InitStructure.ADC_NbrOfConversion = 1;										//单次转换
	ADC_Init(ADC1, &ADC_InitStructure);
	
	//5.指定ADC的通道
	 ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1, ADC_SampleTime_3Cycles);	   

	//6.使能ADC1
	 ADC_Cmd(ADC1, ENABLE);
}
