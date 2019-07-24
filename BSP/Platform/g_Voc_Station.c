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
* Filename      : g_Voc_Station.c
* Version       : V1.00
* Programmer(s) : GLZ
*********************************************************************************************************
*/
#include  <hal_layer_api.h>
#include  <bsp.h>

#if (PRODUCT_TYPE == Voc_Station)

#define SensorNum			6 
#define GasCMDLength        11

AppStruct  App;
DataStruct *AppDataPointer;

uint32_t Send_Buffer[34] = {0xaa,0x00,0x00,0x01,0x01,0x00,0x01,0x7F,0xFF,0x7F,0xFF,0x7F,
0xFF,0x7F,0xFF,0x7F,0xFF,0x7F,0xFF,0x7F,0xFF,0x7F,0xFF,0x7F,0xFF,0x7F,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xff};
uint8_t ScadaGas[GasCMDLength] = {0xFF,0xFF,0x01,0x01,0x05,0x01,0x00,0x6C,0x07,0x74,0xDD};

uint32_t SensorCahe = 0;
static uint8_t SensorStatus_H;
static uint8_t SensorStatus_L;
static uint8_t  SensorStatusBuff[2];       //传感器状态数组
/*******************************************************************************
* 函数名		: AnalyzeComand
* 描述	    	: 解析传感器指令
* 输入参数  	: data,len
* 返回参数  	: 无
*******************************************************************************/
static void AnalyzeComand(uint8_t *data,uint8_t Len)
{
	if(Len != 0)
	{
		hal_Delay_ms(50);
		if((data[0] == 0xFF)&&(data[17] == 0xDD)&&(dRxNum == 18))
		{
			switch(data[3])
			{
				case 0x27:	    //VOC
					SensorCahe = (uint32_t)data[10]*65536 + (uint32_t)data[11]*256 + (uint32_t)data[12];
					AppDataPointer->VOCData.VOC = (float)SensorCahe/100;
					hal_SetBit(SensorStatus_H, 1);   //传感器状态位置1
					Send_Buffer[7] = SensorCahe / 256;
					Send_Buffer[8] = SensorCahe % 256;
					break;
				case 0x01:		//可燃气体
					break;
				case 0x09:		//硫化氢
					break;
				case 0x0B:      //氨气
					break;
				case 0x02:	    //一氧化碳
					SensorCahe = (uint32_t)data[10]*65536 + (uint32_t)data[11]*256 + (uint32_t)data[12];
					AppDataPointer->VOCData.CO = (float)SensorCahe/10;
					hal_SetBit(SensorStatus_L, 5);   //传感器状态位置1
					Send_Buffer[15] = SensorCahe / 256;
					Send_Buffer[16] = SensorCahe % 256;
					break;
				case 0x07:	    //二氧化碳
					SensorCahe = (uint32_t)data[10]*65536 + (uint32_t)data[11]*256 + (uint32_t)data[12];
					AppDataPointer->VOCData.CO2 = SensorCahe;
					hal_SetBit(SensorStatus_L, 4);   //传感器状态位置1
					Send_Buffer[17] = SensorCahe / 256;
					Send_Buffer[18] = SensorCahe % 256;
					break;
				default:
					break;
			}//switch(data[0]) END
		}
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
				ScadaGas[3] = 0x27;        //VOC
				OSBsp.Device.Usart3.WriteNData(ScadaGas,GasCMDLength);
				break;
			case 2:
				break;
			case 3:
				break;
			case 4:
				break;
			case 5:
				ScadaGas[3] = 0x02;        //一氧化碳
				OSBsp.Device.Usart3.WriteNData(ScadaGas,GasCMDLength);
				break;
			case 6:
				ScadaGas[3] = 0x07;        //二氧化碳
				OSBsp.Device.Usart3.WriteNData(ScadaGas,GasCMDLength);
				break;
			default:
				break;
		}
		hal_Delay_ms(10);			//高波特率降低延时为1-2ms，否则容易丢包；低波特率增加延时，如4800延时10ms，否则容易丢包
		Recive_485_Enable;
		hal_Delay_sec(1);
		LED_ON;
		AnalyzeComand(dRxBuff,dRxNum);
		LED_OFF;
	}
}

/*******************************************************************************
* 函数名		: InitScadaData
* 描述	    	: 初始化采集的数据大小
* 输入参数  	: 无
* 返回参数  	: 无
*******************************************************************************/
void InitScadaData(void)
{
//	static uint32_t dataH = 0,dataM = 0,dataL = 0;

	//首先执行指针地址赋值
    AppDataPointer = &(App.Data);     //DataStruct * AppDataPointer; 定义结构体类型的指针 ,只是一个名字，具体要定义指针的地址

	//只初始化要用到的数据
}



#endif //(PRODUCT_TYPE == Voc_Station)

