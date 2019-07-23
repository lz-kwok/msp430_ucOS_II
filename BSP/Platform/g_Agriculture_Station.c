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
* Filename      : g_WRain_Station.c
* Version       : V1.00
* Programmer(s) : GLZ
*********************************************************************************************************
*/
#include  <hal_layer_api.h>
#include  <bsp.h>

#if (PRODUCT_TYPE == Agriculture_Station)

#define SensorNum		6
#define CMDLength       8

AppStruct  App;
DataStruct *AppDataPointer;


uint8_t Inqure_TempHumidity[Command_1Length]={0x03,0x03,0x00,0x00,0x00,0x02,0xC5,0xE9};       //温湿度
uint8_t Inqure_Illumination[Command_1Length]={0x07,0x03,0x00,0x00,0x00,0x02,0xC4,0x6D};       //光照
uint8_t Inqure_WaterPH[Command_1Length]={0x0A,0x03,0x00,0x00,0x00,0x04,0x45,0x72};            //水质PH
uint8_t Inqure_WaterTemp[Command_1Length]={0x0B,0x03,0x00,0x28,0x00,0x03,0x85,0x69};          //水温
uint8_t Inqure_SoilPH[Command_1Length]={0x0D,0x03,0x00,0x00,0x00,0x01,0x84,0xC6};             //土壤PH

uint32_t SensorCahe = 0;
uint32_t sSensorCahe = 0;


/*******************************************************************************
* 函数名	: AnalyzeComand
* 描述	    	: 解析传感器指令
* 输入参数  	: 无
* 返回参数  	: 无
*******************************************************************************/
static void AnalyzeComand(uint8_t *data,uint8_t Len)
{
	if(Len != 0)
	{
		hal_Delay_ms(50);

		CalcuResult = Crc16(data,Len-2);
		CRC_Result[0] = (unsigned char)((CalcuResult & 0xFF00) >> 8);
		CRC_Result[1] = (unsigned char)(CalcuResult & 0xFF);
		if((data[Len-2] == CRC_Result[0]) && (data[Len-1] == CRC_Result[1]))   //判断数据接收是否存在异常
		{
			if(data[1]==0x03)
			{
				switch(data[0])
				{
					case 0x03:		//Temperature&Humidity,温湿度
						/**************湿度**************/
						SensorCahe = (uint32_t)data[3]*256 + data[4];
						AppDataPointer->AgricultureData.Temperature = (float)SensorCahe/10;
						Send_Buffer[9] = SensorCahe / 256;
						Send_Buffer[10] = SensorCahe % 256;
						/**************温度**************/
						SensorCahe = (uint32_t)data[5]*256 + data[6];
						AppDataPointer->AgricultureData.Humidity = (float)SensorCahe/10;
						Send_Buffer[7] = SensorCahe / 256;
						Send_Buffer[8] = SensorCahe % 256;
						break;
					case 0x07:      //光照度
						SensorCahe = (uint32_t)data[3]*256 + data[4];
						SensorCahe = SensorCahe<<16 + (uint32_t)data[5]*256 + data[6];
//								SensorCahe = (uint32_t)data[3]*16777216+data[4]*65536+data[5]*256+data[6];
						AppDataPointer->AgricultureData.Illumination = SensorCahe;
						Send_Buffer[11] = data[3];
						Send_Buffer[12] = data[4];
						Send_Buffer[13] = data[5];
						Send_Buffer[14] = data[6];
						break;
					case 0x0A:	    //水质PH
						SensorCahe = (uint32_t)data[3]*256 + data[4];
						AppDataPointer->AgricultureData.WaterPH = (float)SensorCahe/100;
						Send_Buffer[15] = SensorCahe / 256;
						Send_Buffer[16] = SensorCahe % 256;
						break;
					case 0x0B:	    //水温
						SensorCahe = (uint32_t)data[7]*256 + data[8];
						AppDataPointer->AgricultureData.WaterTemp = (float)SensorCahe/10;
						Send_Buffer[17] = SensorCahe / 256;
						Send_Buffer[18] = SensorCahe % 256;
						break;
					case 0x0C:	    //土壤传感器  温度   水分  电导率
						/**************土壤温度**************/
						SensorCahe = (uint32_t)data[3]*256 + data[4];
						AppDataPointer->AgricultureData.SoilTemp = (float)SensorCahe/010;
						Send_Buffer[19] = SensorCahe / 256;
						Send_Buffer[20] = SensorCahe % 256;
						/**************土壤湿度**************/
						SensorCahe = (uint32_t)data[5]*256 + data[6];
						AppDataPointer->AgricultureData.SoilHum = (float)SensorCahe/100;
						Send_Buffer[21] = SensorCahe / 256;
						Send_Buffer[22] = SensorCahe % 256;
						/**************土壤电导率**************/
						SensorCahe = (uint32_t)data[7]*256 + data[8];
						AppDataPointer->AgricultureData.SoilConductivity = (float)SensorCahe/10;
						Send_Buffer[23] = SensorCahe / 256;
						Send_Buffer[24] = SensorCahe % 256;
						break;
					case 0x0D:		//土壤PH
						SensorCahe = (uint32_t)data[3]*256 + data[4];
						if(SensorCahe > 1400)
							SensorCahe = 1400;
						AppDataPointer->AgricultureData.SoilPH = (float)SensorCahe/100;
						Send_Buffer[25] = SensorCahe / 256;
						Send_Buffer[26] = SensorCahe % 256;
						break;
					default:
						break;
				}//switch(data[0]) END	
			} //(data[1]==0x03)  END
 		} //CRC  END

		SensorStatusBuff[1] = SensorStatus_L;
		SensorStatusBuff[0] = SensorStatus_H;

		AppDataPointer->TerminalInfoData.SensorStatus = (uint16_t)SensorStatus_H*256 + (uint16_t)SensorStatus_L;

		Clear_CMD_Buffer(dRxBuff,dRxNum);
		dRxNum=0;
	}
}

/*******************************************************************************
* 函数名		: InqureSensor
* 描述	    	: 采集传感器数据；分析数据值
* 输入参数  	: 无
* 返回参数  	: 无
*******************************************************************************/
void InqureSensor(void)
{
	volatile char scadaIndex;
	for(scadaIndex=1;scadaIndex<=SensorNum;scadaIndex++)
	{
		Send_485_Enable;
		hal_Delay_ms(5);
		switch(scadaIndex)
		{
			case 1:
				SendNByteToUart3(Inqure_TempHumidity,Command_1Length);
				break;
			case 2:
				SendNByteToUart3(Inqure_Illumination,Command_1Length);
				break;
			case 3:
				SendNByteToUart3(Inqure_WaterPH,Command_1Length);
				break;
			case 4:
				SendNByteToUart3(Inqure_WaterTemp,Command_1Length);
				break;
			case 5:
//							SendNByteToUart3(Inqure_SoilTempHumCond,Command_1Length);
				break;
			case 6:
				SendNByteToUart3(Inqure_SoilPH,Command_1Length);
				break;
			default:
				break;
		}
		hal_Delay_ms(2);//高波特率降低延时为1-2ms，否则容易丢包；低波特率增加延时，如4800延时10ms，否则容易丢包
		Recive_485_Enable;
		hal_Delay_sec(1);
		LED_ON;
		AnalyzeComand(dRxBuff,dRxNum);
		LED_OFF;
	}
}




#endif //(PRODUCT_TYPE == Voc_Station)