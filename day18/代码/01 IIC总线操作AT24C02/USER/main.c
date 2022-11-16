#include "stm32f4xx.h"  
#include <stdio.h>
#include <string.h>


//IIC总线的引脚
#define  IIC_SDA_WRITE(n)   (n)?GPIO_SetBits(GPIOB,GPIO_Pin_9):GPIO_ResetBits(GPIOB,GPIO_Pin_9)
#define  IIC_SCL_WRITE(n)   (n)?GPIO_SetBits(GPIOB,GPIO_Pin_8):GPIO_ResetBits(GPIOB,GPIO_Pin_8)

#define  IIC_SDA_READ       GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)

//W25Q128的SCK时钟引脚
#define  W25Q128_CS(n)		(n)?GPIO_SetBits(GPIOB,GPIO_Pin_14):GPIO_ResetBits(GPIOB,GPIO_Pin_14)	



#define FLASH_USER_START_ADDR   ADDR_FLASH_SECTOR_6		//扇区6的开始地址
#define FLASH_USER_END_ADDR     ADDR_FLASH_SECTOR_7     //扇区7的开始地址


/* flash扇区的基地址 */ 
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base address of Sector 0, 16 Kbytes   */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base address of Sector 1, 16 Kbytes   */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base address of Sector 2, 16 Kbytes   */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base address of Sector 3, 16 Kbytes   */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base address of Sector 4, 64 Kbytes   */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base address of Sector 5, 128 Kbytes  */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base address of Sector 6, 128 Kbytes  */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base address of Sector 7, 128 Kbytes  */
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) /* Base address of Sector 8, 128 Kbytes  */
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) /* Base address of Sector 9, 128 Kbytes  */
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) /* Base address of Sector 10, 128 Kbytes */
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) /* Base address of Sector 11, 128 Kbytes */


#define  TRIG_SET  	  GPIO_SetBits(GPIOB,GPIO_Pin_6)
#define  TRIG_RESET   GPIO_ResetBits(GPIOB,GPIO_Pin_6)
#define  ECHO_READ    GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_6)

#define  DHT_READ	  GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_9)

static volatile unsigned int RTC_WakeUpFlag = 0; //表示RTC是否唤醒  0 没唤醒  1 唤醒了

RTC_DateTypeDef  RTC_DateStructure;
RTC_TimeTypeDef  RTC_TimeStructure;


uint32_t uwStartSector = 0; 	//保存扇区6的扇区号
uint32_t uwEndSector   = 0; 	//保存扇区7的扇区号
uint32_t uwAddress = 0;			
uint32_t uwSectorCounter = 0; 	//保存当前的扇区号

__IO uint32_t uwData32 = 0;
__IO uint32_t uwMemoryProgramStatus = 0;


//重定向fputc，可以调用printf函数
int fputc(int ch, FILE *f) 
{
	USART_SendData(USART1, ch);
	while( USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET ); //等待数据发送完成
	
	return ch;
}

void LED_Init(void)
{
	GPIO_InitTypeDef   GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_OUT;			//输出模式
	GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;			//推挽输出
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_100MHz;		//输出速率
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;			//无上下拉
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_9;				//引脚编号
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOF,GPIO_Pin_9); //默认不亮
}

//系统定时器的初始化
void  SysTick_Init(void)
{
	//把168MHZ经过8分频 得到21MHZ  1000000us振荡21000000次  ---  1us振荡21次
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
}


//延时微秒  不能超过798900us
void Delay_us(uint16_t nus)
{
	SysTick->CTRL &= ~(1<<0); 			// 关闭定时器
	SysTick->LOAD = (21*nus) - 1; 		// 设置重装载值 value-1
	SysTick->VAL  = 0; 					// 清空当前计数值
	SysTick->CTRL |= (1<<0); 			// 开启定时器  开始倒数
	while ( (SysTick->CTRL & 0x00010000)== 0 );// 等待倒数完成
	SysTick->CTRL = 0;					// 关闭定时器
	SysTick->VAL  = 0; 					// 清空当前计数值
}

