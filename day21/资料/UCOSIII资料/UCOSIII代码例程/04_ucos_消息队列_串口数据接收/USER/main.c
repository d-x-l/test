#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "includes.h"

//����1���ƿ�
OS_TCB Task1_TCB;

void task1(void *parg);

CPU_STK task1_stk[128];			//����1�������ջ����СΪ128�֣�Ҳ����512�ֽ�

#define TASK_Q_NUM	64			//�������ڽ���Ϣ���еĳ���


//������
int main(void)
{
	OS_ERR err;

	delay_init(168);  													//ʱ�ӳ�ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);						//�жϷ�������
	uart_init(9600);  				 									//���ڳ�ʼ��
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
					(OS_MSG_QTY)TASK_Q_NUM,									//����1��Ҫʹ���ڽ���Ϣ���У���Ϣ���г���Ϊ64��	
					(OS_TICK)0,												//Ĭ��ʱ��Ƭ����																
					(void  *)0,												//����Ҫ�����û��洢��
					(OS_OPT)OS_OPT_TASK_NONE,								//û���κ�ѡ��
					&err													//���صĴ�����
				);

	//����OS�������������
	OSStart(&err);
	
}



void task1(void *parg)
{
	char *p=NULL;
	OS_MSG_SIZE size;
	
	OS_ERR err;

	printf("task1 is create ok\r\n");

	while(1)
	{
		//������Ϣ
		p=OSTaskQPend((OS_TICK		)0,
                      (OS_OPT		)OS_OPT_PEND_BLOCKING,
                      (OS_MSG_SIZE*	)&size,
                      (CPU_TS*		)0,
                      (OS_ERR*      )&err );
		if(p && size)
		{
			printf("%s:%d\r\n",p,size);
			
			//��ջ�����
			memset(p,0,size);
		}
			
	}
}



