//////////////////////////////////////////////////////////////////////////////////	 
//  功能描述  : UART串口演示例程(STM32F407ZET6系列)
//              说明: 
//              ----------------------------------------------------------------
//              USART1_TX  接PA9
//              USART1_RX  接PA10
//              USART3_TX  接PB10
//              USART3_RX  接PB11
//              ----------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////
#include "uart.h"


//重定向fputc，可以调用printf函数
int fputc(int ch, FILE *f) 
{
	USART_SendData(USART1, ch);
	while( USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET ); //等待数据发送完成
	
	return ch;
}

//串口1的初始化
void USART1_Init(uint32_t  baud)
{
	GPIO_InitTypeDef   GPIO_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	//1.打开USRAT1外设时钟 + GPIOA外设时钟  PA9-U1_TX  PA10-U1_RX
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	//2.配置GPIO引脚 PA9 PA10  引脚模式需要设置为复用模式
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF;				//复用模式
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;			//无上下拉
	GPIO_InitStructure.GPIO_OType 	= GPIO_OType_PP;			//推挽输出
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_100MHz;		//引脚速率	
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_9|GPIO_Pin_10;	//引脚编号
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//3.把GPIO引脚的功能进行复用  复用为USART1  需要调用两次
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);
	
	//4.配置USART1  波特率  停止位  数据位  校验位
	USART_InitStructure.USART_BaudRate = baud;						//波特率  9600bps
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//数据位  8bit
	USART_InitStructure.USART_StopBits = USART_StopBits_1;			//停止位  1bit
	USART_InitStructure.USART_Parity = USART_Parity_No;				//校验位  无
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //无硬件流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART1,&USART_InitStructure);
	
	//5.指定USART1的中断源  接收到数据就发生中断
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	
	//6.配置NVIC 指定中断通道+中断优先级
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;			//中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;		//响应优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能通道
	NVIC_Init(&NVIC_InitStructure);	
	
	//7.打开串口
	USART_Cmd(USART1,ENABLE);
}

//串口3的初始化
void USART3_Init(uint32_t  baud)
{
	GPIO_InitTypeDef   GPIO_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	//1.打开USRAT3外设时钟 + GPIOB外设时钟  PB10-U3_TX  PB11-U3_RX
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	//2.配置GPIO引脚 PB10 PB11  引脚模式需要设置为复用模式
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF;				//复用模式
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;			//无上下拉
	GPIO_InitStructure.GPIO_OType 	= GPIO_OType_PP;			//推挽输出
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_100MHz;		//引脚速率	
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_11|GPIO_Pin_10;	//引脚编号
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//3.把GPIO引脚的功能进行复用  复用为USART3  需要调用两次
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3);
	
	//4.配置USART3  波特率  停止位  数据位  校验位
	USART_InitStructure.USART_BaudRate = baud;						//波特率  9600bps
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//数据位  8bit
	USART_InitStructure.USART_StopBits = USART_StopBits_1;			//停止位  1bit
	USART_InitStructure.USART_Parity = USART_Parity_No;				//校验位  无
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //无硬件流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART3,&USART_InitStructure);
	
	//5.指定USART3的中断源  接收到数据就发生中断
	USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);
	
	//6.配置NVIC 指定中断通道+中断优先级
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;			//中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;		//响应优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能通道
	NVIC_Init(&NVIC_InitStructure);	
	
	//7.打开串口
	USART_Cmd(USART3,ENABLE);
}



//通过串口发送字符串
void USART_SendString(USART_TypeDef* USARTx,char *str)
{
	const char *p = str; //防止地址丢失
	
	while(p && *p)
	{
		USART_SendData(USARTx,*p++); //把收到的数据通过USRAT3发送  蓝牙才收到
		while( USART_GetFlagStatus(USARTx,USART_FLAG_TXE) == RESET ); //等待数据发送完成	
	}
}

//通过串口发送n个字节
void USART_SendBytes(USART_TypeDef* USARTx,char *str,uint32_t len)
{
	while(len--)
	{
		USART_SendData(USARTx,*str++); //把收到的数据通过USRAT3发送  蓝牙才收到
		while( USART_GetFlagStatus(USARTx,USART_FLAG_TXE) == RESET ); //等待数据发送完成
	}
}

//USART1中断服务函数
void USART1_IRQHandler(void)
{
	uint16_t recv_data = 0;
	//检测中断是否发生
	if( USART_GetITStatus(USART1,USART_IT_RXNE) != RESET )
	{
		USART_ClearITPendingBit(USART1,USART_IT_RXNE); //清除中断标志
		
		recv_data = USART_ReceiveData(USART1); //把接收到的数据保存到变量中
		
		USART_SendData(USART3,recv_data); //把收到的数据通过USRAT3发送  蓝牙才收到
		while( USART_GetFlagStatus(USART3,USART_FLAG_TXE) == RESET ); //等待数据发送完成	
	}
}

//USART3中断服务函数
void USART3_IRQHandler(void)
{	
	//检测中断是否发生
	if( USART_GetITStatus(USART3,USART_IT_RXNE) != RESET )
	{
		USART_ClearITPendingBit(USART3,USART_IT_RXNE); //清除中断标志
		
		USART3_RecvBuf[USART3_RecvCnt] = USART_ReceiveData(USART3); //把接收到的数据保存到数组中
		USART3_RecvCnt++;
		
		if( USART3_RecvBuf[USART3_RecvCnt-1] == '\n' || USART3_RecvCnt >= (sizeof USART3_RecvBuf )-1 )
		{
			USARt3_RecvFlag = 1; //说明成立
		}
	}
}

