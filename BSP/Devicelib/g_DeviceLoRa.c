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
* Filename      : g_DeviceGprs.c
* Version       : V1.00
* Programmer(s) : GLZ
*********************************************************************************************************
*/
#include  <bsp.h>

#if (TRANSMIT_TYPE == LoRa_F8L10D_Mode)


static int g_has_response = 0;
static char g_response[256];

//断点续传使用
uint16_t BackupIndex = 0;
uint16_t StartFile = 1;
uint16_t FullFlag = 0;
char RespFile[10];
char Data_Backup[70];
uint8_t ResendData = 0;

//static char TimeString[20] = "20170804 16:00:00";
/*******************************************************************************
* 函数名		: NB_Config
* 描述	    	: 配置NB模组参数
* 输入参数  	: *c--需要配置的指令，m--指令等待时间（m*100ms），t--配置失败重新尝试次数
* 返回参数  	: 0--失败，1--成功
*******************************************************************************/
unsigned char LoRa_Config(unsigned char *c , unsigned char m, unsigned char t)
{
	unsigned char x,i=0;
	i=0;
	Clear_Buffer(aRxBuff,&aRxNum);
	while (!Hal_CheckString(aRxBuff,"OK")  & (i < t))
	{
		Clear_Buffer(aRxBuff,&aRxNum);
	 	User_Printf(c);
	 	g_Printf_dbg(c);		//debug口同步打印
		for(x=0;x<m;x++)
			OSTimeDly(20);

		i++;	
	}
	if(i>=t)
	{
		Clear_Buffer(aRxBuff,&aRxNum);
		return 0;
	}
	else 
	{
		Clear_Buffer(aRxBuff,&aRxNum);
		return 1;
	}
}


