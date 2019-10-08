/*
*********************************************************************************************************
*                                         BOARD SUPPORT PACKAGE
*
*                            (c) Copyright 2014; Micrium, Inc.; Weston, FL
*
*               All rights reserved. Protected by international copyright laws.
*
*               BSP is provided in source form to registered licensees ONLY.  It is
*               illegal to distribute this source code to any third party unless you receive
*               written permission by an authorized Micrium representative.  Knowledge of
*               the source code may NOT be used to develop a similar product.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*
*               You can contact us at www.micrium.com.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                        BOARD SUPPORT PACKAGE
*
*                                 Texas Instruments MSP-EXP430F5529LP
*                                               LaunchPad
*
* Filename      : g_DeviceSPIFlash.h
* Version       : V1.00
* Programmer(s) : HS
*********************************************************************************************************
*/
#ifndef _G_DEVICESPIFLASH_H
#define _G_DEVICESPIFLASH_H



#define PageSize            256
#define PagesinSector       16
#define SectorsinBlock      16

//////W25X16 命令指令表定义
#define WRITE_ENABLE      0X06        //写使能，设置状态寄存器
#define WRITE_DISABLE     0X04        //写禁止
#define READ_STATUSREG    0X05        //读状态寄存器
#define WRITE_STATUSREG   0X01        //写状态寄存器
#define READ_DATE         0X03        //读取存储器数据
#define READ_FAST         0X0B        //快速读取存储器数据
#define READ_DOUBLEFAST   0X3B        //快速双端口输出方式读取存储器数据
#define WRITE_PAGE        0X02        //页面编程--写数据
#define CLEAR_BLOCK       0XD8        //块擦除
#define CLEAR_PAGE        0X20        //扇区擦除
#define CLEAR_SHIP        0XC7        //片擦除
#define POWER_OFF         0XB9        //掉电模式
#define POWER_ON          0XAB        //退出掉电模式、设备ID信息
#define SHIP_ID           0X90        //读取制造厂商ID信息和设备ID信息
#define JEDEC_ID          0X9F        //JEDEC的ID信息

#define FOTA_ADDR_START   0x70000

///////函数声明
extern void Write_Enable(void);
extern void Write_Disable(void);
extern void Read_StatReg(void);
extern void Write_StatReg(uint8_t com);
extern void Erase_Page(long address);
extern uint8_t Read_Byte(long address);
extern void Read_Data(long address,uint8_t Date_Buf[],uint16_t size);
extern void Write_Byte(long address,uint8_t byte);
extern void Write_Date(long address,uint8_t Date_Buf[],uint8_t size);
long g_MTD_spiflash_writeSector(long address,uint8_t Date_Buf[],uint16_t size);
void g_MTD_spiflash_EraseBlock(long block_start);


#endif
