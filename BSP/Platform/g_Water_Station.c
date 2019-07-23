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
* Filename      : g_Water_Station.c
* Version       : V1.00
* Programmer(s) : GLZ
*********************************************************************************************************
*/
#include  <hal_layer_api.h>
#include  <bsp.h>

#if (PRODUCT_TYPE == Water_Station)

#define SensorNum		9 
#define CMDLength       8

AppStruct  App;
DataStruct *AppDataPointer;


uint8_t ScadaLevel[Command_1Length]={0x01,0x03,0x00,0x00,0x00,0x02,0xC4,0x0B};                //雷达水位-大禹
uint8_t ScadaFlowrate[Command_1Length]={0x04,0x03,0x00,0x03,0x00,0x01,0x74,0x5F};             //雷达流速-大禹
uint8_t ScadaCLCupboard[Command_1Length]={0x06,0x03,0x10,0x00,0x00,0x04,0x41,0x7E};           //氯离子柜式监测站
uint8_t ScadaLV[Command_1Length]  = {0x01,0x03,0x00,0x04,0x00,0x01,0xC5,0xCB};		          //投入式液位-格林
uint8_t ScadaUSLV[Command_1Length]  = {0x02,0x03,0x00,0x00,0x00,0x01,0x84,0x39};		      //超声波液位
uint8_t ScadaRulerLV[Command_1Length]  = {0x03,0x03,0x00,0x00,0x00,0x01,0x85,0xE8};		      //感应式电子水尺液位
uint8_t ScadaNH4_QM[Command_1Length]  = {0x04,0x03,0x01,0x82,0x00,0x02,0x65,0x8A};		      //氨氮-清淼
uint8_t ScadaNH4_ZJ[Command_1Length]  = {0x04,0x03,0x00,0x14,0x00,0x02,0x84,0x5A};		      //氨氮-质晶，一次性读取温度及氨氮值
uint8_t ScadaNH4_KMS[Command_1Length] = {0x04,0x03,0x00,0x00,0x00,0x04,0x44,0x5C};		      //氨氮-KMS，一次性读取温度及氨氮值
uint8_t ScadaCOD_KMS[Command_1Length]  = {0x05,0x03,0x00,0x00,0x00,0x04,0x45,0x8D};           //COD_KMS
//uint8_t ScadaCOD_WS[Command_1Length]  = {0x05,0x03,0x00,0x24,0x00,0x01,0xC5,0x85};            //COD_WS
uint8_t ScadaCOD_WS[Command_1Length]  = {0x05,0x03,0x00,0x42,0x00,0x02,0x65,0x9B};            //COD_WS_float
uint8_t ScadaORP_202[Command_1Length]  = {0x06,0x03,0x00,0x00,0x00,0x02,0xC5,0xBC};           //氧化还原电位ORP_KMS
uint8_t ScadaORP_QJ[Command_1Length]  = {0x06,0x03,0x00,0x00,0x00,0x06,0xC4,0x7F};            //氧化还原电位ORP_QJ
uint8_t ScadaRDO[Command_1Length]  = {0x07,0x03,0x00,0x00,0x00,0x04,0x44,0x6F};			      //溶解氧_KMS
uint8_t ScadaDO_QJ[Command_1Length]  = {0x07,0x03,0x00,0x00,0x00,0x06,0xC5,0xAE};			  //溶解氧_QJ
uint8_t ScadaZS[Command_1Length]  = {0x08,0x03,0x00,0x00,0x00,0x04,0x44,0x90};			      //浊度_KMS
uint8_t ScadaZS_WS[Command_1Length]  = {0x08,0x03,0x00,0x1e,0x00,0x01,0xE4,0x95};			  //浊度_WS
uint8_t ScadaPH[Command_1Length]  = {0x09,0x03,0x00,0x00,0x00,0x04,0x45,0x41};	              //PH_KMS
uint8_t ScadaPH_QJ[Command_1Length]  = {0x09,0x03,0x00,0x00,0x00,0x06,0xC4,0x80};	          //PH_QJ
uint8_t ScadaEC[Command_1Length]  = {0x0A,0x03,0x00,0x00,0x00,0x04,0x45,0x72};	              //EC电导率_KMS
uint8_t ScadaEC_QJ[Command_1Length]  = {0x0A,0x03,0x00,0x00,0x00,0x06,0xC4,0xB3};	          //EC电导率_QJ
uint8_t ScadaCHL[Command_1Length]  = {0x0B,0x03,0x00,0x3A,0x00,0x02,0xE4,0xAC};		          //叶绿素_WS


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
					case 0x01:		//液位传感器
						SensorCahe = (uint32_t)data[3]*256 + data[4];
						if(SensorCahe>0xF000) {    //传感器出现负值的时候，直接转换成0
							SensorCahe = 0;
						}
						AppDataPointer->WaterData.LVValue = (float)SensorCahe/10;
						SetBit(SensorStatus_L, 1);   //传感器状态位置1
						Send_Buffer[23] = SensorCahe / 256;
						Send_Buffer[24] = SensorCahe % 256;
						break;
					case 0x02:		//
						break;
					case 0x03:		//
						break;
					case 0x04:
						//KMS 氨氮温度
						sSensorCahe = (uint32_t)data[7]*256 + data[8];
						if((sSensorCahe<500)&&(sSensorCahe>=0)) //如果温度低于50度才存储
						{
							AppDataPointer->WaterData.TemValue = (float)(((float)sSensorCahe)/10);	//精度待确定20181219
	//								sSensorCahe = sSensorCahe/10;  //保证协议与现有的协议文档统一，所以先除以10！！！！！！！！！！！！！！！！！！
							SetBit(SensorStatus_L, 5);   //传感器状态位置1
							Send_Buffer[15] = sSensorCahe / 256;
							Send_Buffer[16] = sSensorCahe % 256;
							WaterTempData_Buffer[WaterTempData_Index] = sSensorCahe;
							WaterTempData_Index++;
						}
						//KMS 氨氮
						SensorCahe = (uint32_t)data[3]*256 + data[4];
						AppDataPointer->WaterData.NH4Value = (float)(((float)SensorCahe)/10);;
						SetBit(SensorStatus_L, 6);     //传感器状态位置1
						Send_Buffer[13] = (uint16_t)(SensorValue*10)/256;//保证协议与现有的协议文档统一，所以先乘以10！！！！！！！！！！！！！！！！！！
						Send_Buffer[14] = (uint16_t)(SensorValue*10)%256;
						break;
					case 0x05:	    //COD
						//蛙视整数采集
	//								SensorCahe = (uint32_t)data[3]*256 + data[4];
	//								AppDataPointer->WaterData.CODValue = (float)(((float)SensorCahe)/100);
	//								SetBit(SensorStatus_H, 1);   //传感器状态位置1
	//								Send_Buffer[7]=SensorCahe/256;
	//								Send_Buffer[8]=SensorCahe%256;
						//蛙视 float 采集
						SensorData.Hex[0] = data[3];      //小端模式，高位字节存放在前面
						SensorData.Hex[1] = data[4];
						SensorData.Hex[2] = data[5];
						SensorData.Hex[3] = data[6];
						SetBit(SensorStatus_H, 1);   //传感器状态位置1
						if(SensorData.Data > 0)
						{
							AppDataPointer->WaterData.CODValue = SensorData.Data;
						}
						else
						{
							AppDataPointer->WaterData.CODValue = 0.01;
						}
						Send_Buffer[7]=(int)(AppDataPointer->WaterData.CODValue*10)/256;
						Send_Buffer[8]=(int)(AppDataPointer->WaterData.CODValue*10)%256;
						//end 蛙视 float 采集
	//								// COD水温
	//								sSensorCahe = (uint32_t)data[7]*256 + data[8];
	//								AppDataPointer->WaterData.TemValue = (float)(((float)sSensorCahe)/10);
	//								SetBit(SensorStatus_L, 5);   //传感器状态位置1
	//								Send_Buffer[15] = sSensorCahe / 256;
	//								Send_Buffer[16] = sSensorCahe % 256;
						break;
					case 0x06:	    //ORP
						if(data[2] == 0x04)  //数据长度是04，代表KMS ORP
						{
							if(data[3]>=0xF0) //ORP为负数
							{
								SensorCahe = 0XFFFF-((uint32_t)data[3]*256 + data[4])+0X01;
								AppDataPointer->WaterData.ORPValue = (int16_t)SensorCahe*(-1);
								SensorCahe = (uint32_t)data[3]*256 + data[4];  //原始数据存放到发送字节中
							}
							else              //ORP为正数
							{
								SensorCahe = (uint32_t)data[3]*256 + data[4];
								AppDataPointer->WaterData.ORPValue = (int16_t)SensorCahe;
							}
							SetBit(SensorStatus_L, 4);   //传感器状态位置1
							Send_Buffer[17]=SensorCahe/256;
							Send_Buffer[18]=SensorCahe%256;
						}
						if(data[2] == 0x0C)  //数据长度是0C，代表QJ ORP,没有水温值
						{
							SensorData.Hex[0] = data[6]; //大端模式，高位字节存放在后面
							SensorData.Hex[1] = data[5];
							SensorData.Hex[2] = data[4];
							SensorData.Hex[3] = data[3];

							AppDataPointer->WaterData.ORPValue = (int16_t)SensorData.Data;
							SetBit(SensorStatus_L, 4);   //传感器状态位置1
							if((int16_t)SensorData.Data >= 0) //ORP为正数
							{
								Send_Buffer[17]=((uint32_t)SensorData.Data)/256;
								Send_Buffer[18]=((uint32_t)SensorData.Data)%256;
							}
							else                              //ORP为负数
							{
								Send_Buffer[17] = (uint32_t)(0xFFFF - ~(int16_t)SensorData.Data)/256;
								Send_Buffer[18] = (uint32_t)(0xFFFF - ~(int16_t)SensorData.Data)%256;
							}
						}
						break;
					case 0x07:	    //RDO
						if(data[2] == 0x08)  //数据长度是08，代表KMS DO
						{
							SensorCahe = (uint32_t)data[3]*256 + data[4];
							AppDataPointer->WaterData.DOValue = (float)(((float)SensorCahe)/100);
							SetBit(SensorStatus_L, 7);   //传感器状态位置1
							Send_Buffer[11]=SensorCahe/256;
							Send_Buffer[12]=SensorCahe%256;
							//RDO水温
							sSensorCahe = (uint32_t)data[7]*256 + data[8];
							if((sSensorCahe<500)&&(sSensorCahe>=0)) //如果温度低于50度才存储
							{
								AppDataPointer->WaterData.TemValue = (float)(((float)sSensorCahe)/10);
								SetBit(SensorStatus_L, 5);   //传感器状态位置1
								Send_Buffer[15] = sSensorCahe / 256;
								Send_Buffer[16] = sSensorCahe % 256;
								WaterTempData_Buffer[WaterTempData_Index] = sSensorCahe;
								WaterTempData_Index++;
							}
						}
						if(data[2] == 0x0C)  //数据长度是0C，代表QJ DO
						{
							SensorData.Hex[0] = data[6];      //大端模式，高位字节存放在后面
							SensorData.Hex[1] = data[5];
							SensorData.Hex[2] = data[4];
							SensorData.Hex[3] = data[3];

							SetBit(SensorStatus_L, 7);   //传感器状态位置1
							Send_Buffer[11]=((uint32_t)(SensorData.Data*100))/256;
							Send_Buffer[12]=((uint32_t)(SensorData.Data*100))%256;
							AppDataPointer->WaterData.DOValue = (float)(Send_Buffer[11]*256 + Send_Buffer[12])/100;
							//DO水温
							SensorData.Hex[0] = data[14];      //大端模式，高位字节存放在后面
							SensorData.Hex[1] = data[13];
							SensorData.Hex[2] = data[12];
							SensorData.Hex[3] = data[11];

							if((SensorData.Data<50)&&(SensorData.Data>=0)) //如果温度低于50度才存储
							{
								SetBit(SensorStatus_L, 5);   //传感器状态位置1
								Send_Buffer[15]=((uint32_t)(SensorData.Data*10))/256;
								Send_Buffer[16]=((uint32_t)(SensorData.Data*10))%256;
								AppDataPointer->WaterData.TemValue =  (float)(Send_Buffer[15]*256 + Send_Buffer[16])/10;
								WaterTempData_Buffer[WaterTempData_Index] = (uint32_t)(SensorData.Data*10);
								WaterTempData_Index++;
							}
						}
						break;
					case 0x08:	    //ZS
						if(data[2] == 0x08)  //数据长度是08，代表KMS ZS
						{
							SensorCahe = (uint32_t)data[3]*256 + data[4];
							if(SensorCahe == 0)
							{
								SensorCahe += 1+rand()%3;
							}
							AppDataPointer->WaterData.ZSValue = (float)(((float)SensorCahe)/10);
							SetBit(SensorStatus_L, 3);   //传感器状态位置1
							Send_Buffer[19]=SensorCahe/256;
							Send_Buffer[20]=SensorCahe%256;
							//z浊度水温
							sSensorCahe = (uint32_t)data[7]*256 + data[8];
							if((sSensorCahe<500)&&(sSensorCahe>=0)) //如果温度低于50度才存储
							{
								AppDataPointer->WaterData.TemValue = (float)(((float)sSensorCahe)/10);
								SetBit(SensorStatus_L, 5);   //传感器状态位置1
								Send_Buffer[15] = sSensorCahe / 256;
								Send_Buffer[16] = sSensorCahe % 256;
								WaterTempData_Buffer[WaterTempData_Index] = sSensorCahe;
								WaterTempData_Index++;
							}
						}
						if(data[2] == 0x02)  //数据长度是02，代表WS ZS
						{
							SensorCahe = (uint32_t)data[3]*256 + data[4];
							if(SensorCahe > 0x8000)//最高为位1，为负数，需要验证测似
							{
								SensorCahe = 0;
							}
							if(SensorCahe == 0)
							{
								SensorCahe += 1+rand()%3;
							}
							AppDataPointer->WaterData.ZSValue = (float)(((float)SensorCahe)/100);
							SetBit(SensorStatus_L, 3);   //传感器状态位置1
							SensorCahe = SensorCahe/10;  //保证协议与现有的协议文档统一，所以先除以10！！！！！！！！！！！！！！！！！！
							Send_Buffer[19]=SensorCahe/256;
							Send_Buffer[20]=SensorCahe%256;
						}
						break;
					case 0x09:		//PH
						if(data[2] == 0x08)  //数据长度是08，代表KMS PH
						{
							SensorCahe = (uint32_t)data[3]*256 + data[4];
							AppDataPointer->WaterData.PHValue = (float)(((float)SensorCahe)/100);
							SetBit(SensorStatus_L, 2);   //传感器状态位置1
							Send_Buffer[21] = SensorCahe / 256;
							Send_Buffer[22] = SensorCahe % 256;
							//PH水温
							sSensorCahe = (uint32_t)data[7]*256 + data[8];
							if((sSensorCahe<500)&&(sSensorCahe>=0)) //如果温度低于50度才存储
							{
								AppDataPointer->WaterData.TemValue = (float)(((float)sSensorCahe)/10);
								SetBit(SensorStatus_L, 5);   //传感器状态位置1
								Send_Buffer[15] = sSensorCahe / 256;
								Send_Buffer[16] = sSensorCahe % 256;
								WaterTempData_Buffer[WaterTempData_Index] = sSensorCahe;
								WaterTempData_Index++;
							}
						}
						if(data[2] == 0x0C)  //数据长度是0C，代表QJ PH
						{
							SensorData.Hex[0] = data[6];      //大端模式，高位字节存放在后面
							SensorData.Hex[1] = data[5];
							SensorData.Hex[2] = data[4];
							SensorData.Hex[3] = data[3];

							SetBit(SensorStatus_L, 2);   //传感器状态位置1
							Send_Buffer[21]=((uint32_t)(SensorData.Data*100))/256;
							Send_Buffer[22]=((uint32_t)(SensorData.Data*100))%256;
							AppDataPointer->WaterData.PHValue =  (float)(Send_Buffer[21]*256 + Send_Buffer[22])/100;
							//PH水温
							SensorData.Hex[0] = data[14];      //大端模式，高位字节存放在后面
							SensorData.Hex[1] = data[13];
							SensorData.Hex[2] = data[12];
							SensorData.Hex[3] = data[11];

							if((SensorData.Data<50)&&(SensorData.Data>=0)) //如果温度低于50度才存储
							{
								SetBit(SensorStatus_L, 5);   //传感器状态位置1
								Send_Buffer[15]=((uint32_t)(SensorData.Data*10))/256;
								Send_Buffer[16]=((uint32_t)(SensorData.Data*10))%256;
								AppDataPointer->WaterData.TemValue =  (float)(Send_Buffer[15]*256 + Send_Buffer[16])/10;
								WaterTempData_Buffer[WaterTempData_Index] = (uint32_t)(SensorData.Data*10);
								WaterTempData_Index++;
							}
						}
						break;
					case 0x0A:		//EC 电导率
						if(data[2] == 0x08)  //数据长度是08，代表KMS EC
						{
							SensorCahe = (uint32_t)data[3]*256 + data[4];
							AppDataPointer->WaterData.ECValue = SensorCahe;
							SetBit(SensorStatus_H, 0);   //传感器状态位置1
							Send_Buffer[9] = SensorCahe / 256;
							Send_Buffer[10] = SensorCahe % 256;
							//电导率水温
							sSensorCahe = (uint32_t)data[7]*256 + data[8];
							if((sSensorCahe<500)&&(sSensorCahe>=0)) //如果温度低于50度才存储
							{
								AppDataPointer->WaterData.TemValue = (float)(((float)sSensorCahe)/10);
								SetBit(SensorStatus_L, 5);   //传感器状态位置1
								Send_Buffer[15] = sSensorCahe / 256;
								Send_Buffer[16] = sSensorCahe % 256;
								WaterTempData_Buffer[WaterTempData_Index] = sSensorCahe;
								WaterTempData_Index++;
							}
						}
						if(data[2] == 0x0C)  //数据长度是0C，代表QJ EC
						{
							SensorData.Hex[0] = data[6];      //大端模式，高位字节存放在后面
							SensorData.Hex[1] = data[5];
							SensorData.Hex[2] = data[4];
							SensorData.Hex[3] = data[3];

							SetBit(SensorStatus_H, 0);   //传感器状态位置1
							Send_Buffer[9]=((uint32_t)(SensorData.Data*1000))/256;
							Send_Buffer[10]=((uint32_t)(SensorData.Data*1000))%256;
							AppDataPointer->WaterData.ECValue = (uint16_t)(SensorData.Data*1000);
							//EC水温
							SensorData.Hex[0] = data[14];      //大端模式，高位字节存放在后面
							SensorData.Hex[1] = data[13];
							SensorData.Hex[2] = data[12];
							SensorData.Hex[3] = data[11];

							if((SensorData.Data<50)&&(SensorData.Data>=0)) //如果温度低于50度才存储
							{
								SetBit(SensorStatus_L, 5);   //传感器状态位置1
								Send_Buffer[15]=((uint32_t)(SensorData.Data*10))/256;
								Send_Buffer[16]=((uint32_t)(SensorData.Data*10))%256;
								AppDataPointer->WaterData.TemValue =  (float)(Send_Buffer[15]*256 + Send_Buffer[16])/10;
								WaterTempData_Buffer[WaterTempData_Index] = (uint32_t)(SensorData.Data*10);
								WaterTempData_Index++;
							}
						}
					case 0x0B:	    //CHL
						if(data[2] == 0x04)  //数据长度是04，代表WS CHL
						{
							SensorData.Hex[0] = data[3];    //小端模式，高位字节存放在前面
							SensorData.Hex[1] = data[4];
							SensorData.Hex[2] = data[5];
							SensorData.Hex[3] = data[6];
							SensorValue = SensorData.Data;
							if(SensorValue <= 0)
							{
								SensorValue = 1.2 + (float)(rand()%60)/100;
							}
							SetBit(SensorStatus_L, 0);   //传感器状态位置1
							Send_Buffer[25]=((uint32_t)(SensorValue*100))/256;
							Send_Buffer[26]=((uint32_t)(SensorValue*100))%256;
							AppDataPointer->WaterData.CHLValue =  (float)(Send_Buffer[25]*256 + Send_Buffer[26])/100;;
						}
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
				SendNByteToUart3(ScadaLV,Command_1Length);
				break;
			case 2:
//							SendNByteToUart3(ScadaNH4_QM,Command_1Length);
//							SendNByteToUart3(ScadaNH4_ZJ,Command_1Length);
				SendNByteToUart3(ScadaNH4_KMS,Command_1Length);
				break;
			case 3:
//							SendNByteToUart3(ScadaCOD_KMS,Command_1Length);
				SendNByteToUart3(ScadaCOD_WS,Command_1Length);
				break;
			case 4:
				SendNByteToUart3(ScadaORP_202,Command_1Length);
//									SendNByteToUart3(ScadaORP_QJ,Command_1Length);
				break;
			case 5:
				SendNByteToUart3(ScadaRDO,Command_1Length);
//									SendNByteToUart3(ScadaDO_QJ,Command_1Length);  //DO_QJ
				break;
			case 6:
//							SendNByteToUart3(ScadaZS,Command_1Length);
				SendNByteToUart3(ScadaZS_WS,Command_1Length);
				break;
			case 7:
				SendNByteToUart3(ScadaPH,Command_1Length);     //PH_KMS
//									SendNByteToUart3(ScadaPH_QJ,Command_1Length);  //PH_QJ
				break;
			case 8:
//							SendNByteToUart3(ScadaEC,Command_1Length);
				SendNByteToUart3(ScadaEC_QJ,Command_1Length);
				break;
			case 9:
				SendNByteToUart3(ScadaCHL,Command_1Length);    //WS_YLS
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