/*******************************************************************************
* 函数名		: g_Device_LoRa_Init
* 描述	    	: LoRa模块初始化---F8L10D
* 输入参数  	: 无
* 返回参数  	: 无
*******************************************************************************/
void g_Device_LoRa_Init(void)
{
	if(AppDataPointer->TransMethodData.LoRaStatus == LoRa_Power_on)
	{
        if(LoRa_Config("AT\r\n",5,5))     //测试AT指令
        {
            AppDataPointer->TransMethodData.LoRaStatus = LoRa_Boot;
        }
        else
        {
            AppDataPointer->TransMethodData.LoRaStatus = LoRa_Power_on;
        }

        if(AppDataPointer->TransMethodData.LoRaStatus == LoRa_Boot)       //申请入网
        {
            User_Printf("AT+RJN\r\n");
            AppDataPointer->TransMethodData.LoRaStatus = LoRa_Init_Done;
        }
    }	
}
/*******************************************************************************
* 函数名		: g_Device_LoRa_GetJoined
* 描述	    	: 等待LoRa 入网，Uart串口根据返回信息进行处理，此处只时间等待，若长时间不入网，重启
* 输入参数  	: 无
* 返回参数  	: 无
*******************************************************************************/
void g_Device_LoRa_GetJoined(void)
{
	uint8_t ii = 0;
	//等待注册成功信息
	//joined
	while ((AppDataPointer->TransMethodData.LoRaNet == 0) & (ii < 60))     //等待2min
	{
		// if(String_Chk(aRxBuff,"+QLWEVTIND:0") == 1 && String_Chk(aRxBuff,"+QLWEVTIND:3") ==1)
		// {
		// 	AppDataPointer->TransMethodData.NBStatus = NB_Registered;
		// 	break;
		// }
		// Clear_Buffer(aRxBuff,&aRxNum);
		// User_Printf("AT+CGPADDR\r\n");
		// g_Printf_dbg("AT+CGPADDR\r\n");
		OSTimeDly(1000);
//		System.Device.Usart2.WriteString(dRxBuff);
		ii++;

	}
	if(ii > 60)
	{
		g_Printf_dbg("LoRa join Failed!\r\n");
		OSTimeDly(1000);
	}
	AppDataPointer->TransMethodData.LoRaStatus = LoRa_Join_Over;
	
}
/*******************************************************************************
* 函数名		: g_Device_LoRa_Send
* 描述	    	: LoRa上报数据
* 输入参数  	: *data--准备上报内容，length--数据长度,port---端口
* 返回参数  	: code
*******************************************************************************/
void g_Device_LoRa_Send(uint32_t *data , uint8_t length , uint8_t port)
{
	static unsigned char ii = 0;
	char buff[15];
	AppDataPointer->TransMethodData.LoRaSendStatus = 0;
	Clear_Buffer(aRxBuff,&aRxNum);
	sprintf(buff,"AT+TXH=%d,",port);
	User_Printf(buff);
    // User_Printf("AT+NMGS=34,");
    for(ii=0;ii<length;ii++)
    {
        if(data[ii] < 0x10)       //Printf下小于16进行补0打印
        {
        	User_Printf("0%X",data[ii] & 0xff);			// & 0xff防止出现数据溢出
        }
        else
        {
        	User_Printf("%X",data[ii] & 0xff);				// & 0xff防止出现数据溢出
        }
    }
    User_Printf("\r\n");
	OSTimeDly(200);
}
void SendLoRaStrHex(uint32_t *data, uint8_t len)
{
	static uint8_t ii = 0;

	User_Printf("AT+TXH=15,");
	User_Printf(data);
//	for(ii=0;ii<len;ii++)
//	{
//	    System.Device.Usart2.WriteData(data[ii]);
//	}
	User_Printf("\r\n");
}
/*******************************************************************************
* 函数名		: g_Device_LoRa_Receive
* 描述	    	: LoRa接收下发数据
* 输入参数  	: 无
* 返回参数  	: 无
*******************************************************************************/
void g_Device_LoRa_Receive(void)
{
	uint8_t *Uart0_RxBuff;
	uint8_t Uart0_RxBuff_data[50];
	uint8_t Uart0_RxBuff_Num;
	uint16_t Temp_SendPeriod;
	unsigned char Flash_Tmp[14];  //flash操作中间变量
	unsigned char TimebuffNum=0;
	unsigned char TimeBuff_Hex[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //16进制的时间Buffer  2018年3月15号 20时50分00秒 星期4

		if(Hal_CheckString(aRxBuff,"FF") & Hal_CheckString(aRxBuff,"AA"))
		{
			if(Hal_CheckString(aRxBuff,"FF0102")) //修改上报周期
			{
				Uart0_RxBuff = strstr(aRxBuff,"FF0102");         //判断接收到的数据是否有效
				while(*(Uart0_RxBuff+6) != 0x0A)
				{
					Uart0_RxBuff_data[Uart0_RxBuff_Num] = *(Uart0_RxBuff+6);
					Uart0_RxBuff_Num++;
					Uart0_RxBuff++;
				}
				Temp_SendPeriod = (Uart0_RxBuff_data[0]-0x30)*1000 + (Uart0_RxBuff_data[1]-0x30)*100
									+ (Uart0_RxBuff_data[2]-0x30)*10 + (Uart0_RxBuff_data[3]-0x30)*1;
				if( (Temp_SendPeriod >= 5) && (Temp_SendPeriod <= 240) )
				{
					App.Data.TerminalInfoData.SendPeriod = (unsigned char)(Temp_SendPeriod & 0x00FF);
					g_Printf_dbg("LoRa Set SendPeriod OK\r\n");
					//将发送周期的信息存入Flash
					// delay_ms(10);
					OSTimeDly(5);
					Flash_Tmp[0] = OSBsp.Device.InnerFlash.innerFLASHRead(0, infor_ChargeAddr);
					Flash_Tmp[1] = OSBsp.Device.InnerFlash.innerFLASHRead(1, infor_ChargeAddr);
					Flash_Tmp[2] = OSBsp.Device.InnerFlash.innerFLASHRead(2, infor_ChargeAddr);
					Flash_Tmp[3] = OSBsp.Device.InnerFlash.innerFLASHRead(3, infor_ChargeAddr);
					Flash_Tmp[4] = OSBsp.Device.InnerFlash.innerFLASHRead(4, infor_ChargeAddr);
					Flash_Tmp[5] = OSBsp.Device.InnerFlash.innerFLASHRead(5, infor_ChargeAddr);
					Flash_Tmp[6] = OSBsp.Device.InnerFlash.innerFLASHRead(6, infor_ChargeAddr);
					Flash_Tmp[7] = OSBsp.Device.InnerFlash.innerFLASHRead(7, infor_ChargeAddr);//终端类型
					Flash_Tmp[8] = OSBsp.Device.InnerFlash.innerFLASHRead(8, infor_ChargeAddr);//传输方式
					Flash_Tmp[9] = OSBsp.Device.InnerFlash.innerFLASHRead(9, infor_ChargeAddr);//DevEUI_H(高八位)
					Flash_Tmp[10] = OSBsp.Device.InnerFlash.innerFLASHRead(10, infor_ChargeAddr);//DevEUI_L(低八位)
					Flash_Tmp[11] = App.Data.TerminalInfoData.SendPeriod;//上传周期（min）
					OSBsp.Device.InnerFlash.FlashRsvWrite(Flash_Tmp, 12, infor_ChargeAddr, 0);//把终端信息写入FLASH
				}
				else
				{
					g_Printf_dbg("LoRa Set SendPeriod Failed！\r\n");
				}
			}
			if(Hal_CheckString(aRxBuff,"FF0208")) //同步设备时间
			{
				// System.Device.Usart2.WriteString("Time Set Done!\r\n");
				Uart0_RxBuff = strstr(aRxBuff,"FF0208");         //判断接收到的数据是否有效
				while(*(Uart0_RxBuff+6) != 0x0A)
				{
					Uart0_RxBuff_data[Uart0_RxBuff_Num] = *(Uart0_RxBuff+6);
					Uart0_RxBuff_Num++;
					Uart0_RxBuff++;
				}

				for(TimebuffNum=0;TimebuffNum<8;TimebuffNum++)
				{
					TimeBuff_Hex[TimebuffNum] = ((Uart0_RxBuff_data[TimebuffNum*2]-0x30)<<4) + (Uart0_RxBuff_data[TimebuffNum*2+1]-0x30);
				}
				if( (TimeBuff_Hex[0]==0x20) && (TimeBuff_Hex[1]>=0x18) && (TimeBuff_Hex[2]>=1) && (TimeBuff_Hex[2]<=0x12) && (TimeBuff_Hex[3]>=1) && (TimeBuff_Hex[3]<=0x31)
					&& (TimeBuff_Hex[4]<0x24)  && (TimeBuff_Hex[5]<0x60) && (TimeBuff_Hex[6]<0x60) && (TimeBuff_Hex[7]>=1) && (TimeBuff_Hex[7]<=0x7) )
				{
					OSBsp.Device.RTC.ConfigExtTime(TimeBuff_Hex,0);   //写入时间
					g_Printf_dbg("LoRa Time Set Done!\r\n");
				}
				else
				{
					g_Printf_dbg("LoRa Time Set Failed!\r\n");
				}

			}
			if(Hal_CheckString(aRxBuff,"FF0301")) //复位设备
			{
				g_Printf_dbg("LoRa Reset Device OK!\r\n");
				OSTimeDly(500);
				hal_Reboot(); //******软件复位*******//
			}
			aRxNum = 0;
		}
	AppDataPointer->TransMethodData.LoRaStatus = LoRa_Idel;
}

void g_Device_check_Response(char *res)
{
	if(g_has_response == -1){
		g_has_response = 0;
		memset(g_response,0x0,256);
		strcpy(g_response,res);
	}

	if(Hal_CheckString(res,"+TXDONE"))
    {
		if(AppDataPointer->TransMethodData.LoRaNet)
            g_Printf_dbg("Send data to model OK...\r\n");
        // AppDataPointer->TransMethodData.LoRaStatus = LoRa_Send_Done;
	}else if(Hal_CheckString(res,"AT_BUSY_ERROR"))
    {
		g_Printf_dbg("internal state is busy...\r\n");
	} else if(Hal_CheckString(res,"+NJS:1") | Hal_CheckString(res,"Joined") )
    {
		g_Printf_dbg("LoRa is on Net...\r\n");
		AppDataPointer->TransMethodData.LoRaNet = 1;
        // AppDataPointer->TransMethodData.LoRaStatus = LoRa_Joined;
	}else if(Hal_CheckString(res,"+JON:") & Hal_CheckString(res,"OK") )
    {
		g_Printf_dbg("LoRa is on Net...\r\n");
		AppDataPointer->TransMethodData.LoRaNet = 1;
        // AppDataPointer->TransMethodData.LoRaStatus = LoRa_Joined;
	}else if(Hal_CheckString(res,"+JON:") & Hal_CheckString(res,"Failed") )
    {
		g_Printf_dbg("Join NET Failed...\r\n");
		AppDataPointer->TransMethodData.LoRaNet = 0;
        // AppDataPointer->TransMethodData.LoRaStatus = LoRa_Boot;
	}else if(Hal_CheckString(res,"ACK"))
    {
		g_Printf_dbg("Get Server ACK...\r\n");
		AppDataPointer->TransMethodData.LoRaSendStatus = 1;
	}
}

void  TransmitTaskStart (void *p_arg)
{
	uint8_t waitTime = 0;
	uint8_t temp = 0;
    (void)p_arg;   
    OSTimeDlyHMSM(0u, 0u, 0u, 100u);      
    g_Printf_info("%s ... ...\n",__func__);           
    while (DEF_TRUE) {               /* Task body, always written as an infinite loop.       */
        if(Hal_getCurrent_work_Mode() == 0){
            if(AppDataPointer->TransMethodData.LoRaStatus == LoRa_Power_off)
			{
				//NB-IoT 第一次开机时对NB上电操作，后续进入低功耗不关电
				// g_Printf_dbg("LoRa Power ON\r\n");
                OSBsp.Device.IOControl.PowerSet(LPModule_Power_On);		//打开LoRa电源
				//reset脚电平
                OSTimeDly(5000);
                // OSBsp.Device.IOControl.PowerSet(AIR202_Power_On);
                AppDataPointer->TransMethodData.LoRaStatus = LoRa_Power_on;
				g_Printf_dbg("LoRa Power ON\r\n");
                
            }
			else if(AppDataPointer->TransMethodData.LoRaStatus == LoRa_Power_on)
			{
                g_Device_LoRa_Init();
            }
			else if(AppDataPointer->TransMethodData.LoRaStatus == LoRa_Init_Done)
			{
				g_Device_LoRa_GetJoined();
			}
			else if(AppDataPointer->TransMethodData.LoRaStatus == LoRa_Join_Over)
			{
                 if( AppDataPointer->TerminalInfoData.DeviceStatus == DEVICE_STATUS_POWER_SCAN_OVER){
					char *data = Hal_Malloc(512*sizeof(char *));
					//  char response[128];
					//SeqNumber ++
					AppDataPointer->TransMethodData.SeqNumber++;
					if(AppDataPointer->TransMethodData.SeqNumber >= 65535)
						AppDataPointer->TransMethodData.SeqNumber = 1;
					Send_Buffer[5] = AppDataPointer->TransMethodData.SeqNumber/256;
					Send_Buffer[6] = AppDataPointer->TransMethodData.SeqNumber%256;
					//Voltage
					GetADCValue();
					data = MakeJsonBodyData(AppDataPointer);		//组包json并存储SD卡
					g_Printf_info("data:%s\r\n",data);
					
					//  memset(response,0x0,128);
					 //发送数据
					 if(AppDataPointer->TransMethodData.LoRaNet == 1)
					 {
						g_Device_LoRa_Send(Send_Buffer,34,15);
						OSTimeDly(2500);
						g_Device_LoRa_Receive();				
					 }
                 }    
            }
			else if(AppDataPointer->TransMethodData.LoRaStatus == LoRa_Idel)
			{
               
                Hal_EnterLowPower_Mode();
            }
            OSTimeDlyHMSM(0u, 0u, 0u, 200u);  
        }
    }
}

#endif //(TRANSMIT_TYPE == LoRa_F8L10D_Mode)

