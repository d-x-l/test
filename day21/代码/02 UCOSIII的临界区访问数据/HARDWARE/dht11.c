//////////////////////////////////////////////////////////////////////////////////	 
//  ��������  : DHT11��ʪ��ģ����ʾ����(STM32F407ZET6ϵ��)
//              ˵��: 
//              ----------------------------------------------------------------
//               DQ  ��PG9
//                
//              ----------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////
#include "dht11.h"

//DHT11�����������ģʽ
void DHT_SetOutputMode(void)
{
	GPIO_InitTypeDef   GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_OUT;			//���ģʽ
	GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;			//�������
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_100MHz;		//�������
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;			//��������
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_9;				//���ű��
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOG,GPIO_Pin_9); //���߿���
}

//DHT11������������ģʽ
void DHT_SetInputMode(void)
{
	GPIO_InitTypeDef   GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_IN;				//����ģʽ
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_UP;			    //��������
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_9;				//���ű��
	GPIO_Init(GPIOG, &GPIO_InitStructure);
}


//DHT��ʼ�ź�
void  DHT_Start(void)
{
	//�������ģʽ
	DHT_SetOutputMode();
	
	//����������  ����18ms
	GPIO_ResetBits(GPIOG,GPIO_Pin_9); 
	delay_ms(20);
	
	//����������
	GPIO_SetBits(GPIOG,GPIO_Pin_9);
	delay_us(30);	

}

//DHT11�ȴ���Ӧ ����1  ��ʾʧ��   ����0 ��ʾ��Ӧ
uint32_t DHT_Check(void)
{
	uint32_t i=0;
	//��������ģʽ
	DHT_SetInputMode();
	
	//�ȴ�PG9���ֵ͵�ƽ  ���ó�ʱ����
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
	
	//�ȴ��͵�ƽ����80us ���ó�ʱ����
	while( DHT_READ == 0 && i < 100 )
	{
		delay_us(1);
		i++;
	}
	if(i>=100)
		return 1;  //����1  ��ʾû��Ӧ
	else
		return 0;  //����0  ��ʾ��Ӧ��
}

//DHT11��ȡ1bit����   ����1  ��ʾ����1   ����0  ��ʾ����0
uint32_t DHT_ReadBit(void)
{
	uint32_t i=0;
	
	//�ȴ��͵�ƽ����  ���ó�ʱ����
	while( DHT_READ == 1 && i < 100 )
	{
		delay_us(1);
		i++;
	}	
	i = 0;
	
	//�ȴ��ߵ�ƽ����  ���ó�ʱ����
	while( DHT_READ == 0 && i < 100 )
	{
		delay_us(1);
		i++;
	}
	
	//��ʱ40us���жϵ�ƽ��״̬
	delay_us(40);
	if(DHT_READ == 1)
		return 1;  //����1
	else
		return 0;  //����0
}

//DHT11��ȡһ���ֽ�����
uint8_t  DHT_ReadByte(void)
{
	uint32_t i=0;
	uint8_t data = 0; //0000 0000
	
	//ѭ����ȡ���ݣ������������  MSB  ��λ�ȳ�  
	for(i=0;i<8;i++)
	{
		data <<= 1;   
		data |= DHT_ReadBit();      
	}
	
	return data;
}

//DHT11��ȡ��ʪ������  ����1  ˵��ʧ��  ����0  ˵���ɹ�
uint32_t  DHT_GetData(uint8_t *buf)
{
	uint32_t i=0;
	
	//���Ϳ�ʼ�ź�
	DHT_Start();
	
	//�ж�DHT�Ƿ���Ӧ
	if( DHT_Check() == 0 ) //˵����Ӧ
	{
		//ѭ����ȡ5���ֽ�����
		for(i=0;i<5;i++)
		{
			buf[i] = DHT_ReadByte();
		}
		//�Ի�ȡ�����ݽ���У��
		if( buf[0]+buf[1]+buf[2]+buf[3] == buf[4] ) //˵��У����ȷ
		{
			return 0; //˵����ȡ���ݳɹ�
		}
		else
			return 1; //��ȡ����ʧ�� ��ΪУ��ʧ��
	}
	else
	{
		return 1; //��ȡ����ʧ�� ��Ϊ����û��Ӧ
	}

}
