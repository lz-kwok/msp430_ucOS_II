// (c)2009 by Texas Instruments Incorporated, All Rights Reserved.
/*----------------------------------------------------------------------------+
|                                                                             |
|                              Texas Instruments                              |
|                                                                             |
|                          MSP430 USB-Example (CDC/HID Driver)                |
|                                                                             |
+-----------------------------------------------------------------------------+
|  Source: device.h, File Version 1.00 2009/12/03                             |
|  Author: RSTO                                                               |
|                                                                             |
|  Description:                                                               |
|  This file is included in other source code files                           |
|  and this only one place to change the included device header               |
+----------------------------------------------------------------------------*/
#ifndef  HAL_DEVICE_H
#define  HAL_DEVICE_H

#include <msp430.h>


#if defined (__MSP430F6638) || defined (__MSP430F6637) || defined (__MSP430F6636) || \
	defined (__MSP430F6635) || defined (__MSP430F6634) || defined (__MSP430F6633) || \
	defined (__MSP430F6632) || defined (__MSP430F6631) || defined (__MSP430F6630) || \
	defined (__MSP430F5638) || defined (__MSP430F5637) || defined (__MSP430F5636) || \
	defined (__MSP430F5635) || defined (__MSP430F5634) || defined (__MSP430F5633) || \
	defined (__MSP430F5632) || defined (__MSP430F5631) || defined (__MSP430F5630) 
	#define __MSP430F563x_F663x
#elif defined (__MSP430F5510) || defined (__MSP430F5509) || defined (__MSP430F5508) || \
	defined (__MSP430F5507) || defined (__MSP430F5506) || defined (__MSP430F5505) || \
	defined (__MSP430F5504) || defined (__MSP430F5503) || defined (__MSP430F5502) || \
	defined (__MSP430F5501) || defined (__MSP430F5500)
    #define __MSP430F550x
#elif defined (__MSP430F5529) || defined (__MSP430F5528) || defined (__MSP430F5527) || \
	defined (__MSP430F5526) || defined (__MSP430F5525) || defined (__MSP430F5524) || \
	defined (__MSP430F5522) || defined (__MSP430F5521) || defined (__MSP430F5519) || \
	defined (__MSP430F5517) || defined (__MSP430F5515) || defined (__MSP430F5514) || \
	defined (__MSP430F5513)
    #define __MSP430F552x
#elif defined (__MSP430F5259__)
    #define __MSP430F525x
#else
    #error Define a constant of format __MSP430Fxxxx__ within the projects preprocessor settings,
    according to the device being used.
#endif

/*------------------------ Nothing Below This Line --------------------------*/
#define SCADATIME					60
// 定义是否具备GPS功能
#define HAVE_GPS_SERVICE
// 定义是否具备蓝牙功能
#define HAVE_BT_SERVICE
// 定义是否具备log存储功能
#define HAVE_LOG_STORE_SERVICE
//*****************终端类型*********************//
#define Air_Station           		0x01	 //空气监测�?
#define Voc_Station           		0x02	 //VOC监测�?
#define Dust_Station	      		0x03	 //扬尘噪音监测�?
#define WRain_Station	      		0x04	 //水雨情监测仪
#define Wether_Station        		0x05	 //气象监测�?
#define Water_Station	      		0x06	 //水质水况监测�?
#define Soil_Station          		0x07	 //土壤墒情监测�?
#define Agriculture_Station   		0x08	 //农业监测�?
#define Flowmeter_Station     		0x09	 //流量计监测仪

#define Seeper_Station        		0x11	 //易涝点液位监测仪
#define Flow_Station          		0x12	 //窨井管道流量监测�?
#define Rain_Station          		0x13	 //雨量监测�?
#define IntegratedPitWell     		0x14	 //一体化窨井液位监测�?
#define InputmodeWell         		0x15	 //投入式窨井液位监测仪

#define NoxiousGas_Station    		0x18	 //有害气体监测�?
#define WetherSoil_Station    		0x19	 //气象土壤监测�?
#define Planting_Station      		0x20	 //农作物种植环境监测仪

#define LevelFlowrate_Station 		0x21	 //雷达水位流速监测仪

#define TankMonitor_Station   		0x0A	 //TankMonitor监测�?
#define MagicSTICK_Station    		0x0B	 //MagicSTICK监测�?
#define AliveNest_Station     		0x0C	 //AliveNest监测�?     一体化空气检测仪
#define CLCupboard_Station    		0x0D	 //氯离子柜式监测站

#define Custom_Station	      		0x55	 //定制需求监测站


#define PRODUCT_TYPE                Dust_Station


//*****************通信方式*********************//
#define GPRS_Mode		      		0x01
#define NBIoT_BC95_Mode	      		0x02
#define NBIoT_M5310_Mode	  		0x03
#define LoRa_F8L10D_Mode	  		0x04
#define LoRa_S78S_Mode		  		0x05
#define LoRa_OM402_Mode		  		0x06
#define GPRS_AIR202_Mode      		0x07
#define TRANSMIT_TYPE 			    NBIoT_BC95_Mode

#if (TRANSMIT_TYPE == GPRS_Mode)
	//这俩预定义只能选一�?
	//#define SIM800C
	#define AIR202
	//这俩预定义只能选一�?
	#define ReportToAliOnly
	// #define ReportTo30000IoT
#endif

//*****************配件方式*********************//
#define GPS_Mode		      		0x01     //GPS配件
#define RS485_Mode		      		0x02     //485配件
#define RS232_Mode		      		0x03     //232配件
#define None_Mode	          		0xFF     //无配�?
#define ACCESSORY_TYPR              GPS_Mode




#endif
