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

	HashValueSet();
	g_Device_Usart0_Init(9600);        //根据所选通信方式选择初始化波特率
	AppDataPointer->TransMethodData.GPRSStatus = GPRS_Waitfor_SMSReady;
	#endif
#elif (TRANSMIT_TYPE == NBIoT_BC95_Mode)
	Socket_3V_ON;	         				// PowerON-P4.3 //传输板上插LoRa模块时供电
	hal_Delay_ms(100);			 			//wj20180511
	ResetCommunication();    				//模块复位管脚复位
	g_Device_Usart0_Init(9600);	     		//根据所选通信方式选择初始化波特率   NBIOT
	Init_NB();
#elif (TRANSMIT_TYPE == LoRa_F8L10D_Mode)
	Socket_3V_ON;	         		//LoRa  PowerON-P4.3 //传输板上插LoRa模块时供电
	hal_Delay_ms(100);			 	//wj20180511
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
	Socket_3V_ON;	         //LoRa  PowerON-P4.3 //传输板上插LoRa模块时供电
	hal_Delay_ms(100);			 //wj20180511
	OSBsp.Device.IOControl.ResetWirelesModule();    //模块复位管脚复位
	g_Device_Usart0_Init(9600);        //根据所选通信方式选择初始化波特率   LoRa
	InitLoRa_OM402();        //初始化门思LoRa模块
#endif

#if (ACCESSORY_TYPR == None_Mode)
	GPS_3V_OFF;
#elif (ACCESSORY_TYPR == ELCD_Mode)
	GPS_3V_ON;
	g_Device_Usart1_Init(115200); 
#elif (ACCESSORY_TYPR == GPS_Mode)
	GPS_3V_ON;
	g_Device_Usart1_Init(9600);
#endif
}



#endif //(PRODUCT_TYPE == Voc_Station)

