//////////////////////////////////////////////////////////////////////////////////	 
//  ��������   :RTCʵʱʱ����ʾ����(STM32F407ZET6ϵ��)
//              ˵��: 
//              ----------------------------------------------------------------
//              ----------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////
#include "rtc.h"


//RTCʵʱʱ�ӵ�����
void RTC_Config(void)
{
	RTC_DateTypeDef  RTC_DateStructure;
	RTC_TimeTypeDef  RTC_TimeStructure;
	RTC_InitTypeDef  RTC_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
	EXTI_InitTypeDef  EXTI_InitStructure;

	//1.��PWR(��Դ������)�����ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	
	//2.���RTC�Ĵ����ķ��ʱ���  �ſ��Բ�������
	PWR_BackupAccessCmd(ENABLE);
	
	//3.���õ����ⲿʱ�ӵľ����·
	RCC_LSEConfig(RCC_LSE_ON);
	
	//4.�ȴ������ⲿʱ��LSE�ľ����·�ȶ�
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);
	
	//5.ѡ������ⲿʱ��LSE��ΪRTC��ʱ��Դ  32.768KHZ
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	
	//6.�Ե����ⲿʱ��LSE��ʱ��Ƶ�ʽ��з�Ƶ
	//ck_spre(1Hz) = RTCCLK(LSE) /(uwAsynchPrediv + 1)*(uwSynchPrediv + 1)  --->  32768HZ / 128 / 256 = 1HZ
	RTC_InitStructure.RTC_AsynchPrediv 	= 0x7F;					//�첽��Ƶ 128��Ƶ
	RTC_InitStructure.RTC_SynchPrediv 	= 0xFF;	  				//ͬ����Ƶ 256��Ƶ		
	RTC_InitStructure.RTC_HourFormat 	= RTC_HourFormat_24;  	//����24Сʱ��
	RTC_Init(&RTC_InitStructure);

	//7.ʹ��RTCʵʱʱ��
	RCC_RTCCLKCmd(ENABLE);
	
	//8.ȷ��RTC�ļĴ���ͬ��
	RTC_WaitForSynchro();
	
	//9.����RTC������  2021��8��25�� ������  "set date=2021-8-26-4\n"  --> "set date=2021-8-26-4" --> p = strstr(buf,"set date") -->p = strtok(p,"=")-->p = strtok(NULL,"=")
	//p --> "2021-8-26-4" --> p = strtok(p,"-") -->  "2021" --> int year = atoi(p) --> year = year-2000 --> 21 --> 33 --> year =  (year/10)*16 + year%10 --> p = strtok(NULL,"-") 
	RTC_DateStructure.RTC_Year = 0x21;						//���    0x39+1 -->  BCD�� 0x40    HEX�� 0x3A
	RTC_DateStructure.RTC_Month = RTC_Month_August;			//�·�
	RTC_DateStructure.RTC_Date = 0x25;						//����
	RTC_DateStructure.RTC_WeekDay = RTC_Weekday_Wednesday;	//����
	RTC_SetDate(RTC_Format_BCD, &RTC_DateStructure);
	
	//10.����RTC��ʱ��   17:22:30   "set time=9-27-30\n"   if(atoi(p) < 12) RTC_H12_AM else RTC_H12_PM
	RTC_TimeStructure.RTC_H12     = RTC_H12_PM;				//����
	RTC_TimeStructure.RTC_Hours   = 0x17;					//ʱ��
	RTC_TimeStructure.RTC_Minutes = 0x22;					//����
	RTC_TimeStructure.RTC_Seconds = 0x30; 					//����
	RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);   
	
	//11.����EXTI22
	EXTI_ClearITPendingBit(EXTI_Line22);
	EXTI_InitStructure.EXTI_Line = EXTI_Line22;				//�ⲿ�ж���EXTI22	
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;		//�ж�ģʽ
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;	//�����ش���
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;				//�ⲿ�ж���ʹ��
	EXTI_Init(&EXTI_InitStructure);

	//12.����NVIC
	NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;			//�жϻ���ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			//��Ӧ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//ʹ���ж�ͨ��
	NVIC_Init(&NVIC_InitStructure);
	
	//13.��ֹ����
	RTC_WakeUpCmd(DISABLE);
	
	//14.���û���ʱ��Դ
	RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);
	
	//15.���û��ѹ����Զ�����ֵ   ����Ϊ1s  1HZ ��1�λ���1s
	RTC_SetWakeUpCounter(0);
	
	//16.����RTC���ѹ���
	RTC_WakeUpCmd(ENABLE);
	
	//17.ʹ��RTC���ж�Դ  �����Ի����ж�
	RTC_ITConfig(RTC_IT_WUT, ENABLE);
	 
	//18.������ѱ�־
	RTC_ClearITPendingBit(RTC_IT_WUT);
}


//RTCʱ�ӻ����ж�
void RTC_WKUP_IRQHandler(void)
{
	if( RTC_GetITStatus(RTC_IT_WUT) != RESET )
	{
		RTC_WakeUpFlag = 1;  //��ʾ����
		RTC_ClearITPendingBit(RTC_IT_WUT); //������ѱ�־
	}
	EXTI_ClearITPendingBit(EXTI_Line22); //���EXTI22�ߵı�־
}
