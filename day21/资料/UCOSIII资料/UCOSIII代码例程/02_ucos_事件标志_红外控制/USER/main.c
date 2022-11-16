#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "includes.h"

static GPIO_InitTypeDef  	GPIO_InitStructure;
static NVIC_InitTypeDef 	NVIC_InitStructure;		
static EXTI_InitTypeDef   	EXTI_InitStructure;

//����1���ƿ�
OS_TCB Task1_TCB;

void task1(void *parg);

CPU_STK task1_stk[128];			//����1�������ջ����СΪ128�֣�Ҳ����512�ֽ�


static volatile uint8_t 		g_ir_data[4]={0};

OS_FLAG_GRP	g_os_flag;			//����һ���¼���־��


void ir_init(void)
{

	/* GPIOAӲ��ʱ��ʹ�� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	/* Enable SYSCFG clock ��ʹ��ϵͳ����ʱ��*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);


	/* ����PA8����Ϊ����ģʽ  */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;						//��8������	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;					//����ģʽ���ܹ�����ⲿ��ƽ״̬
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;				//GPIO�����ٶ�Ϊ100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;				//����Ҫ����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* Configure EXTI Line8 �������ⲿ�жϿ�����8*/
	EXTI_InitStructure.EXTI_Line 	= EXTI_Line8;					//ʹ���ⲿ�жϿ�����8
	EXTI_InitStructure.EXTI_Mode 	= EXTI_Mode_Interrupt;			//�ж�ģʽ
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; 		//�½��ش������ܹ���⵽�����źŵĵ���
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;						//�жϿ�����ʹ�ܣ���������
	EXTI_Init(&EXTI_InitStructure);
	
	/* Connect EXTI Line8 to PA8 pin */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource8);

	
	/* Enable and set EXTI Line8 Interrupt to the lowest priority ��ʹ�ܲ������ⲿ�жϿ�����8�жϣ����ȼ������*/
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;				//�ⲿ�жϿ�����9_5�����ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;	//��ռ���ȼ�Ϊ0xF
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;			//��Ӧ���ȼ�Ϊ0xF
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					//�����ⲿ�жϿ�����9_5�����ж�
	NVIC_Init(&NVIC_InitStructure);


}


//������
int main(void)
{
	OS_ERR err;

	delay_init(168);  													//ʱ�ӳ�ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);						//�жϷ�������
	uart_init(115200);  				 								//���ڳ�ʼ��
	LED_Init();         												//LED��ʼ��	
	//�����ʼ��
	ir_init();

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




	//����һ���¼���־��
	OSFlagCreate(	(OS_FLAG_GRP*	)&g_os_flag,
					(CPU_CHAR*)		"os_flag",
					(OS_FLAGS)		0,
					(OS_ERR*		)&err);




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
		//OS_OPT_PEND_FLAG_SET_ANY ��OS_OPT_PEND_FLAG_SET_ALL���������
		//ǰ���ǵȴ�����һ���¼�������Ҫȷ�������¼�����
		os_flag= OSFlagPend((OS_FLAG_GRP*	)&g_os_flag,
							(OS_FLAGS)0x3,						//�ȴ�bit1��bit0��λ
							(OS_TICK)0,							//0������Ĭ��һֱ�����ȴ�
							(OS_OPT)OS_OPT_PEND_FLAG_SET_ANY+OS_OPT_PEND_FLAG_CONSUME,	//�ȴ��¼���־���κ�һλ��λ
							(CPU_TS *)0,						//Ĭ�ϲ���¼ʱ���
							&err);
		printf("task1,os_flag=%02X\r\n",os_flag);
		printf("ir data:%02X%02X%02X%02X\r\n",g_ir_data[0],g_ir_data[1],g_ir_data[2],g_ir_data[3]);
		

	}

}

//���������źţ��ܽ�����ɣ�ͨ��������ͬ�ĸߵ�ƽ����ʱ�䣬���ܹ�֪����ǰ���ź��������롢����0������1
uint8_t IR_PluseHighTime(void)
{
	uint8_t t=0;
	
	//�ߵ�ƽ
	while(PAin(8) == 1)
	{
		t++;
		delay_us(20);
		
		//��ʱ���
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
	
#if SYSTEM_SUPPORT_OS  //ʹ��UCOS����ϵͳ
	OSIntEnter();    
#endif	
	//��鵱ǰ�ⲿ�жϿ�����8�Ƿ񴥷��ж�
	if(EXTI_GetITStatus(EXTI_Line8) != RESET)
	{
		//��Ӵ���
		while(1)
		{
			//�����ָߵ�ƽ���ͽ��в���
			if(PAin(8)==1)
			{
				t = IR_PluseHighTime();
				
				//��ǰ�ź��ǷǷ��ź�
				if(t >=250)
					break;
				
				//�жϵ�ǰ�ź���������
				if(t>=200 && t<250)		//4ms ~ 5ms
				{
					ir_vaild=1;
					
					continue;
				
				}
				//�յ�bit1
				else if(t>=60 && t<90)	//1.2ms~1.8ms
				{
					ir_bit = 1;
				
				}
				//�յ�bit0
				else if(t>=10 && t<50)  //0.2ms ~ 1 ms
				{
					ir_bit = 0;
				}
				
				//��ȡbit����
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

					//��������У���жϣ���鵱ǰ���յ��ĺ��������Ƿ���ȷ
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
		
		/* Clear the EXTI line 8 pending bit������жϱ�־λ���ʹ���˵���Ѿ�����жϴ��� */
		EXTI_ClearITPendingBit(EXTI_Line8);
	 }
	
#if SYSTEM_SUPPORT_OS  
	OSIntExit();    	//�˳��ж�
#endif	
	if(ir_event)
	{
		OSFlagPost((OS_FLAG_GRP*	)&g_os_flag,
									(OS_FLAGS)0x1,
									(OS_OPT)OS_OPT_POST_FLAG_SET,
									&err);		
	
	}
	
}