//延时毫秒  不能超过798.9ms
void Delay_ms(uint16_t nms)
{
	SysTick->CTRL &= ~(1<<0); 			// 关闭定时器
	SysTick->LOAD = (21*1000*nms) - 1; 	// 设置重装载值 value-1
	SysTick->VAL  = 0; 					// 清空当前计数值
	SysTick->CTRL |= (1<<0); 			// 开启定时器  开始倒数
	while ( (SysTick->CTRL & 0x00010000)== 0 );// 等待倒数完成
	SysTick->CTRL = 0;					// 关闭定时器
	SysTick->VAL  = 0; 					// 清空当前计数值
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

//超声波初始化 TRIG-PB6  ECHO-PE6
void  SR04_Init(void)
{
	GPIO_InitTypeDef   GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOE, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_6;				//引脚编号
	
	//TRIG输出模式
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_OUT;			//输出模式
	GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;			//推挽输出
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_100MHz;		//输出速率
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;			//无上下拉
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//ECHO输入模式
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_IN;				//输入模式
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;			//无上下拉
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	TRIG_RESET;
}

//获取距离
uint32_t SR04_GetDistance(void)
{
	uint32_t cnt=0;
	
	//1.TRIG输出至少10us的高电平
	TRIG_SET;
	Delay_us(15);
	TRIG_RESET;
	
	//2.等待ECHO引脚出现高电平
	while( ECHO_READ == 0 );
	
	//3.计算高电平持续的时间
	while( ECHO_READ == 1 )
	{
		Delay_us(9);
		cnt++;
	}
	
	cnt = cnt/2;
	
	return	cnt*3; //单位是mm

}	




//通过串口发送一个字符串
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


void DHT_SetOutputMode(void)
{
	GPIO_InitTypeDef   GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_OUT;			//输出模式
	GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;			//推挽输出
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_100MHz;		//输出速率
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;			//无上下拉
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_9;				//引脚编号
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOG,GPIO_Pin_9); //总线空闲
}


void DHT_SetInputMode(void)
{
	GPIO_InitTypeDef   GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_IN;				//输入模式
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_UP;			    //上拉电阻
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_9;				//引脚编号
	GPIO_Init(GPIOG, &GPIO_InitStructure);
}


//DHT开始信号
void  DHT_Start(void)
{
	//设置输出模式
	DHT_SetOutputMode();
	
	//把总线拉低  至少18ms
	GPIO_ResetBits(GPIOG,GPIO_Pin_9); 
	Delay_ms(20);
	
	//把总线拉高
	GPIO_SetBits(GPIOG,GPIO_Pin_9);
	Delay_us(30);	

}

//返回1  表示失败   返回0 表示响应
uint32_t DHT_Check(void)
{
	uint32_t i=0;
	//设置输入模式
	DHT_SetInputMode();
	
	//等待PG9出现低电平  设置超时处理
	while( DHT_READ == 1 && i < 100 )
	{
		Delay_us(1);
		i++;
	}
	if(i>=100)
	{
		return 1;
	}
	i=0;
	
	//等待低电平持续80us 设置超时处理
	while( DHT_READ == 0 && i < 100 )
	{
		Delay_us(1);
		i++;
	}
	if(i>=100)
		return 1;  //返回1  表示没响应
	else
		return 0;  //返回0  表示响应了
}

//读取1bit数据   返回1  表示数字1   返回0  表示数字0
uint32_t DHT_ReadBit(void)
{
	uint32_t i=0;
	
	//等待低电平出现  设置超时处理
	while( DHT_READ == 1 && i < 100 )
	{
		Delay_us(1);
		i++;
	}	
	i = 0;
	
	//等待高电平出现  设置超时处理
	while( DHT_READ == 0 && i < 100 )
	{
		Delay_us(1);
		i++;
	}
	
	//延时40us并判断电平的状态
	Delay_us(40);
	if(DHT_READ == 1)
		return 1;  //数字1
	else
		return 0;  //数字0
}

//读取一个字节数据
uint8_t  DHT_ReadByte(void)
{
	uint32_t i=0;
	uint8_t data = 0; //0000 0000
	
	//循环读取数据，并放入变量中  MSB  高位先出  
	for(i=0;i<8;i++)
	{
		data <<= 1;   
		data |= DHT_ReadBit();      
	}
	
	return data;
}

//获取DHT的温湿度数据  返回1  说明失败  返回0  说明成功
uint32_t  DHT_GetData(uint8_t *buf)
{
	uint32_t i=0;
	
	//发送开始信号
	DHT_Start();
	
	//判断DHT是否响应
	if( DHT_Check() == 0 ) //说明响应
	{
		//循环获取5个字节数据
		for(i=0;i<5;i++)
		{
			buf[i] = DHT_ReadByte();
		}
		//对获取的数据进行校验
		if( buf[0]+buf[1]+buf[2]+buf[3] == buf[4] ) //说明校验正确
		{
			return 0; //说明获取数据成功
		}
		else
			return 1; //获取数据失败 因为校验失败
	}
	else
	{
		return 1; //获取数据失败 因为根本没响应
	}

}

void IWDG_Init(void)
{
	//1.判断系统复位是否由看门狗导致
	if( RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET )
	{
		printf("System Reset Is By IWDG\n");
		RCC_ClearFlag();
	}
	else
	{
		printf("System Reset Is By User\n");
	}
		
	//2.解除写保护
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	
	//3.设置喂狗时间  256分配  32KHZ/256 = 125HZ 
	IWDG_SetPrescaler(IWDG_Prescaler_256);
	IWDG_SetReload(125-1);  //喂狗时间 = 1S
	
	//4.重载计数值
	IWDG_ReloadCounter();
	
	//5.启动看门狗
	IWDG_Enable();

}

