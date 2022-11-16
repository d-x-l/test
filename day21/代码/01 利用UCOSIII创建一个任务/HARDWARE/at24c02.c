//////////////////////////////////////////////////////////////////////////////////	 
//  功能描述   : AT24C02芯片演示例程(STM32F407ZET6系列)
//              说明: 
//              ----------------------------------------------------------------
//              SCL   接PB8引脚
//              SDA   接PE9引脚     
//              ----------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////
#include "at24c02.h"


//AT24C02初始化
void AT24C02_IIC_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	//1.打开GPIO外设时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	//2.配置GPIO引脚
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_8|GPIO_Pin_9;						//引脚编号
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_OUT;								//输出模式
	GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;								//推挽输出
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_100MHz;							//输出速率
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;								//无上下拉
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//3.空闲状态下SDA和SCL都为高电平
	AT24C02_IIC_SCL_WRITE(1);
	AT24C02_IIC_SDA_WRITE(1);
}

//AT24C02芯片SDA引脚输出模式
void AT24C02_SDAOutputMode(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//1.打开GPIO外设时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	//2.配置GPIO引脚
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_9;									//引脚编号
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_OUT;								//输出模式
	GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;								//推挽输出
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_100MHz;							//输出速率
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_UP;									//上拉电阻
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

//AT24C02芯片SDA引脚输入模式
void AT24C02_SDAInputMode(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//1.打开GPIO外设时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	//2.配置GPIO引脚
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_9;									//引脚编号
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_IN;									//输入模式
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_UP;									//上拉电阻
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

//IIC总线开始信号
void AT24C02_IIC_StartSignal(void)
{
	//1.设置SDA引脚为输出模式
	AT24C02_SDAOutputMode();
	
	//2.确保SDA和SCL为高电平
	AT24C02_IIC_SCL_WRITE(1);
	AT24C02_IIC_SDA_WRITE(1);
    Delay_us(5);            //提高程序可靠性
    
	//3.把SDA引脚拉低
	AT24C02_IIC_SDA_WRITE(0);
	Delay_us(5);   // IIC总线的通信速率为100KHZ  1000000us = 100000HZ  10us = 1HZ 
	
	//4.把SCL引脚拉低，表示准备通信
	AT24C02_IIC_SCL_WRITE(0);
}

//IIC总线停止信号
void AT24C02_IIC_StopSignal(void)
{
	//1.设置SDA引脚为输出模式
	AT24C02_SDAOutputMode();

	//2.确保SDA和SCL为低电平
	AT24C02_IIC_SCL_WRITE(0);
	AT24C02_IIC_SDA_WRITE(0);
	
	//4.把SCL引脚拉高
	AT24C02_IIC_SCL_WRITE(1);
	Delay_us(5);
	
	//5.把SDA引脚拉高
	AT24C02_IIC_SDA_WRITE(1);
	Delay_us(5);  //确保SDA的电平状态可以被其他从器件检测到
}

//主机发送数据（从机读取数据）
void AT24C02_IIC_SendBytes(uint8_t Data)  
{
	uint8_t i= 0;
	
	//1.设置SDA引脚为输出模式
	AT24C02_SDAOutputMode();
	
	//2.确保SDA和SCL为低电平
	AT24C02_IIC_SCL_WRITE(0);
	AT24C02_IIC_SDA_WRITE(0);
	
	//3.循环发送bit
	for(i=0;i<8;i++)
	{
		//MSB 高位先出   主机准备数据
		if ( Data & 1<<(7-i) ) 
		{
			AT24C02_IIC_SDA_WRITE(1);
		}
		else
			AT24C02_IIC_SDA_WRITE(0);
		
		Delay_us(5);
		
		//SCL为高电平    主机发送数据
		AT24C02_IIC_SCL_WRITE(1);
		Delay_us(5);
		
		//SCL为低电平    主机准备数据
		AT24C02_IIC_SCL_WRITE(0);
		Delay_us(5);
	}
}

//主机读取数据（从机发送数据）
uint8_t AT24C02_IIC_ReadBytes(void)
{
	uint8_t i = 0;
	uint8_t Data = 0; 
	
	//1.设置SDA引脚为输入模式
	AT24C02_SDAInputMode();
	
	//2.SCL为低电平      从机准备数据
	AT24C02_IIC_SCL_WRITE(0);
	Delay_us(5);
	
	//3.循环接收数据
	for(i=0;i<8;i++)
	{
		//SCL为高电平    从机发送数据
		AT24C02_IIC_SCL_WRITE(1);
		Delay_us(5);
		
		if( AT24C02_IIC_SDA_READ == 1) //说明接收的是1
		{
			Data |= 1<<(7-i);
		}
		
		//SCL为低电平    从机准备数据
		AT24C02_IIC_SCL_WRITE(0);
		Delay_us(5);
	}
	
	return Data;
}

//主机等待从机应答  返回0 说明从机应答   返回1 说明从机没应答
uint8_t AT24C02_IIC_WaitAck(void)
{
	uint8_t ack;
	
	//1.设置SDA引脚为输入模式
	AT24C02_SDAInputMode();

	//2.SCL为低电平      从机准备数据
	AT24C02_IIC_SCL_WRITE(0);
	Delay_us(5);
	
	//3.SCL为高电平      从机发送数据
	AT24C02_IIC_SCL_WRITE(1);
	Delay_us(5);
	
	//主机判断从机的数据
	if( AT24C02_IIC_SDA_READ == 1)
	{
		ack=1; //说明无应答
	}
	else
		ack=0; //说明已应答
	
	//4.SCL为低电平       主机忽略数据
	AT24C02_IIC_SCL_WRITE(0);
	Delay_us(5);
	
	return ack;
}

//从机发送数据，主机接收数据并发送应答信号
void  AT24C02_IIC_MasterAck(uint8_t ack)
{
	//1.设置SDA引脚为输出模式
	AT24C02_SDAOutputMode();
	
	//2.确保SDA和SCL为低电平  
	AT24C02_IIC_SCL_WRITE(0);
	AT24C02_IIC_SDA_WRITE(0);
	
	//3.主机准备数据
	if(ack)
		AT24C02_IIC_SDA_WRITE(1);
	else
		AT24C02_IIC_SDA_WRITE(0);
	
	Delay_us(5);
	
	//4.SCL为高电平       主机发送数据
	AT24C02_IIC_SCL_WRITE(1);
	Delay_us(5);
	
	//5.SCL为低电平       从机忽略数据
	AT24C02_IIC_SCL_WRITE(0);
	Delay_us(5);
}


//AT24C02的字节写入  一次写入1字节
void AT24C02_WordWrite(uint8_t Address,uint8_t Data)
{
    //1.主机发送开始信号
    AT24C02_IIC_StartSignal();
    
    //2.主机发送器件地址  写操作
    AT24C02_IIC_SendBytes(0xA0);
    
    //3.主机等待从机应答
    if( AT24C02_IIC_WaitAck() == 1 ) //如果没应答
    {
        printf("AT24C02 Ack Device Address Error\n");
        AT24C02_IIC_StopSignal();  
    }
    //printf("AT24C02 Ack Device Address OK\n");
    
    //4.主机发送存储地址 写操作
    AT24C02_IIC_SendBytes(Address);
    if( AT24C02_IIC_WaitAck() == 1 ) //如果没应答
    {
        printf("AT24C02 Ack Data Address Error\n");
        AT24C02_IIC_StopSignal();  
    }
    //printf("AT24C02 Ack Data Address OK\n");
    
    //5.主机发送存储数据 写操作
     AT24C02_IIC_SendBytes(Data);
    if( AT24C02_IIC_WaitAck() == 1 ) //如果没应答
    {
        printf("AT24C02 Write Data Error\n");
        AT24C02_IIC_StopSignal();  
    }
    //printf("AT24C02 Write Data OK\n");
    
    //6.主机发送停止信号
    AT24C02_IIC_StopSignal(); 
}

//AT24C02读取1字节数据
uint8_t AT24C02_WordRead(uint8_t Address)
{
    uint8_t data = 0;
    
    //1.主机发送开始信号
    AT24C02_IIC_StartSignal();
   
    //2.主机发送器件地址  写操作
    AT24C02_IIC_SendBytes(0xA0);
    
    //3.主机等待从机应答
    if( AT24C02_IIC_WaitAck() == 1 ) //如果没应答
    {
        printf("AT24C02 Ack Device Write Address Error\n");
        AT24C02_IIC_StopSignal();  
    }
    //printf("AT24C02 Ack Device Write Address OK\n"); 

    //4.主机发送存储地址  打算读取数据的地址
    AT24C02_IIC_SendBytes(Address);
    
    //5.主机等待从机应答
    if( AT24C02_IIC_WaitAck() == 1 ) //如果没应答
    {
        printf("AT24C02 Ack Data Address Error\n");
        AT24C02_IIC_StopSignal();  
    }
    //printf("AT24C02 Ack Data Address OK\n");
    
    //6.主机再次发起开始信号
    AT24C02_IIC_StartSignal();
    
    //7.主机发送器件地址  读操作
    AT24C02_IIC_SendBytes(0xA1);
    
    //8.主机等待从机应答
    if( AT24C02_IIC_WaitAck() == 1 ) //如果没应答
    {
        printf("AT24C02 Ack Device Read Address Error\n");
        AT24C02_IIC_StopSignal();  
    }
    //printf("AT24C02 Ack Device Read Address OK\n"); 
    
    //9.主机读取1字节数据
     data = AT24C02_IIC_ReadBytes();
    
    //10.主机发送应答信号
    AT24C02_IIC_MasterAck(1);    //不应答
    
    //11.主机发送停止信号
    AT24C02_IIC_StopSignal(); 
    
    return data;

}

//AT24C02读取n字节数据         存储地址      缓冲数组         数据个数
void AT24C02_RandomRead(uint8_t Address,uint8_t* RecvBuf,uint8_t DataLen)
{
    //1.主机发送开始信号
    AT24C02_IIC_StartSignal();
   
    //2.主机发送器件地址  写操作
    AT24C02_IIC_SendBytes(0xA0);
    
    //3.主机等待从机应答
    if( AT24C02_IIC_WaitAck() == 1 ) //如果没应答
    {
        printf("AT24C02 Ack Device Write Address Error\n");
        AT24C02_IIC_StopSignal();  
    }
    //printf("AT24C02 Ack Device Write Address OK\n"); 

    //4.主机发送存储地址  打算读取数据的地址
    AT24C02_IIC_SendBytes(Address);
    
    //5.主机等待从机应答
    if( AT24C02_IIC_WaitAck() == 1 ) //如果没应答
    {
        printf("AT24C02 Ack Data Address Error\n");
        AT24C02_IIC_StopSignal();  
    }
    //printf("AT24C02 Ack Data Address OK\n");
    
    //6.主机再次发起开始信号
    AT24C02_IIC_StartSignal();
    
    //7.主机发送器件地址  读操作
    AT24C02_IIC_SendBytes(0xA1);
    
    //8.主机等待从机应答
    if( AT24C02_IIC_WaitAck() == 1 ) //如果没应答
    {
        printf("AT24C02 Ack Device Read Address Error\n");
        AT24C02_IIC_StopSignal();  
    }
    //printf("AT24C02 Ack Device Read Address OK\n"); 
    
    //9.主机读取n字节数据
    DataLen = DataLen-1;
    while(DataLen--)                    //读取n-1个
    {
      *RecvBuf++ = AT24C02_IIC_ReadBytes();  
      AT24C02_IIC_MasterAck(0);  //表示主机收到   
    }
    
    //主机接收最后1个字节
    *RecvBuf = AT24C02_IIC_ReadBytes(); 
    
    //10.主机发送应答信号
    AT24C02_IIC_MasterAck(1);    //不应答
    
    //11.主机发送停止信号
    AT24C02_IIC_StopSignal(); 

}

//AT24C02当前地址读   存储地址指的是AT24C02内部地址计数器中的地址  
uint8_t AT24C02_CurrentAddressRead(void)
{
    uint8_t data = 0;
    
    //1.主机发送开始信号
    AT24C02_IIC_StartSignal();
    
    //2.主机发送器件地址  读操作
    AT24C02_IIC_SendBytes(0xA1);
    
    //3.主机等待从机应答
    if( AT24C02_IIC_WaitAck() == 1 ) //如果没应答
    {
        printf("AT24C02 Ack Device Address Error\n");
        AT24C02_IIC_StopSignal();  
    }
    //printf("AT24C02 Ack Device Address OK\n");
    
    //4.主机读取1字节数据
    data = AT24C02_IIC_ReadBytes();
    
    //5.主机发送应答信号
    AT24C02_IIC_MasterAck(1);
    
    //6.主机发送停止信号
    AT24C02_IIC_StopSignal(); 
    
    return data;
}

//AT24C02页写入  一次最多写入8字节
void AT24C02_PageWrite(uint8_t Page_Address,uint8_t *buf,uint8_t DataLen)
{
    //1.主机发送开始信号
    AT24C02_IIC_StartSignal();

    //2.主机发送器件地址  写操作
    AT24C02_IIC_SendBytes(0xA0);
    
    //3.主机等待从机应答
    if( AT24C02_IIC_WaitAck() == 1 ) //如果没应答
    {
        printf("AT24C02 Ack Device Address Error\n");
        AT24C02_IIC_StopSignal();  
    }
    //printf("AT24C02 Ack Device Address OK\n");
    
    //4.主机发送存储地址  页地址
    AT24C02_IIC_SendBytes(Page_Address);
    
    if( AT24C02_IIC_WaitAck() == 1 ) //如果没应答
    {
        printf("AT24C02 Ack Page Address Error\n");
        AT24C02_IIC_StopSignal();  
    }
    //printf("AT24C02 Ack Page Address OK\n");
    
    //5.主机循环发送字节
    while(DataLen--)
    {
        AT24C02_IIC_SendBytes(*buf++);
        if( AT24C02_IIC_WaitAck() == 1 ) //如果没应答
        {
            printf("AT24C02 Write Data Error\n");
            AT24C02_IIC_StopSignal();  
        }   
    }
    
    //6.主机发送停止信号
    AT24C02_IIC_StopSignal(); 
}
