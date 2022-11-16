//////////////////////////////////////////////////////////////////////////////////	 
//  功能描述  : DHT11温湿度模块演示例程(STM32F407ZET6系列)
//              说明: 
//              ----------------------------------------------------------------
//               DQ  接PG9
//                
//              ----------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////
#include "dht11.h"

//DHT11设置引脚输出模式
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

//DHT11设置引脚输入模式
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
	delay_ms(20);
	
	//把总线拉高
	GPIO_SetBits(GPIOG,GPIO_Pin_9);
	delay_us(30);	

}

//DHT11等待响应 返回1  表示失败   返回0 表示响应
uint32_t DHT_Check(void)
{
	uint32_t i=0;
	//设置输入模式
	DHT_SetInputMode();
	
	//等待PG9出现低电平  设置超时处理
	while( DHT_READ == 1 && i < 100 )
	{
		delay_us(1);
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
		delay_us(1);
		i++;
	}
	if(i>=100)
		return 1;  //返回1  表示没响应
	else
		return 0;  //返回0  表示响应了
}

//DHT11读取1bit数据   返回1  表示数字1   返回0  表示数字0
uint32_t DHT_ReadBit(void)
{
	uint32_t i=0;
	
	//等待低电平出现  设置超时处理
	while( DHT_READ == 1 && i < 100 )
	{
		delay_us(1);
		i++;
	}	
	i = 0;
	
	//等待高电平出现  设置超时处理
	while( DHT_READ == 0 && i < 100 )
	{
		delay_us(1);
		i++;
	}
	
	//延时40us并判断电平的状态
	delay_us(40);
	if(DHT_READ == 1)
		return 1;  //数字1
	else
		return 0;  //数字0
}

//DHT11读取一个字节数据
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

//DHT11获取温湿度数据  返回1  说明失败  返回0  说明成功
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
