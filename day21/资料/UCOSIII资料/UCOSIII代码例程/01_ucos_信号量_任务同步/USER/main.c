#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "includes.h"

//����1���ƿ�
OS_TCB Task1_TCB;

void task1(void *parg);

CPU_STK task1_stk[128];			//����1�������ջ����СΪ128�֣�Ҳ����512�ֽ�



//����2���ƿ�
OS_TCB Task2_TCB;

void task2(void *parg);

CPU_STK task2_stk[128];			//����2�������ջ����СΪ128�֣�Ҳ����512�ֽ�


OS_SEM	SYNC_SEM;				//����һ���ź�������������ͬ��

//������
int main(void)
{
	OS_ERR err;

	delay_init(168);  													//ʱ�ӳ�ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);						//�жϷ�������
	uart_init(9600);  				 								//���ڳ�ʼ��
	LED_Init();         												//LED��ʼ��	


	//OS��ʼ�������ǵ�һ�����еĺ���,��ʼ�����ֵ�ȫ�ֱ����������ж�Ƕ�׼����������ȼ����洢��
	OSInit(&err);


	//��������1
	OSTaskCreate(	(OS_TCB *)&Task1_TCB,									//������ƿ�
					(CPU_CHAR *)"Task1",									//���������
					(OS_TASK_PTR)task1,										//������
					(void *)0,												//���ݲ���
					(OS_PRIO)3,											 	//��������ȼ�		
					(CPU_STK *)task1_stk,									//�����ջ����ַ
					(CPU_STK_SIZE)128/10,									//�����ջ�����λ���õ����λ�ã��������ټ���ʹ��
					(CPU_STK_SIZE)128,										//�����ջ��С			
					(OS_MSG_QTY)0,											//��ֹ������Ϣ����
					(OS_TICK)0,												//Ĭ��ʱ��Ƭ����																
					(void  *)0,												//����Ҫ�����û��洢��
					(OS_OPT)OS_OPT_TASK_NONE,								//û���κ�ѡ��
					&err													//���صĴ�����
				);


	//��������2
	OSTaskCreate(	(OS_TCB *)&Task2_TCB,									//������ƿ�
					(CPU_CHAR *)"Task2",									//���������
					(OS_TASK_PTR)task2,										//������
					(void *)0,												//���ݲ���
					(OS_PRIO)2,											 	//��������ȼ�		
					(CPU_STK *)task2_stk,									//�����ջ����ַ
					(CPU_STK_SIZE)128/10,									//�����ջ�����λ���õ����λ�ã��������ټ���ʹ��
					(CPU_STK_SIZE)128,										//�����ջ��С			
					(OS_MSG_QTY)0,											//��ֹ������Ϣ����
					(OS_TICK)0,												//Ĭ��ʱ��Ƭ����																
					(void  *)0,												//����Ҫ�����û��洢��
					(OS_OPT)OS_OPT_TASK_NONE,								//û���κ�ѡ��
					&err													//���صĴ�����
				);

	//����һ���ź���
	OSSemCreate ((OS_SEM*	)&SYNC_SEM,
                 (CPU_CHAR*	)"SYNC_SEM",
                 (OS_SEM_CTR)0,		
                 (OS_ERR*	)&err);


	//����OS�������������
	OSStart(&err);
	
}


void task1(void *parg)
{
	OS_ERR err;

	printf("task1 is create ok\r\n");

	while(1)
	{

		OSSemPend(&SYNC_SEM,0,OS_OPT_PEND_BLOCKING,0,&err); //�����ź���

		printf("task1 is running ...\r\n");

	}

}

void task2(void *parg)
{
	OS_ERR err;

	printf("task2 is create ok\r\n");

	while(1)
	{
		//��������3���ź���
		OSSemPost(&SYNC_SEM,OS_OPT_POST_1,&err);					//�����ź���
		OSSemPost(&SYNC_SEM,OS_OPT_POST_1,&err);					//�����ź���
		OSSemPost(&SYNC_SEM,OS_OPT_POST_1,&err);					//�����ź���

		printf("task2 is running ...\r\n");

	
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); 		//��ʱ10ms  
	}
}








