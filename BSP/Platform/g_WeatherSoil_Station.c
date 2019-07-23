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
* Filename      : g_WeatherSoil_Station.c
* Version       : V1.00
* Programmer(s) : GLZ
*********************************************************************************************************
*/
#include  <hal_layer_api.h>
#include  <bsp.h>

#if (PRODUCT_TYPE == WeatherSoil_Station)

#define SensorNum		5 
#define CMDLength       8

AppStruct  App;
DataStruct *AppDataPointer;


uint8_t ScadaSoilTempHum_YS[Command_1Length]={0x0C,0x03,0x00,0x00,0x00,0x02,0xC5,0x16};       //土壤温湿度_YS 研盛
uint8_t ScadaTempHumIllu_RK[Command_1Length]={0x03,0x03,0x00,0x00,0x00,0x04,0x45,0xEB};       //温湿度光照_RK
uint8_t ScadaCO2_RK[Command_1Length]={0x0A,0x03,0x00,0x02,0x00,0x01,0x24,0xB1};               //CO2_RK

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
					case 0x03:		//温湿度,光照
						/**************湿度**************/
						SensorCahe = (uint32_t)data[3]*256 + data[4];
						AppDataPointer->WetherSoilData.Humidity = (float)SensorCahe/10;
						SetBit(SensorStatus_H, 0);   //传感器状态位置1
						Send_Buffer[9] = SensorCahe / 256;
						Send_Buffer[10] = SensorCahe % 256;
						/**************温度**************/
						if(data[5]>=0xF0) //温度为负数
						{
							sSensorCahe = 0XFFFF-((uint32_t)data[5]*256 + data[6])+0X01;
							AppDataPointer->WetherSoilData.Temperature = (float)sSensorCahe/10*(-1);
							sSensorCahe = (uint32_t)data[5]*256 + data[6]; //原始数据存放到发送字节中
						}
						else              //温度为正数
						{
							sSensorCahe = (uint32_t)data[5]*256 + data[6];
							AppDataPointer->WetherSoilData.Temperature = (float)sSensorCahe/10;
						}
						SetBit(SensorStatus_H, 1);   //传感器状态位置1
						Send_Buffer[7] = sSensorCahe / 256;
						Send_Buffer[8] = sSensorCahe % 256;
						//**************光照度**************//
						SensorCahe = (uint32_t)data[7]*16777216 + data[8]*65536 + data[9]*256 + data[10];
						AppDataPointer->WetherSoilData.Illumination = SensorCahe;
						SetBit(SensorStatus_L, 7);   //传感器状态位置1
						SetBit(SensorStatus_L, 6);   //传感器状态位置1
						Send_Buffer[11] = data[7];
						Send_Buffer[12] = data[8];
						Send_Buffer[13] = data[9];
						Send_Buffer[14] = data[10];
						break;
					case 0x0C:		//土壤温湿度
						//+++++++土壤温湿度电导率_YS+++++++++++//
						/**************土壤温度**************/
						SensorCahe = (uint32_t)data[5]*256 + data[6];
						AppDataPointer->WetherSoilData.SoilTemp = (float)SensorCahe/100-20;
						SetBit(SensorStatus_L, 5);   //传感器状态位置1
						SensorCahe = SensorCahe - 2000;
						Send_Buffer[15] = SensorCahe / 256;
						Send_Buffer[16] = SensorCahe % 256;
						/**************土壤湿度**************/
						SensorCahe = (uint32_t)data[3]*256 + data[4];
						AppDataPointer->WetherSoilData.SoilHum = (float)SensorCahe/10;
						SetBit(SensorStatus_L, 4);   //传感器状态位置1
						Send_Buffer[17] = SensorCahe / 256;
						Send_Buffer[18] = SensorCahe % 256;
						break;
					case 0x0A:		//CO2
						SensorCahe = (uint32_t)data[3]*256 + data[4];
						AppDataPointer->WetherSoilData.CO2 = SensorCahe;
						SetBit(SensorStatus_L, 3);   //传感器状态位置1
						Send_Buffer[19] = SensorCahe / 256;
						Send_Buffer[20] = SensorCahe % 256;
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
				SendNByteToUart3(ScadaTempHumIllu_RK,Command_1Length);   //温湿度光照_RK
				break;
			case 2:
				SendNByteToUart3(ScadaSoilTempHum_YS,Command_1Length);   //土壤温湿度_YS
				break;
			case 3:
				SendNByteToUart3(ScadaCO2_RK,Command_1Length);           //CO2_RK
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