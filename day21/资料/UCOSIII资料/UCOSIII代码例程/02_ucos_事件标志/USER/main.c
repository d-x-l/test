#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "includes.h"

//任务1控制块
OS_TCB Task1_TCB;

void task1(void *parg);

CPU_STK task1_stk[128];			//任务1的任务堆栈，大小为128字，也就是512字节



//任务2控制块
OS_TCB Task2_TCB;

void task2(void *parg);

CPU_STK task2_stk[128];			//任务2的任务堆栈，大小为128字，也就是512字节


//任务3控制块
OS_TCB Task3_TCB;

void task3(void *parg);

CPU_STK task3_stk[128];			//任务2的任务堆栈，大小为128字，也就是512字节


OS_FLAG_GRP	g_os_flag;			//定义一个事件标志组


//按键初始化函数
void KEY_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOE,ENABLE);  //使能GPIOA GPIOE时钟
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;  				//PE2,3,4引脚
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;  								//输入
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;  								//上拉输入
	GPIO_Init(GPIOE,&GPIO_InitStructure); 										//初始化GPIOE
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;  									//PA0引脚
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_DOWN;  								//下拉输入
	GPIO_Init(GPIOA,&GPIO_InitStructure);     									//初始化GPIOA
}


//主函数
int main(void)
{
	OS_ERR err;

	delay_init(168);  													//时钟初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);						//中断分组配置
	uart_init(115200);  				 								//串口初始化
	LED_Init();         												//LED初始化	


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


	//创建任务2
	OSTaskCreate(	(OS_TCB *)&Task2_TCB,									//任务控制块
					(CPU_CHAR *)"Task2",									//任务的名字
					(OS_TASK_PTR)task2,										//任务函数
					(void *)0,												//传递参数
					(OS_PRIO)2,											 	//任务的优先级		
					(CPU_STK *)task2_stk,									//任务堆栈基地址
					(CPU_STK_SIZE)128/10,									//任务堆栈深度限位，用到这个位置，任务不能再继续使用
					(CPU_STK_SIZE)128,										//任务堆栈大小			
					(OS_MSG_QTY)0,											//禁止任务消息队列
					(OS_TICK)0,												//默认时间片长度																
					(void  *)0,												//不需要补充用户存储区
					(OS_OPT)OS_OPT_TASK_NONE,								//没有任何选项
					&err													//返回的错误码
				);

	//创建任务3
	OSTaskCreate(	(OS_TCB *)&Task3_TCB,									//任务控制块
					(CPU_CHAR *)"Task3",									//任务的名字
					(OS_TASK_PTR)task3,										//任务函数
					(void *)0,												//传递参数
					(OS_PRIO)2,											 	//任务的优先级		
					(CPU_STK *)task3_stk,									//任务堆栈基地址
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
					(CPU_CHAR*)"os_flag",
					(OS_FLAGS)0,
					(OS_ERR*		)&err);

	//按键初始化
	KEY_Init();


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
			if(PAin(0) == 0)
			{
				//去抖动
				OSTimeDlyHMSM(0,0,0,20,OS_OPT_TIME_HMSM_STRICT,&err); 		//延时20ms  

				if(PAin(0) == 0)
				{
					//发送事件标志，标志值为0x1，设置g_os_flag的bit0为1
					os_flag= OSFlagPost((OS_FLAG_GRP*	)&g_os_flag,
											(OS_FLAGS)0x1,
											(OS_OPT)OS_OPT_POST_FLAG_SET,
											&err);

					printf("task1,os_flag=%02X\r\n",os_flag);

				}

			}

			OSTimeDlyHMSM(0,0,0,20,OS_OPT_TIME_HMSM_STRICT,&err); 			//延时20ms  


	}

}

void task2(void *parg)
{
	OS_ERR err;

	OS_FLAGS os_flag;

	printf("task2 is create ok\r\n");



	while(1)
	{
		if(PEin(2) == 0)
		{
			OSTimeDlyHMSM(0,0,0,20,OS_OPT_TIME_HMSM_STRICT,&err); 		//延时20ms  

			if(PEin(2) == 0)
			{
				//发送事件标志，标志值为0x2，设置g_os_flag的bit1为1
				os_flag= OSFlagPost((OS_FLAG_GRP*	)&g_os_flag,
									(OS_FLAGS)0x2,
									(OS_OPT)OS_OPT_POST_FLAG_SET,
									&err);

				printf("task2,os_flag=%02X\r\n",os_flag);

			}

		}

		OSTimeDlyHMSM(0,0,0,20,OS_OPT_TIME_HMSM_STRICT,&err); 		//延时20ms  

	}

}

void task3(void *parg)
{
	OS_ERR err;

	OS_FLAGS os_flag;

	printf("task3 is create ok\r\n");

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

		printf("task3,os_flag=%02X\r\n",os_flag);

	}

}