void WWDG_Init(void)
{
	NVIC_InitTypeDef   NVIC_InitStructure;
	
	//1.判断系统复位是否由WWDG看门狗导致
	if( RCC_GetFlagStatus(RCC_FLAG_WWDGRST) != RESET )
	{
		printf("System Reset Is By WWDG\n");
		RCC_ClearFlag();
	}
	else
	{
		printf("System Reset Is By User\n");
	}

	//2.打开WWDG外设的时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);

	//3.设置WWDG的时钟分频
	//WWDG clock counter = (PCLK1 (42MHz)/4096)/8 = 1281 Hz (~780 us)
	WWDG_SetPrescaler(WWDG_Prescaler_8);

	//4.设置WWDG的窗口上限 该值必须小于128，最大值为127
	WWDG_SetWindowValue(80);
	
	//5.使能WWDG外设并设置计数初值  between 0x40 and 0x7F
	//思考：现在用户的喂狗操作的时间在什么范围之间？  
	// ~780 * (127-80) = 36.6ms < refresh window < ~780 * 64 = 49.9ms  必须在该期间内进行喂狗的动作
	//计数值127  窗口上限为80   127-80 = 47次   36.66ms
	//计数值127  窗口下限为80   127-63 = 64次   49.92ms  (时间的最大值)
	WWDG_Enable(127);
	
	//6.设置WWDG中断的参数  NVIC
	NVIC_InitStructure.NVIC_IRQChannel = WWDG_IRQn;				//中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;		//响应优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能通道
	NVIC_Init(&NVIC_InitStructure);	
	
	//7.清空中断唤醒标志
	WWDG_ClearFlag();
	
	//8.打开WWDG中断唤醒功能
	WWDG_EnableIT();
}

void RTC_Config(void)
{
	RTC_DateTypeDef  RTC_DateStructure;
	RTC_TimeTypeDef  RTC_TimeStructure;
	RTC_InitTypeDef  RTC_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
	EXTI_InitTypeDef  EXTI_InitStructure;

	//1.打开PWR(电源控制器)外设的时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	
	//2.解除RTC寄存器的访问保护  才可以操作数据
	PWR_BackupAccessCmd(ENABLE);
	
	//3.配置低速外部时钟的晶振电路
	RCC_LSEConfig(RCC_LSE_ON);
	
	//4.等待低速外部时钟LSE的晶振电路稳定
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);
	
	//5.选择低速外部时钟LSE作为RTC的时钟源  32.768KHZ
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	
	//6.对低速外部时钟LSE的时钟频率进行分频
	//ck_spre(1Hz) = RTCCLK(LSE) /(uwAsynchPrediv + 1)*(uwSynchPrediv + 1)  --->  32768HZ / 128 / 256 = 1HZ
	RTC_InitStructure.RTC_AsynchPrediv 	= 0x7F;					//异步分频 128分频
	RTC_InitStructure.RTC_SynchPrediv 	= 0xFF;	  				//同步分频 256分频		
	RTC_InitStructure.RTC_HourFormat 	= RTC_HourFormat_24;  	//采用24小时制
	RTC_Init(&RTC_InitStructure);

	//7.使能RTC实时时钟
	RCC_RTCCLKCmd(ENABLE);
	
	//8.确保RTC的寄存器同步
	RTC_WaitForSynchro();
	
	//9.设置RTC的日期  2021年8月25日 星期三  "set date=2021-8-26-4\n"  --> "set date=2021-8-26-4" --> p = strstr(buf,"set date") -->p = strtok(p,"=")-->p = strtok(NULL,"=")
	//p --> "2021-8-26-4" --> p = strtok(p,"-") -->  "2021" --> int year = atoi(p) --> year = year-2000 --> 21 --> 33 --> year =  (year/10)*16 + year%10 --> p = strtok(NULL,"-") 
	RTC_DateStructure.RTC_Year = 0x21;						//年份    0x39+1 -->  BCD码 0x40    HEX码 0x3A
	RTC_DateStructure.RTC_Month = RTC_Month_August;			//月份
	RTC_DateStructure.RTC_Date = 0x25;						//日期
	RTC_DateStructure.RTC_WeekDay = RTC_Weekday_Wednesday;	//星期
	RTC_SetDate(RTC_Format_BCD, &RTC_DateStructure);
	
	//10.设置RTC的时间   17:22:30   "set time=9-27-30\n"   if(atoi(p) < 12) RTC_H12_AM else RTC_H12_PM
	RTC_TimeStructure.RTC_H12     = RTC_H12_PM;				//下午
	RTC_TimeStructure.RTC_Hours   = 0x17;					//时钟
	RTC_TimeStructure.RTC_Minutes = 0x22;					//分钟
	RTC_TimeStructure.RTC_Seconds = 0x30; 					//秒钟
	RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);   
	
	//11.配置EXTI22
	EXTI_ClearITPendingBit(EXTI_Line22);
	EXTI_InitStructure.EXTI_Line = EXTI_Line22;				//外部中断线EXTI22	
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;		//中断模式
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;	//上升沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;				//外部中断线使能
	EXTI_Init(&EXTI_InitStructure);

	//12.配置NVIC
	NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;			//中断唤醒通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			//响应优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能中断通道
	NVIC_Init(&NVIC_InitStructure);
	
	//13.禁止唤醒
	RTC_WakeUpCmd(DISABLE);
	
	//14.设置唤醒时钟源
	RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);
	
	//15.设置唤醒功能自动重载值   周期为1s  1HZ 数1次花费1s
	RTC_SetWakeUpCounter(0);
	
	//16.开启RTC唤醒功能
	RTC_WakeUpCmd(ENABLE);
	
	//17.使能RTC的中断源  周期性唤醒中断
	RTC_ITConfig(RTC_IT_WUT, ENABLE);
	 
	//18.清除唤醒标志
	RTC_ClearITPendingBit(RTC_IT_WUT);
}


