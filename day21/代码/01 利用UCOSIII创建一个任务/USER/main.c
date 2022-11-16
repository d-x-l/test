#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "includes.h"
#include "dht11.h"

//任务1控制块
OS_TCB Task1_TCB;

void task1(void *parg);

CPU_STK task1_stk[128];			//任务1的任务堆栈，大小为128字，也就是512字节


//主函数
int main(void)
{
    //定义一个错误处理对象
	OS_ERR err;

	
	delay_init(168);  								//时钟初始化   利用嘀嗒定时器来为操作系统产生周期性的节拍
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//中断分组配置 
	uart_init(115200);  				 			//串口初始化
    

	//OS初始化，它是第一个运行的函数,初始化各种的全局变量，例如中断嵌套计数器、优先级、存储器
	OSInit(&err);		   	 						//初始化UCOSIII
			 
	//创建开始任务
	//创建任务1，着重填写前8个参数
	OSTaskCreate(	(OS_TCB *)&Task1_TCB,									//任务控制块  任务控制块的地址
					(CPU_CHAR *)"Task1",									//任务的名字，自定义
					(OS_TASK_PTR)task1,										//任务函数
                    (void *)0,												//传递参数  NULL  不需要传参
					(OS_PRIO)1,											 	//任务的优先级	   数值越小，优先级越高	
					(CPU_STK *)task1_stk,									//任务堆栈基地址   本质上栈空间就是一个数组   如果任务较多，记得修改汇编文件中栈空间的大小
					(CPU_STK_SIZE)128/10,									//任务堆栈深度限位，用到这个位置，任务不能再继续使用
					(CPU_STK_SIZE)128,										//任务堆栈大小			
					(OS_MSG_QTY)0,											//禁止任务消息队列
					(OS_TICK)0,												//默认时间片长度																
					(void  *)0,												//不需要补充用户存储区
					(OS_OPT)OS_OPT_TASK_NONE,								//没有任何选项
					&err													//返回的错误码
				);
  
	OSStart(&err);     //启动OS，进行任务调度
}

//任务1  优先级1
void task1(void *parg)
{
	OS_ERR err;
	printf("task1 is create ok\r\n");

	while(1)
	{
		printf("task1 is running ...\r\n");
	
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //让出CPU，延时1s
	}
}

