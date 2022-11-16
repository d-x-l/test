#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "includes.h"
#include "dht11.h"

static GPIO_InitTypeDef  	GPIO_InitStructure;

static void dht11_outputmode(void)
{
	//GPIOG9初始化设置 
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9 ;					//第9号引脚
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;			    	//普通输出模式，
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;					//推挽输出，驱动LED需要电流驱动
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		    	//100MHz
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;				    //上拉
	GPIO_Init(GPIOG, &GPIO_InitStructure);							//初始化GPIOG，把配置的数据写入寄存器		


}

static void dht11_inputmode(void)
{
	//GPIOG9初始化设置 
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9 ;					//第9号引脚
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;			    	//普通输入模式，
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		    	//100MHz
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;				    //上拉
	GPIO_Init(GPIOG, &GPIO_InitStructure);							//初始化GPIOG，把配置的数据写入寄存器	
}

static uint32_t dht11_start(void)
{
	uint32_t i=0;
	//输出模式
	dht11_outputmode();
	
	//PG9引脚输出低电平
	PGout(9)=0;
	
	//延时20ms
	delay_ms(20);
	
	//PG9引脚输出高电平
	PGout(9)=1;	
	
	//延时30us
	delay_us(30);	
	
	//输入模式
	dht11_inputmode();	
	
	//检测是否有低电平
	i=0;
	while(i<100)
	{
		if(PGin(9)==0)
			break;
		delay_us(1);
		i++;
	}
	
	//超时
	if(i >=100)
		return 1;
	
	
	//检测是否有高电平
	i=0;
	while(i<100)
	{
		if(PGin(9))
			break;
		delay_us(1);
		i++;
	}
	
	//超时
	if(i >=100)
		return 1;
	
	//没有错误，正确的返回 
	return 0;
}

static uint8_t dht11_read_byte(void)
{
	uint8_t d=0;
	
	uint32_t i=0;
	
	//等待高电平持续完毕
	while(PGin(9));
	
	for(i=0; i<8; i++)
	{
		//检测低电平
		while(PGin(9)==0);
		
		//延时40us
		delay_us(40);
		
		//判断PG9引脚的电平,若为高电平就是bit1
		if(PGin(9))
		{
			d|=1<<(7-i);
			
			//等待高电平持续完毕
			while(PGin(9));		
		}
	}
		
	return d;
}

void dht11_init(void)
{
	//使能GPIOG的硬件时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
}

uint32_t dht11_read_data(uint8_t *pbuf)
{
	uint32_t i=0;
	uint8_t  check_sum=0;
	
	//建立启动信号
	while(dht11_start()==1);
	
	//连续读取5个字节
	for(i=0; i<5; i++)
	{
		pbuf[i] = dht11_read_byte();
	
	}

	//计算校验和
	check_sum = pbuf[0]+pbuf[1]+pbuf[2]+pbuf[3];
	
	//检验和判断
	if(check_sum != pbuf[4])
		return 1;
	
	return 0;
}