//获取flash的扇区号   通过扇区地址进行获取
uint32_t GetSector(uint32_t Address)
{
  uint32_t sector = 0;
  
  if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
  {
    sector = FLASH_Sector_0;  
  }
  else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
  {
    sector = FLASH_Sector_1;  
  }
  else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
  {
    sector = FLASH_Sector_2;  
  }
  else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
  {
    sector = FLASH_Sector_3;  
  }
  else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
  {
    sector = FLASH_Sector_4;  
  }
  else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
  {
    sector = FLASH_Sector_5;  
  }
  else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
  {
    sector = FLASH_Sector_6;  
  }
  else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
  {
    sector = FLASH_Sector_7;  
  }
  else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
  {
    sector = FLASH_Sector_8;  
  }
  else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
  {
    sector = FLASH_Sector_9;  
  }
  else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
  {
    sector = FLASH_Sector_10;  
  }
  
  else/*(Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_11))*/
  {
    sector = FLASH_Sector_11;  
  }

  return sector; //把计算出来的扇区号返回
}

//PA5  ADC1_IN5
void  Res_ADC_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;

	//1.打开ADC外设时钟 +GPIO外设时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	//2.配置GPIO
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_5;				//引脚编号
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AN;				//模拟模式
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;			//无上下拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//3.配置ADC外设的参数
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;						//通道模式  独立模式  只用了一个ADC 
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;						//时钟分频  84MHZ/2 = 42MHZ
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;			//关闭DMA
	ADC_CommonInit(&ADC_CommonInitStructure);
	
	//4.配置ADC的通道的参数
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;							//转换精度  12bit
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;									//关闭扫描模式  单次转换
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;								//打开ADC转换器
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;		//不使用外部触发源  使用内部软件触发
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;							//数据对齐  右对齐
	ADC_InitStructure.ADC_NbrOfConversion = 1;										//单次转换
	ADC_Init(ADC1, &ADC_InitStructure);
	
	//5.指定ADC的通道
	 ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1, ADC_SampleTime_3Cycles);	   

	//6.使能ADC1
	 ADC_Cmd(ADC1, ENABLE);

}

/***************************************
*      从机型号：W25Q128
	   引脚定义
				CS	： PB14
				SCK	： PB3
				MISO： PB4
				MOSI： PB5

****************************************/
void W25Q128_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;
	
	//1.打开SPI1的外设时钟 + GPIO外设时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	
	//2.配置GPIO引脚
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;				//引脚编号
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF;									//复用模式
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;								//无上下拉
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_14;									//引脚编号
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_OUT;								//输出模式
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;								//无上下拉
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//3.空闲状态下CS片选引脚默认高电平 表示不通信
	W25Q128_CS(1);
	
	//4.把GPIO引脚的功能复用为SPI1
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource3, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource4, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource5, GPIO_AF_SPI1);
	
	//5.配置SPI1外设参数
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;				//全双工通信
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;									//主机模式
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;								//数据位数8bit
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;										//时钟极性高
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;									//第二个边沿采集数据
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;										//软件控制CS
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;				//84MHZ/16 = 5.25Mbps W25Q128支持35Mbps
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;								//高位先出
	SPI_Init(SPI1, &SPI_InitStructure);
	
	//6.使能SPI1外设
	SPI_Cmd(SPI1, ENABLE);
}

