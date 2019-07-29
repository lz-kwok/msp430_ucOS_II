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
* Filename      : g_Dust_Station.c
* Version       : V1.00
* Programmer(s) : GLZ
*********************************************************************************************************
*/
#include  <hal_layer_api.h>
#include  <bsp.h>

#if (PRODUCT_TYPE == Dust_Station)

#define SensorNum			5 
#define CMDLength        	8

AppStruct  App;
DataStruct *AppDataPointer;

uint32_t Send_Buffer[34] = {0xaa,0x00,0x00,0x01,0x01,0x00,0x01,0x7F,0xFF,0x7F,0xFF,0x7F,
0xFF,0x7F,0xFF,0x7F,0xFF,0x7F,0xFF,0x7F,0xFF,0x7F,0xFF,0x7F,0xFF,0x7F,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xff};

uint8_t Inqure_WindSpeed[CMDLength]={0x01,0x03,0x00,0x00,0x00,0x01,0x84,0x0A};          //风速
uint8_t Inqure_WindDirection[CMDLength]={0x02,0x03,0x00,0x00,0x00,0x02,0xC4,0x38};      //风向
uint8_t Inqure_TempHumidity[CMDLength]={0x03,0x03,0x00,0x00,0x00,0x02,0xC5,0xE9};       //温湿度
uint8_t Inqure_Noise[CMDLength]={0x04,0x03,0x00,0x00,0x00,0x01,0x84,0x5F};              //噪音
uint8_t Inqure_PM[CMDLength]={0x05,0x03,0x00,0x00,0x00,0x02,0xC5,0x8F};                 //PM
// uint8_t Inqure_AirPressure[CMDLength]={0x06,0x03,0x00,0x00,0x00,0x01,0x85,0xBD};        //气压
// uint8_t Inqure_Illumination[CMDLength]={0x07,0x03,0x00,0x00,0x00,0x02,0xC4,0x6D};       //光照
// uint8_t Inqure_RainGauge[CMDLength]={0x08,0x03,0x00,0x00,0x00,0x01,0x84,0x93};          //雨量-XPH
// uint8_t Inqure_Radiation[CMDLength]={0x09,0x03,0x00,0x00,0x00,0x01,0x85,0x42};          //总辐射-太阳辐射
// uint8_t Inqure_HCD6816_YS[CMDLength]={0xFF,0x03,0x00,0x03,0x00,0x0C,0xA0,0x11};         //气象六要素传感器--YS

uint32_t SensorCahe = 0;
uint32_t sSensorCahe = 0;
static uint8_t SensorStatus_H;
static uint8_t SensorStatus_L;
static uint8_t  SensorStatusBuff[2];       //传感器状态数组

