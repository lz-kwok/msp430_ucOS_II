/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                          (c) Copyright 2003-2014; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                            EXAMPLE CODE
*
*                                      Texas Instruments MSP430
*                                               on the
*                                          MSP-EXP430F5259LP
*                                          Evaluation Board
*
* Filename      : g_DeviceFlash.c
* Version       : V1.00
* Programmer(s) : GLZ
*********************************************************************************************************
*/
#include  <bsp.h>
#include "MSP430F5xx_6xx\flashctl.h"

static uint8_t cacheBuf[14];
uint8_t BootFlag[1] = {0x01};
uint8_t Terminal_Info[14]={0xFF};


/*******************************************************************************
* Function Name  : infoFLASH_read
* Description    :
* Input para     : index add
* Output para    : value
*******************************************************************************/
uint8_t infoFLASH_read(uint8_t index,uint32_t add)
{
	uint32_t *Flash_ptrD;
	Flash_ptrD = (uint32_t *)add;
	uint8_t *flash_ptr =  ((uint8_t *)Flash_ptrD) + index;

	return *flash_ptr;
}

/*******************************************************************************
* 函数名      	: infoFLASH_write
* 描述	  		: Flash写操作
* 输入参数  	: *data_ptr--写入数据，*flashAddr--写入地址，count--长度
* 返回参数  	: 无
*******************************************************************************/
void infoFLASH_write(uint8_t *data_ptr,uint8_t *flashAddr,uint16_t count)
{
	FlashCtl_eraseSegment(flashAddr);

	if(FlashCtl_performEraseCheck(flashAddr,count) == 0x01)
	{
		FlashCtl_write8(data_ptr,flashAddr,count);
	}
}
/*******************************************************************************
* 函数名      	: InforFlash_Write
* 描述	  		: Flash写操作
* 输入参数  	: 无
* 返回参数  	: 无
*******************************************************************************/
void InforFlash_Write(unsigned char dat)
{
	char *Flash_ptr;		                     	//Flash指针
	Flash_ptr = (char *) infor_ChargeAddr;			//初始化Flash指针
	FCTL3 = FWKEY;						       		//清除锁定控制位
	FCTL1 = FWKEY+ERASE;				    		//段擦除
	_DINT();										//Flash操作器件不允许任何中断
	*Flash_ptr = 0;									//空写，启动擦除
	while(FCTL3 & BUSY);							//等待擦除操作完成
	FCTL1 = FWKEY+WRT;								//采用字节/字写入模式
	*Flash_ptr = dat;								//写入数据
	while(FCTL3 & BUSY);							//等待写入完成
	_EINT();										//启动全局中断
	FCTL1 = FWKEY;									//Flash退出写模式
	FCTL3 = FWKEY + LOCK;							//恢复Flash锁定位，保护数据

}
/*******************************************************************************
* 函数名      	: FlashRsvWrite
* 描述	  		: Flash指定位置写入指定数据，整页
* 输入参数  	: *data_ptr要写入的数据，indux--写入数据长度，infostartaddr--存储起始地址，num--存储地址偏移位
* 返回参数  	: 无
*******************************************************************************/
void FlashRsvWrite(uint8_t *data_ptr,uint8_t index,uint32_t infostartaddr,uint8_t num)
{
	static uint8_t m;
	uint8_t cacheBuf[128];

	for(m=0;m<128;m++)
	{
		cacheBuf[m] = infoFLASH_read(m,infostartaddr);
	}

	for(m=0;m<index;m++)
		cacheBuf[m+num] = *data_ptr++;

	infoFLASH_write(cacheBuf,(uint8_t *)infostartaddr,128);
}
/*******************************************************************************
* 函数名      	: g_Device_InnerFlash_Init
* 描述	  		: 初始化内部Flash操作
* 输入参数  	: 无
* 返回参数  	: 无
*******************************************************************************/
void g_Device_InnerFlash_Init(void)
{
//	uint8_t TerminalNum = 0x07;

	OSBsp.Device.InnerFlash.innerFLASHRead  = infoFLASH_read;
	OSBsp.Device.InnerFlash.innerFLASHWrite = infoFLASH_write;
	OSBsp.Device.InnerFlash.FlashRsvWrite   = FlashRsvWrite;

	hal_Delay_ms(10);

//	OSBsp.Device.InnerFlash.innerFLASHWrite(&TerminalNum,(uint8_t *)infor_ChargeAddr,1);
}
/*******************************************************************************
* 函数名      	: InforOperate
* 描述	  		: 内部Flash参数配置既读取
* 输入参数  	: data--指令标志位
* 返回参数  	: 无
*******************************************************************************/
void InforOperate(uint8_t data)
{
	static uint8_t i;
	switch(data)
	{
		case 0xFE:         //设置出厂信息（出厂日期、SN、ID）
			cacheBuf[0] = cRxBuff[5];
			cacheBuf[1] = cRxBuff[6];
			cacheBuf[2] = cRxBuff[7];
			cacheBuf[3] = cRxBuff[8];
			cacheBuf[4] = cRxBuff[2];
			cacheBuf[5] = cRxBuff[3];
			cacheBuf[6] = cRxBuff[4];

			FlashRsvWrite(cacheBuf,7,infor_ChargeAddr,0);
			cRxNum=0;
			break;
		case 0xFD:			//软件复位
			hal_Reboot();
			break;
		case 0xFA:         //设置终端类型
			cacheBuf[0] = cRxBuff[2];
			cacheBuf[1] = cRxBuff[3];
			cacheBuf[2] = cRxBuff[4];
			cacheBuf[3] = cRxBuff[5];
			cacheBuf[4] = cRxBuff[6];
			cacheBuf[5] = cRxBuff[7];
			cacheBuf[6] = cRxBuff[8];
			FlashRsvWrite(cacheBuf,7,infor_ChargeAddr,7);
			cRxNum=0;
			break;
		case 0xEF:         //请求升级，进入BootLoader
			{
			loop1:
				FlashRsvWrite(BootFlag,1,infor_BootAddr,0);
				hal_Delay_ms(10);
				if(infoFLASH_read(0,infor_BootAddr) == BootFlag[0])
					hal_Reboot();
				else
					goto loop1;
			}
			break;
		case 0xF0:			//终端信息查询指令
			{
				Terminal_Info[1] = infoFLASH_read(1,infor_BootAddr);
				for(i = 0; i < 11; i ++)
				{
					Terminal_Info[i+2] = infoFLASH_read(i,infor_ChargeAddr);
				}
				Terminal_Info[13]=0xFF;
				OSBsp.Device.Usart2.WriteNData(Terminal_Info,14);
			}
			cRxNum=0;
			break;
		case 0xF5:			//同步时间
			for(i = 1; i < 8; i++)
			{
				time_buf[i]=cRxBuff[i+1];//存“年月日时分秒周”
				cRxBuff[i]=0;
			}
			DS1302_write_time();
			OSBsp.Device.Usart2.WriteString("Time Set Done!\r\n");
			cRxNum=0;
			break;
	}
}