//通过SPI外设发送一个字节数据 同时会收到一个字节
uint8_t W25Q128_SendByte(uint8_t byte)
{
  /*!< Loop while DR register in not emplty */
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);

  /*!< Send byte through the SPI1 peripheral */
  SPI_I2S_SendData(SPI1, byte);

  /*!< Wait to receive a byte */
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);

  /*!< Return the byte read from the SPI bus */
  return SPI_I2S_ReceiveData(SPI1);
}

uint16_t W25Q128_ReadDeviceId(void)
{
	uint16_t ID = 0;
	//1.把CS片选引脚拉低  表示选中
	W25Q128_CS(0); 
	
	//2.发送指令
	W25Q128_SendByte(0x90);
	
	//3.发送地址  24bit
	W25Q128_SendByte(0x00);
	W25Q128_SendByte(0x00);
	W25Q128_SendByte(0x00);

	//4.接收厂商ID和设备ID  主机发送任意数据即可
	ID  = W25Q128_SendByte(0x00) << 8; 
	ID |= W25Q128_SendByte(0x00);
	
	//5.把CS片选引脚拉高  表示不通信
	W25Q128_CS(1); 
	
	return ID;
}

//使能写入
void W25Q128_WriteEnable(void)
{
	//1.把CS片选引脚拉低  表示选中
	W25Q128_CS(0);
	
	//2.发送指令
	W25Q128_SendByte(0x06);
	
	//3.把CS片选引脚拉高  表示不通信
	W25Q128_CS(1);
}


//禁止写入
void W25Q128_WriteDisable(void)
{
	//1.把CS片选引脚拉低  表示选中
	W25Q128_CS(0);
	
	//2.发送指令
	W25Q128_SendByte(0x04);
	
	//3.把CS片选引脚拉高  表示不通信
	W25Q128_CS(1);
}


uint8_t	W25Q128_ReadStatusRegister1(void)
{
	uint8_t status = 0;
	//1.把CS片选引脚拉低  表示选中
	W25Q128_CS(0);

	//2.发送指令
	W25Q128_SendByte(0x05);
	
	//3.发送任意数据  目的是接收一个字节的返回值
	status = W25Q128_SendByte(0xFF);

	//4.把CS片选引脚拉高  表示不通信
	W25Q128_CS(1);
	
	return status;
}


void W25Q128_SectorErase(uint32_t Address)
{
	//1.开启写使能
	W25Q128_WriteEnable();
	Delay_ms(1);  
	
	//2.把CS片选引脚拉低  表示选中
	W25Q128_CS(0);
	
	//3.发送指令
	W25Q128_SendByte(0x20);
	
	//4.发送地址   数据是高位先出（MSB）
	W25Q128_SendByte( (Address & 0xFF0000)>>16 );
	W25Q128_SendByte( (Address & 0xFF00)>>8 ); 
	W25Q128_SendByte( Address & 0xFF );  

	//5.把CS片选引脚拉高  表示不通信
	W25Q128_CS(1);
	
	//6.等待擦除完成  BUSY位 = 1 表示正在工作  BUSY位=0 表示工作结束
	while( W25Q128_ReadStatusRegister1() & 0x01 );
	
	//7.关闭写使能
	W25Q128_WriteDisable();
}


void W25Q128_ReadData(uint32_t Address,uint8_t *RecvBuf,uint32_t ReadLen)
{
	//1.把CS片选引脚拉低  表示选中
	W25Q128_CS(0);

	//2.发送指令
	W25Q128_SendByte(0x03);
	
	//3.发送地址
	W25Q128_SendByte( (Address & 0xFF0000)>>16 );
	W25Q128_SendByte( (Address & 0xFF00)>>8 ); 
	W25Q128_SendByte( Address & 0xFF );  
	
	//4.接收数据
	while(ReadLen--)
	{
		*RecvBuf++ = W25Q128_SendByte(0xFF);
	}
	
	//5.把CS片选引脚拉高  表示不通信
	W25Q128_CS(1);
}

//扇区写入  最多写入256个字节
void W25Q128_PageProgram(uint32_t Address,uint8_t *RecvBuf,uint32_t ReadLen)
{
	//1.开启写使能
	W25Q128_WriteEnable();
	Delay_ms(1);  
	
	//2.把CS片选引脚拉低  表示选中
	W25Q128_CS(0);
	
	//3.发送指令
	W25Q128_SendByte(0x02);
	
	//4.发送地址   数据是高位先出（MSB）
	W25Q128_SendByte( (Address & 0xFF0000)>>16 );
	W25Q128_SendByte( (Address & 0xFF00)>>8 ); 
	W25Q128_SendByte( Address & 0xFF );  
	
	//5.发送数据
	while(ReadLen--)
	{
		W25Q128_SendByte(*RecvBuf++);
	}
	
	//6.把CS片选引脚拉高  表示不通信
	W25Q128_CS(1);
	
	//7.等待写入完成  BUSY位 = 1 表示忙碌  BUSY位=0 表示空闲
	while( W25Q128_ReadStatusRegister1() & 0x01 );
	
	//8.关闭写使能
	W25Q128_WriteDisable();
}