/*******************************************************************************
* 函数名		: AnalyzeComand
* 描述	    	: 解析传感器指令
* 输入参数  	: 无
* 返回参数  	: 无
*******************************************************************************/
static int AnalyzeComand(uint8_t *data,uint8_t Len)
{
	if(Len != 0)
	{
		uint16_t CalcuResult = 0;
		uint8_t CRC_Result[2];
		hal_Delay_ms(50);

		CalcuResult = Crc16(data,Len-2);
		CRC_Result[0] = (uint8_t)((CalcuResult & 0xFF00) >> 8);
		CRC_Result[1] = (uint8_t)(CalcuResult & 0xFF);
		if((data[Len-2] == CRC_Result[0]) && (data[Len-1] == CRC_Result[1]))   //判断数据接收是否存在异常
		{
			if(data[1]==0x03)
			{
				switch(data[0])
				{
					case 0x01:		//Wind_Speed
						SensorCahe = (uint32_t)data[3]*256 + data[4];
						AppDataPointer->DustData.WindSpeed = (float)SensorCahe/10;
						hal_SetBit(SensorStatus_L, 7);   //传感器状态位置1
						Send_Buffer[11] = SensorCahe / 256;
						Send_Buffer[12] = SensorCahe % 256;
						break;
					case 0x02:		//Wind_Direction
						SensorCahe = (uint32_t)data[5]*256 + data[6];
						AppDataPointer->DustData.WindDirection = SensorCahe;
						hal_SetBit(SensorStatus_L, 6);   //传感器状态位置1
						Send_Buffer[13] = SensorCahe / 256;
						Send_Buffer[14] = SensorCahe % 256;
						break;
					case 0x03:		//Temperature&Humidity,温湿度
						/**************湿度**************/
						SensorCahe = (uint32_t)data[3]*256 + data[4];
						AppDataPointer->DustData.Humidity = (float)SensorCahe/10;
						hal_SetBit(SensorStatus_H, 0);   //传感器状态位置1
						Send_Buffer[9] = SensorCahe / 256;
						Send_Buffer[10] = SensorCahe % 256;
						/**************温度**************/
						if(data[5]>=0xF0) //土壤温度为负数
						{
							sSensorCahe = 0XFFFF-((uint32_t)data[5]*256 + data[6])+0X01;
							AppDataPointer->DustData.Temperature = (float)sSensorCahe/10*(-1);
							sSensorCahe = (uint32_t)data[5]*256 + data[6];//原始数据存放到发送字节中
						}
						else              //土壤温度为正数
						{
							sSensorCahe = (uint32_t)data[5]*256 + data[6];
							AppDataPointer->DustData.Temperature = (float)sSensorCahe/10;
						}
						hal_SetBit(SensorStatus_H, 1);   //传感器状态位置1
						Send_Buffer[7] = sSensorCahe / 256;
						Send_Buffer[8] = sSensorCahe % 256;
						break;
					case 0x04:      //噪音
						SensorCahe = (uint32_t)data[3]*256 + data[4];
						AppDataPointer->DustData.Noise = (float)SensorCahe/10;
						hal_SetBit(SensorStatus_L, 3);   //传感器状态位置1
						Send_Buffer[19] = SensorCahe / 256;
						Send_Buffer[20] = SensorCahe % 256;
						break;
					case 0x05:	    //PM2.5 ,PM10
						/**************PM10**************/
						SensorCahe = (uint32_t)data[3]*256 + data[4];
						AppDataPointer->DustData.PM10 = SensorCahe;
						hal_SetBit(SensorStatus_L, 4);   //传感器状态位置1
						Send_Buffer[17] = SensorCahe / 256;
						Send_Buffer[18] = SensorCahe % 256;
						/**************PM2.5**************/
						sSensorCahe = (uint32_t)data[5]*256 + data[6];
						AppDataPointer->DustData.PM25 = sSensorCahe;
						hal_SetBit(SensorStatus_L, 5);   //传感器状态位置1
						Send_Buffer[15] = sSensorCahe / 256;
						Send_Buffer[16] = sSensorCahe % 256;
						break;
					default:
						break;
				}//switch(data[0]) END
				
			} //(data[1]==0x03)  END
 		}else{
			 return 0;
		}

		SensorStatusBuff[1] = SensorStatus_L;
		SensorStatusBuff[0] = SensorStatus_H;

		AppDataPointer->TerminalInfoData.SensorStatus = (uint16_t)SensorStatus_H*256 + (uint16_t)SensorStatus_L;

		Clear_CMD_Buffer(dRxBuff,dRxNum);
		dRxNum=0;

		return 1;
	}else{
		return -1;
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
				// OSBsp.Device.Usart3.WriteNData(Inqure_TempHumidity,CMDLength);
				break;
			case 2:
				// OSBsp.Device.Usart3.WriteNData(Inqure_WindSpeed,CMDLength);
				break;
			case 3:
				OSBsp.Device.Usart3.WriteNData(Inqure_WindDirection,CMDLength);
				break;
			case 4:
				OSBsp.Device.Usart3.WriteNData(Inqure_PM,CMDLength);
				break;
			case 5:
				OSBsp.Device.Usart3.WriteNData(Inqure_Noise,CMDLength);
				break;
			default:
				break;
		}
		hal_Delay_ms(10);//高波特率降低延时为1-2ms，否则容易丢包；低波特率增加延时，如4800延时10ms，否则容易丢包
		Recive_485_Enable;
		LED_ON;
		OSTimeDly(500);  //任务挂起
		int ret = AnalyzeComand(dRxBuff,dRxNum);
		uint32_t times = scadaIndex;
		if(ret == 0){
			g_Printf_dbg("%s.AnalyzeComand.CRC check failed\r\n",__func__);
		}else if(ret == -1){
			g_Printf_dbg("%s.AnalyzeComand.Sensor has no answer %d times\r\n",__func__,times);
		}
		LED_OFF;
	}
}


