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


OS_SEM	SYNC_SEM;				//定义一个信号量，用于任务同步

//主函数
int main(void)
{
	OS_ERR err;

	delay_init(168);  													//时钟初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);						//中断分组配置
	uart_init(9600);  				 								//串口初始化
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

	//创建一个信号量
	OSSemCreate ((OS_SEM*	)&SYNC_SEM,
                 (CPU_CHAR*	)"SYNC_SEM",
                 (OS_SEM_CTR)0,		
                 (OS_ERR*	)&err);


	//启动OS，进行任务调度
	OSStart(&err);
	
}


void task1(void *parg)
{
	OS_ERR err;

	printf("task1 is create ok\r\n");

	while(1)
	{

		OSSemPend(&SYNC_SEM,0,OS_OPT_PEND_BLOCKING,0,&err); //请求信号量

		printf("task1 is running ...\r\n");

	}

}

void task2(void *parg)
{
	OS_ERR err;

	printf("task2 is create ok\r\n");

	while(1)
	{
		//连续发送3个信号量
		OSSemPost(&SYNC_SEM,OS_OPT_POST_1,&err);					//发送信号量
		OSSemPost(&SYNC_SEM,OS_OPT_POST_1,&err);					//发送信号量
		OSSemPost(&SYNC_SEM,OS_OPT_POST_1,&err);					//发送信号量

		printf("task2 is running ...\r\n");

	
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); 		//延时10ms  
	}
}