//IIC总线的初始化  SCL--PB8  SDA--PB9
void IIC_Init(void)
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
	IIC_SCL_WRITE(1);
	IIC_SDA_WRITE(1);
	
}	

void IIC_SDAOutputMode(void)
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

void IIC_SDAInputMode(void)
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
void IIC_StartSignal(void)
{
	//1.设置SDA引脚为输出模式
	IIC_SDAOutputMode();
	
	//2.确保SDA和SCL为高电平
	IIC_SCL_WRITE(1);
	IIC_SDA_WRITE(1);
    Delay_us(5);            //提高程序可靠性
    
	//3.把SDA引脚拉低
	IIC_SDA_WRITE(0);
	Delay_us(5);   // IIC总线的通信速率为100KHZ  1000000us = 100000HZ  10us = 1HZ 
	
	//4.把SCL引脚拉低，表示准备通信
	IIC_SCL_WRITE(0);
}

//IIC总线停止信号
void IIC_StopSignal(void)
{
	//1.设置SDA引脚为输出模式
	IIC_SDAOutputMode();

	//2.确保SDA和SCL为低电平
	IIC_SCL_WRITE(0);
	IIC_SDA_WRITE(0);
	
	//4.把SCL引脚拉高
	IIC_SCL_WRITE(1);
	Delay_us(5);
	
	//5.把SDA引脚拉高
	IIC_SDA_WRITE(1);
	Delay_us(5);  //确保SDA的电平状态可以被其他从器件检测到
}


//主机发送数据（从机读取数据）
void IIC_SendBytes(uint8_t Data)  
{
	uint8_t i= 0;
	
	//1.设置SDA引脚为输出模式
	IIC_SDAOutputMode();
	
	//2.确保SDA和SCL为低电平
	IIC_SCL_WRITE(0);
	IIC_SDA_WRITE(0);
	
	//3.循环发送bit
	for(i=0;i<8;i++)
	{
		//MSB 高位先出   主机准备数据
		if ( Data & 1<<(7-i) ) 
		{
			IIC_SDA_WRITE(1);
		}
		else
			IIC_SDA_WRITE(0);
		
		Delay_us(5);
		
		//SCL为高电平    主机发送数据
		IIC_SCL_WRITE(1);
		Delay_us(5);
		
		//SCL为低电平    主机准备数据
		IIC_SCL_WRITE(0);
		Delay_us(5);
	}
}

//主机读取数据（从机发送数据）
uint8_t IIC_ReadBytes(void)
{
	uint8_t i = 0;
	uint8_t Data = 0; 
	
	//1.设置SDA引脚为输入模式
	IIC_SDAInputMode();
	
	//2.SCL为低电平      从机准备数据
	IIC_SCL_WRITE(0);
	Delay_us(5);
	
	//3.循环接收数据
	for(i=0;i<8;i++)
	{
		//SCL为高电平    从机发送数据
		IIC_SCL_WRITE(1);
		Delay_us(5);
		
		if( IIC_SDA_READ == 1) //说明接收的是1
		{
			Data |= 1<<(7-i);
		}
		
		//SCL为低电平    从机准备数据
		IIC_SCL_WRITE(0);
		Delay_us(5);
	}
	
	return Data;
}

//主机等待从机应答  返回0 说明从机应答   返回1 说明从机没应答
uint8_t IIC_WaitAck(void)
{
	uint8_t ack;
	
	//1.设置SDA引脚为输入模式
	IIC_SDAInputMode();

	//2.SCL为低电平      从机准备数据
	IIC_SCL_WRITE(0);
	Delay_us(5);
	
	//3.SCL为高电平      从机发送数据
	IIC_SCL_WRITE(1);
	Delay_us(5);
	
	//主机判断从机的数据
	if( IIC_SDA_READ == 1)
	{
		ack=1; //说明无应答
	}
	else
		ack=0; //说明已应答
	
	//4.SCL为低电平       主机忽略数据
	IIC_SCL_WRITE(0);
	Delay_us(5);
	
	return ack;
}

//从机发送数据，主机接收数据并发送应答信号
void  IIC_MasterAck(uint8_t ack)
{
	//1.设置SDA引脚为输出模式
	IIC_SDAOutputMode();
	
	//2.确保SDA和SCL为低电平  
	IIC_SCL_WRITE(0);
	IIC_SDA_WRITE(0);
	
	//3.主机准备数据
	if(ack)
		IIC_SDA_WRITE(1);
	else
		IIC_SDA_WRITE(0);
	
	Delay_us(5);
	
	//4.SCL为高电平       主机发送数据
	IIC_SCL_WRITE(1);
	Delay_us(5);
	
	//5.SCL为低电平       从机忽略数据
	IIC_SCL_WRITE(0);
	Delay_us(5);
}




