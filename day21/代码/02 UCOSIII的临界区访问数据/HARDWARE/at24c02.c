//////////////////////////////////////////////////////////////////////////////////	 
//  ��������   : AT24C02оƬ��ʾ����(STM32F407ZET6ϵ��)
//              ˵��: 
//              ----------------------------------------------------------------
//              SCL   ��PB8����
//              SDA   ��PE9����     
//              ----------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////
#include "at24c02.h"


//AT24C02��ʼ��
void AT24C02_IIC_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	//1.��GPIO����ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	//2.����GPIO����
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_8|GPIO_Pin_9;						//���ű��
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_OUT;								//���ģʽ
	GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;								//�������
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_100MHz;							//�������
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;								//��������
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//3.����״̬��SDA��SCL��Ϊ�ߵ�ƽ
	AT24C02_IIC_SCL_WRITE(1);
	AT24C02_IIC_SDA_WRITE(1);
}

//AT24C02оƬSDA�������ģʽ
void AT24C02_SDAOutputMode(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//1.��GPIO����ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	//2.����GPIO����
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_9;									//���ű��
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_OUT;								//���ģʽ
	GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;								//�������
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_100MHz;							//�������
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_UP;									//��������
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

//AT24C02оƬSDA��������ģʽ
void AT24C02_SDAInputMode(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//1.��GPIO����ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	//2.����GPIO����
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_9;									//���ű��
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_IN;									//����ģʽ
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_UP;									//��������
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

//IIC���߿�ʼ�ź�
void AT24C02_IIC_StartSignal(void)
{
	//1.����SDA����Ϊ���ģʽ
	AT24C02_SDAOutputMode();
	
	//2.ȷ��SDA��SCLΪ�ߵ�ƽ
	AT24C02_IIC_SCL_WRITE(1);
	AT24C02_IIC_SDA_WRITE(1);
    Delay_us(5);            //��߳���ɿ���
    
	//3.��SDA��������
	AT24C02_IIC_SDA_WRITE(0);
	Delay_us(5);   // IIC���ߵ�ͨ������Ϊ100KHZ  1000000us = 100000HZ  10us = 1HZ 
	
	//4.��SCL�������ͣ���ʾ׼��ͨ��
	AT24C02_IIC_SCL_WRITE(0);
}

//IIC����ֹͣ�ź�
void AT24C02_IIC_StopSignal(void)
{
	//1.����SDA����Ϊ���ģʽ
	AT24C02_SDAOutputMode();

	//2.ȷ��SDA��SCLΪ�͵�ƽ
	AT24C02_IIC_SCL_WRITE(0);
	AT24C02_IIC_SDA_WRITE(0);
	
	//4.��SCL��������
	AT24C02_IIC_SCL_WRITE(1);
	Delay_us(5);
	
	//5.��SDA��������
	AT24C02_IIC_SDA_WRITE(1);
	Delay_us(5);  //ȷ��SDA�ĵ�ƽ״̬���Ա�������������⵽
}

//�����������ݣ��ӻ���ȡ���ݣ�
void AT24C02_IIC_SendBytes(uint8_t Data)  
{
	uint8_t i= 0;
	
	//1.����SDA����Ϊ���ģʽ
	AT24C02_SDAOutputMode();
	
	//2.ȷ��SDA��SCLΪ�͵�ƽ
	AT24C02_IIC_SCL_WRITE(0);
	AT24C02_IIC_SDA_WRITE(0);
	
	//3.ѭ������bit
	for(i=0;i<8;i++)
	{
		//MSB ��λ�ȳ�   ����׼������
		if ( Data & 1<<(7-i) ) 
		{
			AT24C02_IIC_SDA_WRITE(1);
		}
		else
			AT24C02_IIC_SDA_WRITE(0);
		
		Delay_us(5);
		
		//SCLΪ�ߵ�ƽ    ������������
		AT24C02_IIC_SCL_WRITE(1);
		Delay_us(5);
		
		//SCLΪ�͵�ƽ    ����׼������
		AT24C02_IIC_SCL_WRITE(0);
		Delay_us(5);
	}
}

//������ȡ���ݣ��ӻ��������ݣ�
uint8_t AT24C02_IIC_ReadBytes(void)
{
	uint8_t i = 0;
	uint8_t Data = 0; 
	
	//1.����SDA����Ϊ����ģʽ
	AT24C02_SDAInputMode();
	
	//2.SCLΪ�͵�ƽ      �ӻ�׼������
	AT24C02_IIC_SCL_WRITE(0);
	Delay_us(5);
	
	//3.ѭ����������
	for(i=0;i<8;i++)
	{
		//SCLΪ�ߵ�ƽ    �ӻ���������
		AT24C02_IIC_SCL_WRITE(1);
		Delay_us(5);
		
		if( AT24C02_IIC_SDA_READ == 1) //˵�����յ���1
		{
			Data |= 1<<(7-i);
		}
		
		//SCLΪ�͵�ƽ    �ӻ�׼������
		AT24C02_IIC_SCL_WRITE(0);
		Delay_us(5);
	}
	
	return Data;
}

//�����ȴ��ӻ�Ӧ��  ����0 ˵���ӻ�Ӧ��   ����1 ˵���ӻ�ûӦ��
uint8_t AT24C02_IIC_WaitAck(void)
{
	uint8_t ack;
	
	//1.����SDA����Ϊ����ģʽ
	AT24C02_SDAInputMode();

	//2.SCLΪ�͵�ƽ      �ӻ�׼������
	AT24C02_IIC_SCL_WRITE(0);
	Delay_us(5);
	
	//3.SCLΪ�ߵ�ƽ      �ӻ���������
	AT24C02_IIC_SCL_WRITE(1);
	Delay_us(5);
	
	//�����жϴӻ�������
	if( AT24C02_IIC_SDA_READ == 1)
	{
		ack=1; //˵����Ӧ��
	}
	else
		ack=0; //˵����Ӧ��
	
	//4.SCLΪ�͵�ƽ       ������������
	AT24C02_IIC_SCL_WRITE(0);
	Delay_us(5);
	
	return ack;
}

//�ӻ��������ݣ������������ݲ�����Ӧ���ź�
void  AT24C02_IIC_MasterAck(uint8_t ack)
{
	//1.����SDA����Ϊ���ģʽ
	AT24C02_SDAOutputMode();
	
	//2.ȷ��SDA��SCLΪ�͵�ƽ  
	AT24C02_IIC_SCL_WRITE(0);
	AT24C02_IIC_SDA_WRITE(0);
	
	//3.����׼������
	if(ack)
		AT24C02_IIC_SDA_WRITE(1);
	else
		AT24C02_IIC_SDA_WRITE(0);
	
	Delay_us(5);
	
	//4.SCLΪ�ߵ�ƽ       ������������
	AT24C02_IIC_SCL_WRITE(1);
	Delay_us(5);
	
	//5.SCLΪ�͵�ƽ       �ӻ���������
	AT24C02_IIC_SCL_WRITE(0);
	Delay_us(5);
}


//AT24C02���ֽ�д��  һ��д��1�ֽ�
void AT24C02_WordWrite(uint8_t Address,uint8_t Data)
{
    //1.�������Ϳ�ʼ�ź�
    AT24C02_IIC_StartSignal();
    
    //2.��������������ַ  д����
    AT24C02_IIC_SendBytes(0xA0);
    
    //3.�����ȴ��ӻ�Ӧ��
    if( AT24C02_IIC_WaitAck() == 1 ) //���ûӦ��
    {
        printf("AT24C02 Ack Device Address Error\n");
        AT24C02_IIC_StopSignal();  
    }
    //printf("AT24C02 Ack Device Address OK\n");
    
    //4.�������ʹ洢��ַ д����
    AT24C02_IIC_SendBytes(Address);
    if( AT24C02_IIC_WaitAck() == 1 ) //���ûӦ��
    {
        printf("AT24C02 Ack Data Address Error\n");
        AT24C02_IIC_StopSignal();  
    }
    //printf("AT24C02 Ack Data Address OK\n");
    
    //5.�������ʹ洢���� д����
     AT24C02_IIC_SendBytes(Data);
    if( AT24C02_IIC_WaitAck() == 1 ) //���ûӦ��
    {
        printf("AT24C02 Write Data Error\n");
        AT24C02_IIC_StopSignal();  
    }
    //printf("AT24C02 Write Data OK\n");
    
    //6.��������ֹͣ�ź�
    AT24C02_IIC_StopSignal(); 
}

//AT24C02��ȡ1�ֽ�����
uint8_t AT24C02_WordRead(uint8_t Address)
{
    uint8_t data = 0;
    
    //1.�������Ϳ�ʼ�ź�
    AT24C02_IIC_StartSignal();
   
    //2.��������������ַ  д����
    AT24C02_IIC_SendBytes(0xA0);
    
    //3.�����ȴ��ӻ�Ӧ��
    if( AT24C02_IIC_WaitAck() == 1 ) //���ûӦ��
    {
        printf("AT24C02 Ack Device Write Address Error\n");
        AT24C02_IIC_StopSignal();  
    }
    //printf("AT24C02 Ack Device Write Address OK\n"); 

    //4.�������ʹ洢��ַ  �����ȡ���ݵĵ�ַ
    AT24C02_IIC_SendBytes(Address);
    
    //5.�����ȴ��ӻ�Ӧ��
    if( AT24C02_IIC_WaitAck() == 1 ) //���ûӦ��
    {
        printf("AT24C02 Ack Data Address Error\n");
        AT24C02_IIC_StopSignal();  
    }
    //printf("AT24C02 Ack Data Address OK\n");
    
    //6.�����ٴη���ʼ�ź�
    AT24C02_IIC_StartSignal();
    
    //7.��������������ַ  ������
    AT24C02_IIC_SendBytes(0xA1);
    
    //8.�����ȴ��ӻ�Ӧ��
    if( AT24C02_IIC_WaitAck() == 1 ) //���ûӦ��
    {
        printf("AT24C02 Ack Device Read Address Error\n");
        AT24C02_IIC_StopSignal();  
    }
    //printf("AT24C02 Ack Device Read Address OK\n"); 
    
    //9.������ȡ1�ֽ�����
     data = AT24C02_IIC_ReadBytes();
    
    //10.��������Ӧ���ź�
    AT24C02_IIC_MasterAck(1);    //��Ӧ��
    
    //11.��������ֹͣ�ź�
    AT24C02_IIC_StopSignal(); 
    
    return data;

}

//AT24C02��ȡn�ֽ�����         �洢��ַ      ��������         ���ݸ���
void AT24C02_RandomRead(uint8_t Address,uint8_t* RecvBuf,uint8_t DataLen)
{
    //1.�������Ϳ�ʼ�ź�
    AT24C02_IIC_StartSignal();
   
    //2.��������������ַ  д����
    AT24C02_IIC_SendBytes(0xA0);
    
    //3.�����ȴ��ӻ�Ӧ��
    if( AT24C02_IIC_WaitAck() == 1 ) //���ûӦ��
    {
        printf("AT24C02 Ack Device Write Address Error\n");
        AT24C02_IIC_StopSignal();  
    }
    //printf("AT24C02 Ack Device Write Address OK\n"); 

    //4.�������ʹ洢��ַ  �����ȡ���ݵĵ�ַ
    AT24C02_IIC_SendBytes(Address);
    
    //5.�����ȴ��ӻ�Ӧ��
    if( AT24C02_IIC_WaitAck() == 1 ) //���ûӦ��
    {
        printf("AT24C02 Ack Data Address Error\n");
        AT24C02_IIC_StopSignal();  
    }
    //printf("AT24C02 Ack Data Address OK\n");
    
    //6.�����ٴη���ʼ�ź�
    AT24C02_IIC_StartSignal();
    
    //7.��������������ַ  ������
    AT24C02_IIC_SendBytes(0xA1);
    
    //8.�����ȴ��ӻ�Ӧ��
    if( AT24C02_IIC_WaitAck() == 1 ) //���ûӦ��
    {
        printf("AT24C02 Ack Device Read Address Error\n");
        AT24C02_IIC_StopSignal();  
    }
    //printf("AT24C02 Ack Device Read Address OK\n"); 
    
    //9.������ȡn�ֽ�����
    DataLen = DataLen-1;
    while(DataLen--)                    //��ȡn-1��
    {
      *RecvBuf++ = AT24C02_IIC_ReadBytes();  
      AT24C02_IIC_MasterAck(0);  //��ʾ�����յ�   
    }
    
    //�����������1���ֽ�
    *RecvBuf = AT24C02_IIC_ReadBytes(); 
    
    //10.��������Ӧ���ź�
    AT24C02_IIC_MasterAck(1);    //��Ӧ��
    
    //11.��������ֹͣ�ź�
    AT24C02_IIC_StopSignal(); 

}

//AT24C02��ǰ��ַ��   �洢��ַָ����AT24C02�ڲ���ַ�������еĵ�ַ  
uint8_t AT24C02_CurrentAddressRead(void)
{
    uint8_t data = 0;
    
    //1.�������Ϳ�ʼ�ź�
    AT24C02_IIC_StartSignal();
    
    //2.��������������ַ  ������
    AT24C02_IIC_SendBytes(0xA1);
    
    //3.�����ȴ��ӻ�Ӧ��
    if( AT24C02_IIC_WaitAck() == 1 ) //���ûӦ��
    {
        printf("AT24C02 Ack Device Address Error\n");
        AT24C02_IIC_StopSignal();  
    }
    //printf("AT24C02 Ack Device Address OK\n");
    
    //4.������ȡ1�ֽ�����
    data = AT24C02_IIC_ReadBytes();
    
    //5.��������Ӧ���ź�
    AT24C02_IIC_MasterAck(1);
    
    //6.��������ֹͣ�ź�
    AT24C02_IIC_StopSignal(); 
    
    return data;
}

//AT24C02ҳд��  һ�����д��8�ֽ�
void AT24C02_PageWrite(uint8_t Page_Address,uint8_t *buf,uint8_t DataLen)
{
    //1.�������Ϳ�ʼ�ź�
    AT24C02_IIC_StartSignal();

    //2.��������������ַ  д����
    AT24C02_IIC_SendBytes(0xA0);
    
    //3.�����ȴ��ӻ�Ӧ��
    if( AT24C02_IIC_WaitAck() == 1 ) //���ûӦ��
    {
        printf("AT24C02 Ack Device Address Error\n");
        AT24C02_IIC_StopSignal();  
    }
    //printf("AT24C02 Ack Device Address OK\n");
    
    //4.�������ʹ洢��ַ  ҳ��ַ
    AT24C02_IIC_SendBytes(Page_Address);
    
    if( AT24C02_IIC_WaitAck() == 1 ) //���ûӦ��
    {
        printf("AT24C02 Ack Page Address Error\n");
        AT24C02_IIC_StopSignal();  
    }
    //printf("AT24C02 Ack Page Address OK\n");
    
    //5.����ѭ�������ֽ�
    while(DataLen--)
    {
        AT24C02_IIC_SendBytes(*buf++);
        if( AT24C02_IIC_WaitAck() == 1 ) //���ûӦ��
        {
            printf("AT24C02 Write Data Error\n");
            AT24C02_IIC_StopSignal();  
        }   
    }
    
    //6.��������ֹͣ�ź�
    AT24C02_IIC_StopSignal(); 
}
