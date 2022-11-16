//////////////////////////////////////////////////////////////////////////////////	 
//  功能描述   :RTC实时时钟演示例程(STM32F407ZET6系列)
//              说明: 
//              ----------------------------------------------------------------
//              ----------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////
#include "rtc.h"


//RTC实时时钟的配置
void RTC_Config(void)
{
	RTC_DateTypeDef  RTC_DateStructure;
	RTC_TimeTypeDef  RTC_TimeStructure;
	RTC_InitTypeDef  RTC_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
	EXTI_InitTypeDef  EXTI_InitStructure;

	//1.打开PWR(电源控制器)外设的时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	
	//2.解除RTC寄存器的访问保护  才可以操作数据
	PWR_BackupAccessCmd(ENABLE);
	
	//3.配置低速外部时钟的晶振电路
	RCC_LSEConfig(RCC_LSE_ON);
	
	//4.等待低速外部时钟LSE的晶振电路稳定
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);
	
	//5.选择低速外部时钟LSE作为RTC的时钟源  32.768KHZ
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	
	//6.对低速外部时钟LSE的时钟频率进行分频
	//ck_spre(1Hz) = RTCCLK(LSE) /(uwAsynchPrediv + 1)*(uwSynchPrediv + 1)  --->  32768HZ / 128 / 256 = 1HZ
	RTC_InitStructure.RTC_AsynchPrediv 	= 0x7F;					//异步分频 128分频
	RTC_InitStructure.RTC_SynchPrediv 	= 0xFF;	  				//同步分频 256分频		
	RTC_InitStructure.RTC_HourFormat 	= RTC_HourFormat_24;  	//采用24小时制
	RTC_Init(&RTC_InitStructure);

	//7.使能RTC实时时钟
	RCC_RTCCLKCmd(ENABLE);
	
	//8.确保RTC的寄存器同步
	RTC_WaitForSynchro();
	
	//9.设置RTC的日期  2021年8月25日 星期三  "set date=2021-8-26-4\n"  --> "set date=2021-8-26-4" --> p = strstr(buf,"set date") -->p = strtok(p,"=")-->p = strtok(NULL,"=")
	//p --> "2021-8-26-4" --> p = strtok(p,"-") -->  "2021" --> int year = atoi(p) --> year = year-2000 --> 21 --> 33 --> year =  (year/10)*16 + year%10 --> p = strtok(NULL,"-") 
	RTC_DateStructure.RTC_Year = 0x21;						//年份    0x39+1 -->  BCD码 0x40    HEX码 0x3A
	RTC_DateStructure.RTC_Month = RTC_Month_August;			//月份
	RTC_DateStructure.RTC_Date = 0x25;						//日期
	RTC_DateStructure.RTC_WeekDay = RTC_Weekday_Wednesday;	//星期
	RTC_SetDate(RTC_Format_BCD, &RTC_DateStructure);
	
	//10.设置RTC的时间   17:22:30   "set time=9-27-30\n"   if(atoi(p) < 12) RTC_H12_AM else RTC_H12_PM
	RTC_TimeStructure.RTC_H12     = RTC_H12_PM;				//下午
	RTC_TimeStructure.RTC_Hours   = 0x17;					//时钟
	RTC_TimeStructure.RTC_Minutes = 0x22;					//分钟
	RTC_TimeStructure.RTC_Seconds = 0x30; 					//秒钟
	RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);   
	
	//11.配置EXTI22
	EXTI_ClearITPendingBit(EXTI_Line22);
	EXTI_InitStructure.EXTI_Line = EXTI_Line22;				//外部中断线EXTI22	
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;		//中断模式
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;	//上升沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;				//外部中断线使能
	EXTI_Init(&EXTI_InitStructure);

	//12.配置NVIC
	NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;			//中断唤醒通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			//响应优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能中断通道
	NVIC_Init(&NVIC_InitStructure);
	
	//13.禁止唤醒
	RTC_WakeUpCmd(DISABLE);
	
	//14.设置唤醒时钟源
	RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);
	
	//15.设置唤醒功能自动重载值   周期为1s  1HZ 数1次花费1s
	RTC_SetWakeUpCounter(0);
	
	//16.开启RTC唤醒功能
	RTC_WakeUpCmd(ENABLE);
	
	//17.使能RTC的中断源  周期性唤醒中断
	RTC_ITConfig(RTC_IT_WUT, ENABLE);
	 
	//18.清除唤醒标志
	RTC_ClearITPendingBit(RTC_IT_WUT);
}


//RTC时钟唤醒中断
void RTC_WKUP_IRQHandler(void)
{
	if( RTC_GetITStatus(RTC_IT_WUT) != RESET )
	{
		RTC_WakeUpFlag = 1;  //表示唤醒
		RTC_ClearITPendingBit(RTC_IT_WUT); //清除唤醒标志
	}
	EXTI_ClearITPendingBit(EXTI_Line22); //清除EXTI22线的标志
}
