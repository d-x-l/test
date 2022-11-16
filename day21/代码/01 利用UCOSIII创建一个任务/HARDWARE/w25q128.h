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
#ifndef _W25Q128_H
#define _W25Q128_H

#include "stm32f4xx.h"
#include "systick.h"

//W25Q128��SCKʱ������
#define  W25Q128_CS(n)		(n)?GPIO_SetBits(GPIOB,GPIO_Pin_14):GPIO_ResetBits(GPIOB,GPIO_Pin_14)	


#define FLASH_USER_START_ADDR   ADDR_FLASH_SECTOR_6		//����6�Ŀ�ʼ��ַ
#define FLASH_USER_END_ADDR     ADDR_FLASH_SECTOR_7     //����7�Ŀ�ʼ��ַ


/* flash�����Ļ���ַ */ 
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

uint32_t uwStartSector = 0; 	//��������6��������
uint32_t uwEndSector   = 0; 	//��������7��������
uint32_t uwAddress = 0;			
uint32_t uwSectorCounter = 0; 	//���浱ǰ��������

__IO uint32_t uwData32 = 0;
__IO uint32_t uwMemoryProgramStatus = 0;


//��ȡflash��������   ͨ��������ַ���л�ȡ
uint32_t GetSector(uint32_t Address);

//W25Q128��ʼ��
void W25Q128_Init(void);

//ͨ��SPI���跢��һ���ֽ����� ͬʱ���յ�һ���ֽ�
uint8_t W25Q128_SendByte(uint8_t byte);

//W25Q128��ȡ�豸ID
uint16_t W25Q128_ReadDeviceId(void);

//ʹ��д��
void W25Q128_WriteEnable(void);

//��ֹд��
void W25Q128_WriteDisable(void);

//W25Q128��ȡ״̬�Ĵ���1
uint8_t	W25Q128_ReadStatusRegister1(void);

//W25Q128��������
void W25Q128_SectorErase(uint32_t Address);

//W25Q128��ȡ����
void W25Q128_ReadData(uint32_t Address,uint8_t *RecvBuf,uint32_t ReadLen);

//W25Q128����д��  ���д��256���ֽ�
void W25Q128_PageProgram(uint32_t Address,uint8_t *RecvBuf,uint32_t ReadLen);

#endif
