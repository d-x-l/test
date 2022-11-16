#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "dht11.h"
#include "includes.h"

//����1���ƿ�
OS_TCB Task1_TCB;

void task1(void *parg);

CPU_STK task1_stk[128];			//����1�������ջ����СΪ128�֣�Ҳ����512�ֽ�


//����2���ƿ�
OS_TCB Task2_TCB;

void task2(void *parg);

CPU_STK task2_stk[128];			//����2�������ջ����СΪ128�֣�Ҳ����512�ֽ�


//����3���ƿ�
OS_TCB Task3_TCB;

void task3(void *parg);

CPU_STK task3_stk[128];			//����3�������ջ����СΪ128�֣�Ҳ����512�ֽ�


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

	
	
	//��������1��������дǰ8������
	OSTaskCreate(	(OS_TCB *)&Task1_TCB,									//������ƿ�
					(CPU_CHAR *)"Task1",									//��������֣��Զ���
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
					(OS_PRIO)4,											 	//��������ȼ�		
					(CPU_STK *)task2_stk,									//�����ջ����ַ
					(CPU_STK_SIZE)128/10,									//�����ջ�����λ���õ����λ�ã��������ټ���ʹ��
					(CPU_STK_SIZE)128,										//�����ջ��С			
					(OS_MSG_QTY)0,											//��ֹ������Ϣ����
					(OS_TICK)0,												//Ĭ��ʱ��Ƭ����																
					(void  *)0,												//����Ҫ�����û��洢��
					(OS_OPT)OS_OPT_TASK_NONE,								//û���κ�ѡ��
					&err													//���صĴ�����
				);
					
	//��������3��������ʪ��������ʾ
	OSTaskCreate(	(OS_TCB *)&Task3_TCB,									//������ƿ�
					(CPU_CHAR *)"Task3",									//���������
					(OS_TASK_PTR)task3,										//������
					(void *)0,												//���ݲ���
					(OS_PRIO)2,											 	//��������ȼ�		
					(CPU_STK *)task3_stk,									//�����ջ����ַ
					(CPU_STK_SIZE)128/10,									//�����ջ�����λ���õ����λ�ã��������ټ���ʹ��
					(CPU_STK_SIZE)128,										//�����ջ��С			
					(OS_MSG_QTY)0,											//��ֹ������Ϣ����
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
	CPU_SR_ALLOC();							//���뱣��CPU��״̬
	
	OS_ERR err;
	printf("task1 is create ok\r\n");

	while(1)
	{
		//�����ٽ������������µĴ���
		OS_CRITICAL_ENTER();
		
		printf("task1 is running ...\r\n");
		
		//�˳��ٽ���
		OS_CRITICAL_EXIT();

		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //�ó�CPU����ʱ1ms
	}
}

void task2(void *parg)
{
	CPU_SR_ALLOC();							//���뱣��CPU��״̬
	
	OS_ERR err;
	printf("task2 is create ok\r\n");

	while(1)
	{
		//�����ٽ������������µĴ���
		OS_CRITICAL_ENTER();
		
		printf("task2 is running ...\r\n");
		
		//�˳��ٽ���
		OS_CRITICAL_EXIT();

		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //�ó�CPU����ʱ1ms
	}

}


void task3(void *parg)
{
	uint8_t dht_data[5]={0};
	
	CPU_SR_ALLOC();							//���뱣��CPU��״̬
	
	OS_ERR err;
	printf("task3 is create ok\r\n");
	
	//��ʪ��ģ���ʼ��
	dht11_init();

	while(1)
	{
		//�����ٽ������������µĴ���
		OS_CRITICAL_ENTER();
		
		if(dht11_read_data(dht_data)==0)
		{
			printf("temp=%d.%d\r\n",dht_data[2],dht_data[3]);
			printf("humi=%d.%d\r\n",dht_data[0],dht_data[1]);
			printf("\r\n");
		
		}
		
		//�˳��ٽ���
		OS_CRITICAL_EXIT();

		OSTimeDlyHMSM(0,0,2,0,OS_OPT_TIME_HMSM_STRICT,&err); //�ó�CPU����ʱ
        
	}

}








