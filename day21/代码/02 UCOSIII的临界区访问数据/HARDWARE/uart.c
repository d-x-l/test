//////////////////////////////////////////////////////////////////////////////////	 
//  ��������  : UART������ʾ����(STM32F407ZET6ϵ��)
//              ˵��: 
//              ----------------------------------------------------------------
//              USART1_TX  ��PA9
//              USART1_RX  ��PA10
//              USART3_TX  ��PB10
//              USART3_RX  ��PB11
//              ----------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////
#include "uart.h"


//�ض���fputc�����Ե���printf����
int fputc(int ch, FILE *f) 
{
	USART_SendData(USART1, ch);
	while( USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET ); //�ȴ����ݷ������
	
	return ch;
}

//����1�ĳ�ʼ��
void USART1_Init(uint32_t  baud)
{
	GPIO_InitTypeDef   GPIO_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	//1.��USRAT1����ʱ�� + GPIOA����ʱ��  PA9-U1_TX  PA10-U1_RX
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	//2.����GPIO���� PA9 PA10  ����ģʽ��Ҫ����Ϊ����ģʽ
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF;				//����ģʽ
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;			//��������
	GPIO_InitStructure.GPIO_OType 	= GPIO_OType_PP;			//�������
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_100MHz;		//��������	
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_9|GPIO_Pin_10;	//���ű��
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//3.��GPIO���ŵĹ��ܽ��и���  ����ΪUSART1  ��Ҫ��������
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);
	
	//4.����USART1  ������  ֹͣλ  ����λ  У��λ
	USART_InitStructure.USART_BaudRate = baud;						//������  9600bps
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//����λ  8bit
	USART_InitStructure.USART_StopBits = USART_StopBits_1;			//ֹͣλ  1bit
	USART_InitStructure.USART_Parity = USART_Parity_No;				//У��λ  ��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //��Ӳ��������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART1,&USART_InitStructure);
	
	//5.ָ��USART1���ж�Դ  ���յ����ݾͷ����ж�
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	
	//6.����NVIC ָ���ж�ͨ��+�ж����ȼ�
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;			//�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;		//��Ӧ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//ʹ��ͨ��
	NVIC_Init(&NVIC_InitStructure);	
	
	//7.�򿪴���
	USART_Cmd(USART1,ENABLE);
}

//����3�ĳ�ʼ��
void USART3_Init(uint32_t  baud)
{
	GPIO_InitTypeDef   GPIO_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	//1.��USRAT3����ʱ�� + GPIOB����ʱ��  PB10-U3_TX  PB11-U3_RX
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	//2.����GPIO���� PB10 PB11  ����ģʽ��Ҫ����Ϊ����ģʽ
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF;				//����ģʽ
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;			//��������
	GPIO_InitStructure.GPIO_OType 	= GPIO_OType_PP;			//�������
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_100MHz;		//��������	
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_11|GPIO_Pin_10;	//���ű��
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//3.��GPIO���ŵĹ��ܽ��и���  ����ΪUSART3  ��Ҫ��������
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3);
	
	//4.����USART3  ������  ֹͣλ  ����λ  У��λ
	USART_InitStructure.USART_BaudRate = baud;						//������  9600bps
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//����λ  8bit
	USART_InitStructure.USART_StopBits = USART_StopBits_1;			//ֹͣλ  1bit
	USART_InitStructure.USART_Parity = USART_Parity_No;				//У��λ  ��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //��Ӳ��������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART3,&USART_InitStructure);
	
	//5.ָ��USART3���ж�Դ  ���յ����ݾͷ����ж�
	USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);
	
	//6.����NVIC ָ���ж�ͨ��+�ж����ȼ�
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;			//�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;		//��Ӧ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//ʹ��ͨ��
	NVIC_Init(&NVIC_InitStructure);	
	
	//7.�򿪴���
	USART_Cmd(USART3,ENABLE);
}



//ͨ�����ڷ����ַ���
void USART_SendString(USART_TypeDef* USARTx,char *str)
{
	const char *p = str; //��ֹ��ַ��ʧ
	
	while(p && *p)
	{
		USART_SendData(USARTx,*p++); //���յ�������ͨ��USRAT3����  �������յ�
		while( USART_GetFlagStatus(USARTx,USART_FLAG_TXE) == RESET ); //�ȴ����ݷ������	
	}
}

//ͨ�����ڷ���n���ֽ�
void USART_SendBytes(USART_TypeDef* USARTx,char *str,uint32_t len)
{
	while(len--)
	{
		USART_SendData(USARTx,*str++); //���յ�������ͨ��USRAT3����  �������յ�
		while( USART_GetFlagStatus(USARTx,USART_FLAG_TXE) == RESET ); //�ȴ����ݷ������
	}
}

//USART1�жϷ�����
void USART1_IRQHandler(void)
{
	uint16_t recv_data = 0;
	//����ж��Ƿ���
	if( USART_GetITStatus(USART1,USART_IT_RXNE) != RESET )
	{
		USART_ClearITPendingBit(USART1,USART_IT_RXNE); //����жϱ�־
		
		recv_data = USART_ReceiveData(USART1); //�ѽ��յ������ݱ��浽������
		
		USART_SendData(USART3,recv_data); //���յ�������ͨ��USRAT3����  �������յ�
		while( USART_GetFlagStatus(USART3,USART_FLAG_TXE) == RESET ); //�ȴ����ݷ������	
	}
}

//USART3�жϷ�����
void USART3_IRQHandler(void)
{	
	//����ж��Ƿ���
	if( USART_GetITStatus(USART3,USART_IT_RXNE) != RESET )
	{
		USART_ClearITPendingBit(USART3,USART_IT_RXNE); //����жϱ�־
		
		USART3_RecvBuf[USART3_RecvCnt] = USART_ReceiveData(USART3); //�ѽ��յ������ݱ��浽������
		USART3_RecvCnt++;
		
		if( USART3_RecvBuf[USART3_RecvCnt-1] == '\n' || USART3_RecvCnt >= (sizeof USART3_RecvBuf )-1 )
		{
			USARt3_RecvFlag = 1; //˵������
		}
	}
}

