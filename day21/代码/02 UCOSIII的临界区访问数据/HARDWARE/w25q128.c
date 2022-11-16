//////////////////////////////////////////////////////////////////////////////////	 
//  功能描述  : W25Q128模块演示例程(STM32F407ZET6系列)
//              说明: 
//              ----------------------------------------------------------------
//              CS	： PB14
//				SCK	： PB3
//				MISO： PB4
//				MOSI： PB5
//              ----------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////
#include "w25q128.h"


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

/***************************************
*      从机型号：W25Q128
	   引脚定义
				CS	： PB14
				SCK	： PB3
				MISO： PB4
				MOSI： PB5

****************************************/
//W25Q128初始化
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

//W25Q128读取设备ID
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

//W25Q128读取状态寄存器1
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

//W25Q128擦除扇区
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

//W25Q128读取数据
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

//W25Q128扇区写入  最多写入256个字节
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