//AT24C02初始化
void AT24C02_Init(void)
{
	IIC_Init();
}

//AT24C02的字节写入  一次写入1字节
void AT24C02_WordWrite(uint8_t Address,uint8_t Data)
{
    //1.主机发送开始信号
    IIC_StartSignal();
    
    //2.主机发送器件地址  写操作
    IIC_SendBytes(0xA0);
    
    //3.主机等待从机应答
    if( IIC_WaitAck() == 1 ) //如果没应答
    {
        printf("AT24C02 Ack Device Address Error\n");
        IIC_StopSignal();  
    }
    //printf("AT24C02 Ack Device Address OK\n");
    
    //4.主机发送存储地址 写操作
    IIC_SendBytes(Address);
    if( IIC_WaitAck() == 1 ) //如果没应答
    {
        printf("AT24C02 Ack Data Address Error\n");
        IIC_StopSignal();  
    }
    //printf("AT24C02 Ack Data Address OK\n");
    
    //5.主机发送存储数据 写操作
     IIC_SendBytes(Data);
    if( IIC_WaitAck() == 1 ) //如果没应答
    {
        printf("AT24C02 Write Data Error\n");
        IIC_StopSignal();  
    }
    //printf("AT24C02 Write Data OK\n");
    
    //6.主机发送停止信号
    IIC_StopSignal(); 
}

//AT24C02读取1字节数据
uint8_t AT24C02_WordRead(uint8_t Address)
{
    uint8_t data = 0;
    
    //1.主机发送开始信号
    IIC_StartSignal();
   
    //2.主机发送器件地址  写操作
    IIC_SendBytes(0xA0);
    
    //3.主机等待从机应答
    if( IIC_WaitAck() == 1 ) //如果没应答
    {
        printf("AT24C02 Ack Device Write Address Error\n");
        IIC_StopSignal();  
    }
    //printf("AT24C02 Ack Device Write Address OK\n"); 

    //4.主机发送存储地址  打算读取数据的地址
    IIC_SendBytes(Address);
    
    //5.主机等待从机应答
    if( IIC_WaitAck() == 1 ) //如果没应答
    {
        printf("AT24C02 Ack Data Address Error\n");
        IIC_StopSignal();  
    }
    //printf("AT24C02 Ack Data Address OK\n");
    
    //6.主机再次发起开始信号
    IIC_StartSignal();
    
    //7.主机发送器件地址  读操作
    IIC_SendBytes(0xA1);
    
    //8.主机等待从机应答
    if( IIC_WaitAck() == 1 ) //如果没应答
    {
        printf("AT24C02 Ack Device Read Address Error\n");
        IIC_StopSignal();  
    }
    //printf("AT24C02 Ack Device Read Address OK\n"); 
    
    //9.主机读取1字节数据
     data = IIC_ReadBytes();
    
    //10.主机发送应答信号
    IIC_MasterAck(1);    //不应答
    
    //11.主机发送停止信号
    IIC_StopSignal(); 
    
    return data;

}

//AT24C02读取n字节数据         存储地址      缓冲数组         数据个数
void AT24C02_RandomRead(uint8_t Address,uint8_t* RecvBuf,uint8_t DataLen)
{
    //1.主机发送开始信号
    IIC_StartSignal();
   
    //2.主机发送器件地址  写操作
    IIC_SendBytes(0xA0);
    
    //3.主机等待从机应答
    if( IIC_WaitAck() == 1 ) //如果没应答
    {
        printf("AT24C02 Ack Device Write Address Error\n");
        IIC_StopSignal();  
    }
    //printf("AT24C02 Ack Device Write Address OK\n"); 

    //4.主机发送存储地址  打算读取数据的地址
    IIC_SendBytes(Address);
    
    //5.主机等待从机应答
    if( IIC_WaitAck() == 1 ) //如果没应答
    {
        printf("AT24C02 Ack Data Address Error\n");
        IIC_StopSignal();  
    }
    //printf("AT24C02 Ack Data Address OK\n");
    
    //6.主机再次发起开始信号
    IIC_StartSignal();
    
    //7.主机发送器件地址  读操作
    IIC_SendBytes(0xA1);
    
    //8.主机等待从机应答
    if( IIC_WaitAck() == 1 ) //如果没应答
    {
        printf("AT24C02 Ack Device Read Address Error\n");
        IIC_StopSignal();  
    }
    //printf("AT24C02 Ack Device Read Address OK\n"); 
    
    //9.主机读取n字节数据
    DataLen = DataLen-1;
    while(DataLen--)                    //读取n-1个
    {
      *RecvBuf++ = IIC_ReadBytes();  
      IIC_MasterAck(0);  //表示主机收到   
    }
    
    //主机接收最后1个字节
    *RecvBuf = IIC_ReadBytes(); 
    
    //10.主机发送应答信号
    IIC_MasterAck(1);    //不应答
    
    //11.主机发送停止信号
    IIC_StopSignal(); 

}

