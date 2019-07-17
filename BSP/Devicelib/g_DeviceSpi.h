#ifndef G_DEVICESPI_H
#define G_DEVICESPI_H

#include <stdint.h>

#define SD_CS_High()	P7OUT |= BIT5		//SD_cs put high
#define SD_CS_Low()		P7OUT &=~BIT5		//SD_cs put low Enable
#define CS_1 			P2OUT |= BIT2		//SPI falsh cs put high
#define CS_0 			P2OUT &=~BIT2		//SPI falsh cs put low


void SPIx_Init(void);
unsigned char SPIx_ReadWriteByte(unsigned char TxData);
void g_Device_SPI2_Init(void);
void g_Device_SendByte_SPI2(uint8_t chr);
void g_Device_SendNByte_SPI2(uint8_t *data,uint8_t len);
uint8_t g_Device_SPI2_ReadWriteByte(uint8_t TxData);
void g_Device_SPI3_Init(void);
void g_Device_SendByte_SPI3(uint8_t chr);
void g_Device_SendNByte_SPI3(uint8_t *data,uint8_t len);

#define Spi_WriteByte   g_Device_SPI2_ReadWriteByte
#define Spi_ReadByte()  g_Device_SPI2_ReadWriteByte(0xff)

#endif




