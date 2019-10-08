/****************SPI_Flash.c*************************************/
#include <bsp.h>



//芯片写使能
static void Write_Enable(void)
{
  SD_CS_Low();
  hal_Delay_us(20);
  CS_0;  
  hal_Delay_us(10);
  OSBsp.Device.Spi2.WriteReadData(WRITE_ENABLE);  
  hal_Delay_us(10);
  CS_1;
}

///芯片禁止写入
static void Write_Disable(void)
{
  SD_CS_Low();
  hal_Delay_us(20);
  CS_0;  
  hal_Delay_us(10);
  OSBsp.Device.Spi2.WriteReadData(WRITE_DISABLE);
  hal_Delay_us(10);
  CS_1;
}

//读取芯片状态寄存器
static uint8_t Read_StatReg(void)
{
  uint32_t temp;
  CS_1;
  SD_CS_High();
  hal_Delay_us(20);
  CS_0;  
  hal_Delay_us(20);
  OSBsp.Device.Spi2.WriteReadData(READ_STATUSREG);
  temp = OSBsp.Device.Spi2.WriteReadData(0xff);
  hal_Delay_us(20);
  g_Printf_info("READ_STATUSREG = %02x\r\n",temp);
  CS_1;  
  return temp;
}

//写状态寄存器
void Write_StatReg(uint8_t com)
{
  SD_CS_Low();
  hal_Delay_us(20);
  CS_0;  
  hal_Delay_us(10);
  OSBsp.Device.Spi2.WriteReadData(WRITE_STATUSREG);  
  OSBsp.Device.Spi2.WriteReadData(com);  
  hal_Delay_us(10);
  CS_1;
}

void Erase_Page(long address)
{
  SD_CS_Low();
  hal_Delay_us(20);
  uint8_t H,M,L;
  H = address>>16;  
  M = address>>8;  
  L = address&0xff;  
  Write_Enable(); //��ִ��дʹ��
  CS_0;  
  hal_Delay_us(10);
  OSBsp.Device.Spi2.WriteReadData(CLEAR_PAGE);  
  OSBsp.Device.Spi2.WriteReadData(H);  
  OSBsp.Device.Spi2.WriteReadData(M);  
  OSBsp.Device.Spi2.WriteReadData(L); 
  hal_Delay_us(10);
  CS_1;
}

//在任意地址写入一个字节
void Write_Byte(long address,uint8_t byte)

{
  SD_CS_Low();
  hal_Delay_us(20);
  uint8_t H,M,L;
  H = address>>16;  
  M = address>>8; 
  L = address&0xff;  
  CS_0;  
  hal_Delay_us(10);
  OSBsp.Device.Spi2.WriteReadData(WRITE_PAGE);  
  OSBsp.Device.Spi2.WriteReadData(H);  
  OSBsp.Device.Spi2.WriteReadData(M);  
  OSBsp.Device.Spi2.WriteReadData(L);  
  OSBsp.Device.Spi2.WriteReadData(byte);
  hal_Delay_us(10);
  CS_1;
}

//在任意地址开始写入一个数据包（最大长度不超过256个字节）
void Write_Date(long address,uint8_t Date_Buf[],uint8_t size)
{  
  SD_CS_Low();
  hal_Delay_us(20);
  uint8_t i;
  uint8_t H,M,L;
  H = address>>16;  
  M = address>>8;  
  L = address&0xff;  
  CS_0;  
  hal_Delay_us(10);
  OSBsp.Device.Spi2.WriteReadData(WRITE_PAGE);  
  OSBsp.Device.Spi2.WriteReadData(H);  
  OSBsp.Device.Spi2.WriteReadData(M);  
  OSBsp.Device.Spi2.WriteReadData(L);
  for(i=0;i<size;i++)  
  {  
    OSBsp.Device.Spi2.WriteReadData(Date_Buf[i]);
  }  
  hal_Delay_us(10);
  CS_1;
}

//在任意地址读出一个字节
uint8_t Read_Byte(long address)
{
  SD_CS_Low();
  hal_Delay_us(20);
  uint8_t temp;
  uint8_t H,M,L;
  H = address>>16; 
  M = address>>8;  
  L = address&0xff; 
  CS_0;  
  hal_Delay_us(10);
  OSBsp.Device.Spi2.WriteReadData(READ_DATE);  
  OSBsp.Device.Spi2.WriteReadData(H);  
  OSBsp.Device.Spi2.WriteReadData(M);  
  OSBsp.Device.Spi2.WriteReadData(L);  
  temp = OSBsp.Device.Spi2.WriteReadData(0xff);  
  hal_Delay_us(10);
  CS_1;  
  return temp;
}

//从任意地址开始读出数据
void Read_Data(long address,uint8_t Date_Buf[],uint16_t size)
{
  SD_CS_Low();
  hal_Delay_us(20);
  int i;
  uint8_t H,M,L;
  H = address>>16;  
  M = address>>8;  
  L = address&0xff;  
  CS_0;  
  hal_Delay_us(10);
  OSBsp.Device.Spi2.WriteReadData(READ_DATE);  
  OSBsp.Device.Spi2.WriteReadData(H);  
  OSBsp.Device.Spi2.WriteReadData(M); 
  OSBsp.Device.Spi2.WriteReadData(L); 
  for(i=0;i<size;i++) 
  {  
    Date_Buf[i] = OSBsp.Device.Spi2.WriteReadData(0xff);  
    // g_Printf_info("%s",&Date_Buf[i]);
    OSBsp.Device.Usart2.WriteData(Date_Buf[i]);
  }  
  hal_Delay_us(10);
  CS_1;
}

long g_MTD_spiflash_writeSector(long address,uint8_t Date_Buf[],uint16_t size)
{
  static int m = 0;
  uint16_t size_to_write = 256;
  uint16_t remain_size = size;
  long addr = address;
  while(Read_StatReg()&0x01);
	Write_Enable();

  for(m=0;m<PagesinSector;m++){
    if(remain_size <= PageSize){
      Write_Date(addr,&Date_Buf[size_to_write],size);
      addr += size;
      break;
    }else{
      Write_Date(addr,&Date_Buf[size_to_write],PageSize);
      remain_size -= PageSize;
      addr += PageSize;
      size_to_write += PageSize;
    }
  }
  Write_Disable();

  return addr;
}

void g_MTD_spiflash_EraseBlock(long block_start)
{
  static int m;
  long addr = block_start;
  for(m=0;m<256;m++){
    Erase_Page(addr);
    addr += PageSize;
  }
}

void g_MTD_spiflash_readSector(long address,uint8_t *Date_Buf)
{
  static int m = 0;
  uint16_t size_to_write = 256;
  long addr = address;

  for(m=0;m<PagesinSector;m++){

  }
}
