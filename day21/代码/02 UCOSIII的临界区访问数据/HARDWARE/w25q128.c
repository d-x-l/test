//////////////////////////////////////////////////////////////////////////////////	 
//  ��������  : W25Q128ģ����ʾ����(STM32F407ZET6ϵ��)
//              ˵��: 
//              ----------------------------------------------------------------
//              CS	�� PB14
//				SCK	�� PB3
//				MISO�� PB4
//				MOSI�� PB5
//              ----------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////
#include "w25q128.h"


//��ȡflash��������   ͨ��������ַ���л�ȡ
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

  return sector; //�Ѽ�������������ŷ���
}

/***************************************
*      �ӻ��ͺţ�W25Q128
	   ���Ŷ���
				CS	�� PB14
				SCK	�� PB3
				MISO�� PB4
				MOSI�� PB5

****************************************/
//W25Q128��ʼ��
void W25Q128_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;
	
	//1.��SPI1������ʱ�� + GPIO����ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	
	//2.����GPIO����
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;				//���ű��
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF;									//����ģʽ
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;								//��������
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_14;									//���ű��
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_OUT;								//���ģʽ
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;								//��������
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//3.����״̬��CSƬѡ����Ĭ�ϸߵ�ƽ ��ʾ��ͨ��
	W25Q128_CS(1);
	
	//4.��GPIO���ŵĹ��ܸ���ΪSPI1
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource3, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource4, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource5, GPIO_AF_SPI1);
	
	//5.����SPI1�������
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;				//ȫ˫��ͨ��
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;									//����ģʽ
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;								//����λ��8bit
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;										//ʱ�Ӽ��Ը�
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;									//�ڶ������زɼ�����
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;										//�������CS
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;				//84MHZ/16 = 5.25Mbps W25Q128֧��35Mbps
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;								//��λ�ȳ�
	SPI_Init(SPI1, &SPI_InitStructure);
	
	//6.ʹ��SPI1����
	SPI_Cmd(SPI1, ENABLE);
}

//ͨ��SPI���跢��һ���ֽ����� ͬʱ���յ�һ���ֽ�
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

//W25Q128��ȡ�豸ID
uint16_t W25Q128_ReadDeviceId(void)
{
	uint16_t ID = 0;
	//1.��CSƬѡ��������  ��ʾѡ��
	W25Q128_CS(0); 
	
	//2.����ָ��
	W25Q128_SendByte(0x90);
	
	//3.���͵�ַ  24bit
	W25Q128_SendByte(0x00);
	W25Q128_SendByte(0x00);
	W25Q128_SendByte(0x00);

	//4.���ճ���ID���豸ID  ���������������ݼ���
	ID  = W25Q128_SendByte(0x00) << 8; 
	ID |= W25Q128_SendByte(0x00);
	
	//5.��CSƬѡ��������  ��ʾ��ͨ��
	W25Q128_CS(1); 
	
	return ID;
}

//ʹ��д��
void W25Q128_WriteEnable(void)
{
	//1.��CSƬѡ��������  ��ʾѡ��
	W25Q128_CS(0);
	
	//2.����ָ��
	W25Q128_SendByte(0x06);
	
	//3.��CSƬѡ��������  ��ʾ��ͨ��
	W25Q128_CS(1);
}


//��ֹд��
void W25Q128_WriteDisable(void)
{
	//1.��CSƬѡ��������  ��ʾѡ��
	W25Q128_CS(0);
	
	//2.����ָ��
	W25Q128_SendByte(0x04);
	
	//3.��CSƬѡ��������  ��ʾ��ͨ��
	W25Q128_CS(1);
}

//W25Q128��ȡ״̬�Ĵ���1
uint8_t	W25Q128_ReadStatusRegister1(void)
{
	uint8_t status = 0;
	//1.��CSƬѡ��������  ��ʾѡ��
	W25Q128_CS(0);

	//2.����ָ��
	W25Q128_SendByte(0x05);
	
	//3.������������  Ŀ���ǽ���һ���ֽڵķ���ֵ
	status = W25Q128_SendByte(0xFF);

	//4.��CSƬѡ��������  ��ʾ��ͨ��
	W25Q128_CS(1);
	
	return status;
}

//W25Q128��������
void W25Q128_SectorErase(uint32_t Address)
{
	//1.����дʹ��
	W25Q128_WriteEnable();
	Delay_ms(1);  
	
	//2.��CSƬѡ��������  ��ʾѡ��
	W25Q128_CS(0);
	
	//3.����ָ��
	W25Q128_SendByte(0x20);
	
	//4.���͵�ַ   �����Ǹ�λ�ȳ���MSB��
	W25Q128_SendByte( (Address & 0xFF0000)>>16 );
	W25Q128_SendByte( (Address & 0xFF00)>>8 ); 
	W25Q128_SendByte( Address & 0xFF );  

	//5.��CSƬѡ��������  ��ʾ��ͨ��
	W25Q128_CS(1);
	
	//6.�ȴ��������  BUSYλ = 1 ��ʾ���ڹ���  BUSYλ=0 ��ʾ��������
	while( W25Q128_ReadStatusRegister1() & 0x01 );
	
	//7.�ر�дʹ��
	W25Q128_WriteDisable();
}

//W25Q128��ȡ����
void W25Q128_ReadData(uint32_t Address,uint8_t *RecvBuf,uint32_t ReadLen)
{
	//1.��CSƬѡ��������  ��ʾѡ��
	W25Q128_CS(0);

	//2.����ָ��
	W25Q128_SendByte(0x03);
	
	//3.���͵�ַ
	W25Q128_SendByte( (Address & 0xFF0000)>>16 );
	W25Q128_SendByte( (Address & 0xFF00)>>8 ); 
	W25Q128_SendByte( Address & 0xFF );  
	
	//4.��������
	while(ReadLen--)
	{
		*RecvBuf++ = W25Q128_SendByte(0xFF);
	}
	
	//5.��CSƬѡ��������  ��ʾ��ͨ��
	W25Q128_CS(1);
}

//W25Q128����д��  ���д��256���ֽ�
void W25Q128_PageProgram(uint32_t Address,uint8_t *RecvBuf,uint32_t ReadLen)
{
	//1.����дʹ��
	W25Q128_WriteEnable();
	Delay_ms(1);  
	
	//2.��CSƬѡ��������  ��ʾѡ��
	W25Q128_CS(0);
	
	//3.����ָ��
	W25Q128_SendByte(0x02);
	
	//4.���͵�ַ   �����Ǹ�λ�ȳ���MSB��
	W25Q128_SendByte( (Address & 0xFF0000)>>16 );
	W25Q128_SendByte( (Address & 0xFF00)>>8 ); 
	W25Q128_SendByte( Address & 0xFF );  
	
	//5.��������
	while(ReadLen--)
	{
		W25Q128_SendByte(*RecvBuf++);
	}
	
	//6.��CSƬѡ��������  ��ʾ��ͨ��
	W25Q128_CS(1);
	
	//7.�ȴ�д�����  BUSYλ = 1 ��ʾæµ  BUSYλ=0 ��ʾ����
	while( W25Q128_ReadStatusRegister1() & 0x01 );
	
	//8.�ر�дʹ��
	W25Q128_WriteDisable();
}