//AT24C02当前地址读   存储地址指的是AT24C02内部地址计数器中的地址  
uint8_t AT24C02_CurrentAddressRead(void)
{
    uint8_t data = 0;
    
    //1.主机发送开始信号
    IIC_StartSignal();
    
    //2.主机发送器件地址  读操作
    IIC_SendBytes(0xA1);
    
    //3.主机等待从机应答
    if( IIC_WaitAck() == 1 ) //如果没应答
    {
        printf("AT24C02 Ack Device Address Error\n");
        IIC_StopSignal();  
    }
    //printf("AT24C02 Ack Device Address OK\n");
    
    //4.主机读取1字节数据
    data = IIC_ReadBytes();
    
    //5.主机发送应答信号
    IIC_MasterAck(1);
    
    //6.主机发送停止信号
    IIC_StopSignal(); 
    
    return data;
}

//AT24C02页写入  一次最多写入8字节
void AT24C02_PageWrite(uint8_t Page_Address,uint8_t *buf,uint8_t DataLen)
{
    //1.主机发送开始信号
    IIC_StartSignal();

    //2.主机发送器件地址  写操作
    IIC_SendBytes(0xA0);
    
    //3.主机等待从机应答
    if( IIC_WaitAck() == 1 ) //如果没应答
    {
        printf("AT24C02 Ack Device Address Error\n");
        IIC_StopSignal();  
    }
    //printf("AT24C02 Ack Device Address OK\n");
    
    //4.主机发送存储地址  页地址
    IIC_SendBytes(Page_Address);
    
    if( IIC_WaitAck() == 1 ) //如果没应答
    {
        printf("AT24C02 Ack Page Address Error\n");
        IIC_StopSignal();  
    }
    //printf("AT24C02 Ack Page Address OK\n");
    
    //5.主机循环发送字节
    while(DataLen--)
    {
        IIC_SendBytes(*buf++);
        if( IIC_WaitAck() == 1 ) //如果没应答
        {
            printf("AT24C02 Write Data Error\n");
            IIC_StopSignal();  
        }   
    }
    
    //6.主机发送停止信号
    IIC_StopSignal(); 
}



int main()
{
    uint8_t recvbuf[10] = {0};
    
    uint8_t data = 0;
	//1.硬件初始化
	USART1_Init(9600);
	SysTick_Init();
	AT24C02_Init();
	
	//2.往AT24C02写入1字节数据  数据地址 0x00
    AT24C02_WordWrite(0x00,'a');
	Delay_ms(100); //等待AT24C02写完  写入1字节需要耗费5ms
    
    //3.随机读
    data = AT24C02_WordRead(0x00);
    printf("Read Data From AT24C02 Is %c \n",data);
    
    //4.主机写入n字节数据 "hello"  0x08指的是第2页的首地址   1~32页
    AT24C02_PageWrite(0x08,(uint8_t *)"hello",5);
    Delay_ms(100); //等待AT24C02写完  写入1字节需要耗费5ms
    
    //5.主机读取n字节数据 0x08 --- 页地址
    AT24C02_RandomRead(0x08,recvbuf,5);
    printf("Read Data From Page Address Is %s \n",recvbuf);
    
    
	//主程序的循环
	while(1)
	{
		
	}
}


//RTC时钟唤醒中断
void RTC_WKUP_IRQHandler(void)
{
	if( RTC_GetITStatus(RTC_IT_WUT) != RESET )
	{
		RTC_WakeUpFlag = 1;  //表示唤醒
		RTC_ClearITPendingBit(RTC_IT_WUT); //清除唤醒标志
	}
	EXTI_ClearITPendingBit(EXTI_Line22); //清除EXTI22线的标志
}



//WWDG中断服务函数
void  WWDG_IRQHandler(void)
{
	if( WWDG_GetFlagStatus() != RESET )
	{
		WWDG_SetCounter(127); 	//执行喂狗
		WWDG_ClearFlag();		//清空中断唤醒标志
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
	uint16_t recv_data = 0;
	//检测中断是否发生
	if( USART_GetITStatus(USART3,USART_IT_RXNE) != RESET )
	{
		USART_ClearITPendingBit(USART3,USART_IT_RXNE); //清除中断标志
		
		recv_data = USART_ReceiveData(USART3); //把接收到的数据保存到变量中
		
		USART_SendData(USART1,recv_data); //把收到的数据发送
		while( USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET ); //等待数据发送完成	
	}
}
