#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "includes.h"

static GPIO_InitTypeDef  	GPIO_InitStructure;
static NVIC_InitTypeDef 	NVIC_InitStructure;		
static EXTI_InitTypeDef   	EXTI_InitStructure;

//任务1控制块
OS_TCB Task1_TCB;

void task1(void *parg);

CPU_STK task1_stk[128];			//任务1的任务堆栈，大小为128字，也就是512字节


static volatile uint8_t 		g_ir_data[4]={0};

OS_FLAG_GRP	g_os_flag;			//定义一个事件标志组


void ir_init(void)
{

	/* GPIOA硬件时钟使能 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	/* Enable SYSCFG clock ，使能系统配置时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);


	/* 配置PA8引脚为输入模式  */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;						//第8根引脚	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;					//输入模式，能够检测外部电平状态
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;				//GPIO最大的速度为100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;				//不需要上下拉电阻
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* Configure EXTI Line8 ，配置外部中断控制线8*/
	EXTI_InitStructure.EXTI_Line 	= EXTI_Line8;					//使能外部中断控制线8
	EXTI_InitStructure.EXTI_Mode 	= EXTI_Mode_Interrupt;			//中断模式
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; 		//下降沿触发，能够检测到红外信号的到达
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;						//中断控制线使能，让它工作
	EXTI_Init(&EXTI_InitStructure);
	
	/* Connect EXTI Line8 to PA8 pin */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource8);

	
	/* Enable and set EXTI Line8 Interrupt to the lowest priority ，使能并设置外部中断控制线8中断，优先级是最低*/
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;				//外部中断控制线9_5触发中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;	//抢占优先级为0xF
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;			//响应优先级为0xF
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					//允许外部中断控制线9_5触发中断
	NVIC_Init(&NVIC_InitStructure);


}


//主函数
int main(void)
{
	OS_ERR err;

	delay_init(168);  													//时钟初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);						//中断分组配置
	uart_init(115200);  				 								//串口初始化
	LED_Init();         												//LED初始化	
	//红外初始化
	ir_init();

	//OS初始化，它是第一个运行的函数,初始化各种的全局变量，例如中断嵌套计数器、优先级、存储器
	OSInit(&err);


	
	//创建任务1
	OSTaskCreate(	(OS_TCB *)&Task1_TCB,									//任务控制块
					(CPU_CHAR *)"Task1",									//任务的名字
					(OS_TASK_PTR)task1,										//任务函数
					(void *)0,												//传递参数
					(OS_PRIO)3,											 	//任务的优先级		
					(CPU_STK *)task1_stk,									//任务堆栈基地址
					(CPU_STK_SIZE)128/10,									//任务堆栈深度限位，用到这个位置，任务不能再继续使用
					(CPU_STK_SIZE)128,										//任务堆栈大小			
					(OS_MSG_QTY)0,											//禁止任务消息队列
					(OS_TICK)0,												//默认时间片长度																
					(void  *)0,												//不需要补充用户存储区
					(OS_OPT)OS_OPT_TASK_NONE,								//没有任何选项
					&err													//返回的错误码
				);




	//创建一个事件标志组
	OSFlagCreate(	(OS_FLAG_GRP*	)&g_os_flag,
					(CPU_CHAR*)		"os_flag",
					(OS_FLAGS)		0,
					(OS_ERR*		)&err);




	//启动OS，进行任务调度
	OSStart(&err);
	
}



void task1(void *parg)
{
	OS_ERR err;

	OS_FLAGS os_flag;

	printf("task1 is create ok\r\n");

	while(1)
	{
		//OS_OPT_PEND_FLAG_SET_ANY 和OS_OPT_PEND_FLAG_SET_ALL是有区别的
		//前者是等待任意一个事件，后者要确保所有事件成立
		os_flag= OSFlagPend((OS_FLAG_GRP*	)&g_os_flag,
							(OS_FLAGS)0x3,						//等待bit1和bit0置位
							(OS_TICK)0,							//0代表是默认一直阻塞等待
							(OS_OPT)OS_OPT_PEND_FLAG_SET_ANY+OS_OPT_PEND_FLAG_CONSUME,	//等待事件标志组任何一位置位
							(CPU_TS *)0,						//默认不记录时间戳
							&err);
		printf("task1,os_flag=%02X\r\n",os_flag);
		printf("ir data:%02X%02X%02X%02X\r\n",g_ir_data[0],g_ir_data[1],g_ir_data[2],g_ir_data[3]);
		

	}

}

//分析红外信号，总结出规律，通过测量不同的高电平持续时间，就能够知道当前的信号是引导码、比特0、比特1
uint8_t IR_PluseHighTime(void)
{
	uint8_t t=0;
	
	//高电平
	while(PAin(8) == 1)
	{
		t++;
		delay_us(20);
		
		//超时溢出
		if(t > 250)
			return t;
	
	}

	return t;
}


void EXTI9_5_IRQHandler(void)
{
	uint8_t t=0,ir_vaild=0,ir_bit=0;
	uint32_t ir_data=0,ir_bit_cnt=0;
	uint32_t ir_event=0;
	
	OS_ERR err;
	
#if SYSTEM_SUPPORT_OS  //使用UCOS操作系统
	OSIntEnter();    
#endif	
	//检查当前外部中断控制线8是否触发中断
	if(EXTI_GetITStatus(EXTI_Line8) != RESET)
	{
		//添加代码
		while(1)
		{
			//若出现高电平，就进行测量
			if(PAin(8)==1)
			{
				t = IR_PluseHighTime();
				
				//当前信号是非法信号
				if(t >=250)
					break;
				
				//判断当前信号是引导码
				if(t>=200 && t<250)		//4ms ~ 5ms
				{
					ir_vaild=1;
					
					continue;
				
				}
				//收到bit1
				else if(t>=60 && t<90)	//1.2ms~1.8ms
				{
					ir_bit = 1;
				
				}
				//收到bit0
				else if(t>=10 && t<50)  //0.2ms ~ 1 ms
				{
					ir_bit = 0;
				}
				
				//获取bit数据
				if(ir_vaild)
				{
					ir_data|=ir_bit<<ir_bit_cnt;
				}
				
				ir_bit_cnt++;
				
				if(ir_bit_cnt >=32)
				{
					g_ir_data[0] = (uint8_t)((ir_data>>24)&0xFF);
					g_ir_data[1] = (uint8_t)((ir_data>>16)&0xFF);				
					g_ir_data[2] = (uint8_t)((ir_data>>8)&0xFF);
					g_ir_data[3] = (uint8_t)(ir_data&0xFF);

					//进行数据校验判断，检查当前接收到的红外数据是否正确
					if(g_ir_data[0] == (0xFF - g_ir_data[1]))
					{
						if(g_ir_data[2] == (0xFF - g_ir_data[3]))
						{
							//printf("ir data:%02X%02X%02X%02X\r\n",g_ir_data[0],g_ir_data[1],g_ir_data[2],g_ir_data[3]);
							ir_event = 1;
					
						}
					
					}
					
					break;

				}
			}
		
		}
		
		/* Clear the EXTI line 8 pending bit，清空中断标志位，就代表说我已经完成中断处理 */
		EXTI_ClearITPendingBit(EXTI_Line8);
	 }
	
#if SYSTEM_SUPPORT_OS  
	OSIntExit();    	//退出中断
#endif	
	if(ir_event)
	{
		OSFlagPost((OS_FLAG_GRP*	)&g_os_flag,
									(OS_FLAGS)0x1,
									(OS_OPT)OS_OPT_POST_FLAG_SET,
									&err);		
	
	}
	
}





