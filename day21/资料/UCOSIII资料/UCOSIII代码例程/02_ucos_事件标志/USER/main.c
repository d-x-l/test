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


//����3���ƿ�
OS_TCB Task3_TCB;

void task3(void *parg);

CPU_STK task3_stk[128];			//����2�������ջ����СΪ128�֣�Ҳ����512�ֽ�


OS_FLAG_GRP	g_os_flag;			//����һ���¼���־��


//������ʼ������
void KEY_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOE,ENABLE);  //ʹ��GPIOA GPIOEʱ��
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;  				//PE2,3,4����
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;  								//����
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;  								//��������
	GPIO_Init(GPIOE,&GPIO_InitStructure); 										//��ʼ��GPIOE
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;  									//PA0����
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_DOWN;  								//��������
	GPIO_Init(GPIOA,&GPIO_InitStructure);     									//��ʼ��GPIOA
}


//������
int main(void)
{
	OS_ERR err;

	delay_init(168);  													//ʱ�ӳ�ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);						//�жϷ�������
	uart_init(115200);  				 								//���ڳ�ʼ��
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

	//��������3
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


	//����һ���¼���־��
	OSFlagCreate(	(OS_FLAG_GRP*	)&g_os_flag,
					(CPU_CHAR*)"os_flag",
					(OS_FLAGS)0,
					(OS_ERR*		)&err);

	//������ʼ��
	KEY_Init();


	//����OS�������������
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
				//ȥ����
				OSTimeDlyHMSM(0,0,0,20,OS_OPT_TIME_HMSM_STRICT,&err); 		//��ʱ20ms  

				if(PAin(0) == 0)
				{
					//�����¼���־����־ֵΪ0x1������g_os_flag��bit0Ϊ1
					os_flag= OSFlagPost((OS_FLAG_GRP*	)&g_os_flag,
											(OS_FLAGS)0x1,
											(OS_OPT)OS_OPT_POST_FLAG_SET,
											&err);

					printf("task1,os_flag=%02X\r\n",os_flag);

				}

			}

			OSTimeDlyHMSM(0,0,0,20,OS_OPT_TIME_HMSM_STRICT,&err); 			//��ʱ20ms  


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
			OSTimeDlyHMSM(0,0,0,20,OS_OPT_TIME_HMSM_STRICT,&err); 		//��ʱ20ms  

			if(PEin(2) == 0)
			{
				//�����¼���־����־ֵΪ0x2������g_os_flag��bit1Ϊ1
				os_flag= OSFlagPost((OS_FLAG_GRP*	)&g_os_flag,
									(OS_FLAGS)0x2,
									(OS_OPT)OS_OPT_POST_FLAG_SET,
									&err);

				printf("task2,os_flag=%02X\r\n",os_flag);

			}

		}

		OSTimeDlyHMSM(0,0,0,20,OS_OPT_TIME_HMSM_STRICT,&err); 		//��ʱ20ms  

	}

}

void task3(void *parg)
{
	OS_ERR err;

	OS_FLAGS os_flag;

	printf("task3 is create ok\r\n");

	while(1)
	{
		//OS_OPT_PEND_FLAG_SET_ANY ��OS_OPT_PEND_FLAG_SET_ALL���������
		//ǰ���ǵȴ�����һ���¼�������Ҫȷ�������¼�����
		os_flag= OSFlagPend((OS_FLAG_GRP*	)&g_os_flag,
							(OS_FLAGS)0x3,						//�ȴ�bit1��bit0��λ
							(OS_TICK)0,							//0������Ĭ��һֱ�����ȴ�
							(OS_OPT)OS_OPT_PEND_FLAG_SET_ANY+OS_OPT_PEND_FLAG_CONSUME,	//�ȴ��¼���־���κ�һλ��λ
							(CPU_TS *)0,						//Ĭ�ϲ���¼ʱ���
							&err);

		printf("task3,os_flag=%02X\r\n",os_flag);

	}

}