char *MakeJsonBodyData(DataStruct *DataPointer)
{
    mallco_dev.init();

    cJSON * pJsonRoot = NULL;
	cJSON * pSubJson;
	cJSON * pBack;
	char * p;

    pJsonRoot = cJSON_CreateObject();
    if(NULL == pJsonRoot)
    {
        cJSON_Delete(pJsonRoot);
        return NULL;
    }

    cJSON_AddNumberToObject(pJsonRoot, "SN",DataPointer->TerminalInfoData.SerialNumber);
    cJSON_AddNumberToObject(pJsonRoot, "DeviceID",DataPointer->TerminalInfoData.DeviceID);
//    cJSON_AddNumberToObject(pJsonRoot, "DeviceType",DataPointer->TerminalInfoData.DeviceType);
    cJSON_AddNumberToObject(pJsonRoot, "SeqNum",DataPointer->TransMethodData.SeqNumber);
//    cJSON_AddNumberToObject(pJsonRoot, "DecSensorStatus",DataPointer->TerminalInfoData.SensorStatus);//10进制传感器状态

    pSubJson = NULL;
    pSubJson = cJSON_CreateObject();
    if(NULL == pSubJson)
    {
      //create object faild, exit
      cJSON_Delete(pSubJson);
      return NULL;
    }

	if(hal_GetBit(SensorStatus_H, 1)) {
		cJSON_AddNumberToObject(pSubJson, "温度",DataPointer->DustData.Temperature);
	}
	if(hal_GetBit(SensorStatus_H, 0)) {
		cJSON_AddNumberToObject(pSubJson, "湿度",DataPointer->DustData.Humidity);
	}
	if(hal_GetBit(SensorStatus_L, 7)) {
		cJSON_AddNumberToObject(pSubJson, "风速",DataPointer->DustData.WindSpeed);
	}
	if(hal_GetBit(SensorStatus_L, 6)) {
		cJSON_AddNumberToObject(pSubJson, "风向",DataPointer->DustData.WindDirection);
	}
	if(hal_GetBit(SensorStatus_L, 5)) {
		cJSON_AddNumberToObject(pSubJson, "PM2.5",DataPointer->DustData.PM25);
	}
	if(hal_GetBit(SensorStatus_L, 4)) {
		cJSON_AddNumberToObject(pSubJson, "PM10",DataPointer->DustData.PM10);
	}
	if(hal_GetBit(SensorStatus_L, 3)) {
		cJSON_AddNumberToObject(pSubJson, "噪音",DataPointer->DustData.Noise);
	}
	cJSON_AddItemToObject(pJsonRoot, "DustData", pSubJson);
	pBack = NULL;
	pBack = cJSON_CreateObject();
	if(NULL == pBack)
	{
		// create object faild, exit
		cJSON_Delete(pJsonRoot);
		return NULL;
	}
	cJSON_AddNumberToObject(pJsonRoot, "Version",DataPointer->TerminalInfoData.Version);
	cJSON_AddNumberToObject(pJsonRoot, "Quanity",DataPointer->TerminalInfoData.PowerQuantity);
	cJSON_AddStringToObject(pJsonRoot, "Uptime",TimeString);

    p = cJSON_Print(pJsonRoot);
    if(NULL == p)
    {
      //convert json list to string faild, exit
      //because sub json pSubJson han been add to pJsonRoot, so just delete pJsonRoot, if you also delete pSubJson, it will coredump, and error is : double free
      cJSON_Delete(pJsonRoot);
      return NULL;
    }

    cJSON_Delete(pJsonRoot);
    return p;
}


