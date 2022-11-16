#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "includes.h"
#include "dht11.h"

static GPIO_InitTypeDef  	GPIO_InitStructure;

static void dht11_outputmode(void)
{
	//GPIOG9��ʼ������ 
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9 ;					//��9������
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;			    	//��ͨ���ģʽ��
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;					//�������������LED��Ҫ��������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		    	//100MHz
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;				    //����
	GPIO_Init(GPIOG, &GPIO_InitStructure);							//��ʼ��GPIOG�������õ�����д��Ĵ���		


}

static void dht11_inputmode(void)
{
	//GPIOG9��ʼ������ 
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9 ;					//��9������
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;			    	//��ͨ����ģʽ��
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		    	//100MHz
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;				    //����
	GPIO_Init(GPIOG, &GPIO_InitStructure);							//��ʼ��GPIOG�������õ�����д��Ĵ���	
}

static uint32_t dht11_start(void)
{
	uint32_t i=0;
	//���ģʽ
	dht11_outputmode();
	
	//PG9��������͵�ƽ
	PGout(9)=0;
	
	//��ʱ20ms
	delay_ms(20);
	
	//PG9��������ߵ�ƽ
	PGout(9)=1;	
	
	//��ʱ30us
	delay_us(30);	
	
	//����ģʽ
	dht11_inputmode();	
	
	//����Ƿ��е͵�ƽ
	i=0;
	while(i<100)
	{
		if(PGin(9)==0)
			break;
		delay_us(1);
		i++;
	}
	
	//��ʱ
	if(i >=100)
		return 1;
	
	
	//����Ƿ��иߵ�ƽ
	i=0;
	while(i<100)
	{
		if(PGin(9))
			break;
		delay_us(1);
		i++;
	}
	
	//��ʱ
	if(i >=100)
		return 1;
	
	//û�д�����ȷ�ķ��� 
	return 0;
}

static uint8_t dht11_read_byte(void)
{
	uint8_t d=0;
	
	uint32_t i=0;
	
	//�ȴ��ߵ�ƽ�������
	while(PGin(9));
	
	for(i=0; i<8; i++)
	{
		//���͵�ƽ
		while(PGin(9)==0);
		
		//��ʱ40us
		delay_us(40);
		
		//�ж�PG9���ŵĵ�ƽ,��Ϊ�ߵ�ƽ����bit1
		if(PGin(9))
		{
			d|=1<<(7-i);
			
			//�ȴ��ߵ�ƽ�������
			while(PGin(9));		
		}
	}
		
	return d;
}

void dht11_init(void)
{
	//ʹ��GPIOG��Ӳ��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
}

uint32_t dht11_read_data(uint8_t *pbuf)
{
	uint32_t i=0;
	uint8_t  check_sum=0;
	
	//���������ź�
	while(dht11_start()==1);
	
	//������ȡ5���ֽ�
	for(i=0; i<5; i++)
	{
		pbuf[i] = dht11_read_byte();
	
	}

	//����У���
	check_sum = pbuf[0]+pbuf[1]+pbuf[2]+pbuf[3];
	
	//������ж�
	if(check_sum != pbuf[4])
		return 1;
	
	return 0;
}