/*******************************************************************************
* 函数名		: ScadaData_base_Init
* 描述	    	: 初始化采集的数据大小
* 输入参数  	: 无
* 返回参数  	: 无
*******************************************************************************/
void ScadaData_base_Init(void)
{
//	static uint32_t dataH = 0,dataM = 0,dataL = 0;

	//首先执行指针地址赋值
    AppDataPointer = &(App.Data);     //DataStruct * AppDataPointer; 定义结构体类型的指针 ,只是一个名字，具体要定义指针的地址

	//只初始化要用到的数据
#if (TRANSMIT_TYPE == GPRS_Mode)
	AppDataPointer->TerminalInfoData.DeviceStatus = DEVICE_STATUS_POWER_OFF;
	AppDataPointer->TransMethodData.GPRSStatus = GPRS_Power_off;
	AppDataPointer->TransMethodData.GPRSNet = 0;
#endif
}

/*******************************************************************************
* 函数名		: Terminal_Para_Init
* 描述	    	: 根据Flash储存信息初始化终端参数
* 输入参数  	: 无
* 返回参数 		: 无
*******************************************************************************/
void Terminal_Para_Init(void)
{
	int i = 0;
	Hex2Double TransferData;
	/************************地理信息******************************************/
	for(i=0;i<8;i++){
		TransferData.Hex[i] = OSBsp.Device.InnerFlash.innerFLASHRead(i+100,infor_ChargeAddr);
	}
	App.Data.TerminalInfoData.Longitude = TransferData.Data;
	for(i=0;i<8;i++){
		TransferData.Hex[i] = OSBsp.Device.InnerFlash.innerFLASHRead(i+108,infor_ChargeAddr);
	}
	App.Data.TerminalInfoData.Latitude = TransferData.Data;
	for(i=0;i<8;i++){
		TransferData.Hex[i] = OSBsp.Device.InnerFlash.innerFLASHRead(i+116,infor_ChargeAddr);
	}
	App.Data.TerminalInfoData.Altitude = TransferData.Data;


	/************************DeviceID******************************************/
	App.Data.TerminalInfoData.DeviceID = Hal_getDeviceID();
	Send_Buffer[2] = App.Data.TerminalInfoData.DeviceID/256;
	Send_Buffer[3] = App.Data.TerminalInfoData.DeviceID%256;
	//---------------------//
//	App.Data.TerminalInfoData.DeviceID = 91005;
//	Send_Buffer[1] = 0x01;
//	Send_Buffer[2] = 0x63;
//	Send_Buffer[3] = 0x7D;
	/************************出厂编号******************************************/
	App.Data.TerminalInfoData.SerialNumber = Hal_getSerialNumber();
//	App.Data.TerminalInfoData.SerialNumber = 91005;
	/************************生产日期******************************************/
	App.Data.TerminalInfoData.PD = Hal_getManufactureDate();
	/************************Version******************************************/
	App.Data.TerminalInfoData.Version = Hal_getFirmwareVersion();       //软件版本
	/************************SendPeriod******************************************/
	App.Data.TerminalInfoData.SendPeriod = Hal_getTransmitPeriod();		//发送周期
	/************************TerminalIndex 终端类型******************************************/
	App.Data.TerminalInfoData.DeviceType = PRODUCT_TYPE;

#if (TRANSMIT_TYPE == GPRS_Mode)
	#ifdef SIM800C
	// Socket_5V_ON;            //GPRS  PowerON-P5.0 //传输板上插GPRS模块时供电
	// delay_sec(5); 			 //wj20180511  为了稳定5V电源一段时间
	// ResetCommunication();    //模块复位管脚复位,对于GPRS模块电压需要达到5V,单片输出管脚电压只有3.3V
	g_Device_Usart0_Init(9600);	     //根据所选通信方式选择初始化波特率  GPRS
	// InitSim800C();           //初始化GPRS
	#endif
	#ifdef AIR202
	// Socket_5V_ON;            //GPRS  PowerON-P5.0 //传输板上插GPRS模块时供电
	// Socket_3V_ON;
	// delay_sec(2);
	// AIR202_Power_ON;
	// delay_sec(2);
	// AIR202_Power_OFF;
	if(Hal_getProductKey(App.Data.TerminalInfoData.ProductKey) != 0){
		return ;
	}
	OSBsp.Device.Usart2.WriteString("ProductKey: ");
	OSBsp.Device.Usart2.WriteString(App.Data.TerminalInfoData.ProductKey);
	OSBsp.Device.Usart2.WriteString("\r\n");

	if(Hal_getDeviceName(App.Data.TerminalInfoData.DeviceName) != 0){
		return ;
	}
	OSBsp.Device.Usart2.WriteString("DeviceName: ");
	OSBsp.Device.Usart2.WriteString(App.Data.TerminalInfoData.DeviceName);
	OSBsp.Device.Usart2.WriteString("\r\n");

	if(Hal_getDeviceSecret(App.Data.TerminalInfoData.DeviceSecret) != 0){
		return ;
	}
	OSBsp.Device.Usart2.WriteString("DeviceSecret: ");
	OSBsp.Device.Usart2.WriteString(App.Data.TerminalInfoData.DeviceSecret);
	OSBsp.Device.Usart2.WriteString("\r\n");

	// HashValueSet();
	g_Device_Usart0_Init(9600);        //根据所选通信方式选择初始化波特率
	AppDataPointer->TransMethodData.GPRSStatus = GPRS_Waitfor_SMSReady;
	#endif
#elif (TRANSMIT_TYPE == NBIoT_BC95_Mode)
	Socket_3V_ON;	         			// PowerON-P4.3 //传输板上插LoRa模块时供电
	hal_Delay_ms(100);			 			//wj20180511
	ResetCommunication();    			//模块复位管脚复位
	g_Device_Usart0_Init(9600);	     	//根据所选通信方式选择初始化波特率   NBIOT
	Init_NB();
#elif (TRANSMIT_TYPE == LoRa_F8L10D_Mode)
	Socket_3V_ON;	         //LoRa  PowerON-P4.3 //传输板上插LoRa模块时供电
	hal_Delay_ms(100);			 //wj20180511
	OSBsp.Device.IOControl.ResetWirelesModule();    //模块复位管脚复位
#if LoRa_QunDeng
	g_Device_Usart0_Init(115200);      //根据所选通信方式选择初始化波特率   LoRa
	LoRaDevEui = System.Device.InnerFlash.innerFLASHRead(9,infor_ChargeAddr);
	LoRaDevEui=LoRaDevEui<<8;
	LoRaDevEui += System.Device.InnerFlash.innerFLASHRead(10,infor_ChargeAddr);
	LoRa_Deveui[27]= LoRaDevEui/1000 + 0x30;
	LoRa_Deveui[28]= LoRaDevEui%1000/100 + 0x30;
	LoRa_Deveui[29]= LoRaDevEui%100/10 + 0x30;
	LoRa_Deveui[30]= LoRaDevEui%10 + 0x30;
#endif
	g_Device_Usart0_Init(115200);      //根据所选通信方式选择初始化波特率   LoRa
	InitLoRa_F8L10D();        //初始化LoRa
#elif (TRANSMIT_TYPE == LoRa_OM402_Mode)
	OSBsp.Device.IOControl.PowerSet(LPModule_Power_On);;	         //LoRa  PowerON-P4.3 //传输板上插LoRa模块时供电
	hal_Delay_ms(100);			 //wj20180511
	OSBsp.Device.IOControl.ResetWirelesModule();    //模块复位管脚复位
	g_Device_Usart0_Init(9600);        //根据所选通信方式选择初始化波特率   LoRa
	InitLoRa_OM402();        //初始化门思LoRa模块
#endif

#if (ACCESSORY_TYPR == None_Mode)
	OSBsp.Device.IOControl.PowerSet(GPS_Power_Off);
#elif (ACCESSORY_TYPR == ELCD_Mode)
	OSBsp.Device.IOControl.PowerSet(GPS_Power_On);
	g_Device_Usart1_Init(115200); 
#elif (ACCESSORY_TYPR == GPS_Mode)
	OSBsp.Device.IOControl.PowerSet(GPS_Power_On);
	g_Device_Usart1_Init(9600);
#endif
}

#endif //(PRODUCT_TYPE == Dust_Station)

