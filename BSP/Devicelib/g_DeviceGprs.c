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

#if (TRANSMIT_TYPE == GPRS_Mode)

uint8_t GprsInitStatus = 0;
uint8_t WirelessStatus = (enum GprsStatus)PowerOff;

enum CoordinateSystem{
	WGS_84 = 1,
	GCJ_02 = 2
};


//static char TimeString[20] = "20170804 16:00:00";

/*******************************************************************************
* 函数名		: g_Device_GPRS_Init
* 描述	    	: GPRS模块初始化
* 输入参数  	: 无
* 返回参数  	: 无
*******************************************************************************/
void g_Device_GPRS_Init(void)
{
#ifdef SIM800C
	static char gprs_tick = 0;

	OSBsp.Device.Usart2.WriteString("Start Init GPRS\r\n");
	hal_Delay_ms(100);
	OSBsp.Device.Usart2.WriteString("Please Wait ...\r\n");
	hal_Delay_ms(100);

	OSBsp.Device.Usart2.WriteString("AT\r\n");
	User_Printf("AT\r\n");           //AT同步，与主机端同步波特率
	hal_Delay_sec(1);

	OSBsp.Device.Usart2.WriteString("ATE0\r\n");
	User_Printf("ATE0\r\n");         //关闭回显，初始化阶段关闭
	hal_Delay_sec(1);

	gprs_tick = 0;
	while( (AppDataPointer->TransMethodData.GPRSNet == 0)&&(gprs_tick<5) )
	{
		gprs_tick++;
		OSBsp.Device.Usart2.WriteString("AT+CSQ\r\n");
		User_Printf("AT+CSQ\r\n");   //查询信号质量
		hal_Delay_sec(2);
	}

	gprs_tick = 0;
	while( (AppDataPointer->TransMethodData.GPRSAttached == 0)&&(gprs_tick<5) )
	{
		gprs_tick++;
		OSBsp.Device.Usart2.WriteString("AT+CGATT?\r\n");
		User_Printf("AT+CGATT?\r\n"); 		//查询GPRS附着状态 ，0表示分离，1表示附着
		hal_Delay_sec(2);
		if (AppDataPointer->TransMethodData.GPRSAttached == 0)
		{
			OSBsp.Device.Usart2.WriteString("AT+CGATT=1\r\n"); //手动附着GPRS网络
			User_Printf("AT+CGATT=1\r\n");
			hal_Delay_sec(2);
		}
	}

	OSBsp.Device.Usart2.WriteString("AT+CIPSHUT\r\n");
	User_Printf("AT+CIPSHUT\r\n");
	hal_Delay_sec(5);

	if( (AppDataPointer->TransMethodData.GPRSNet == 0)||(AppDataPointer->TransMethodData.GPRSAttached == 0))
	{
		OSBsp.Device.Usart2.WriteString("GPRS Init Failed!!!\r\n");
	}
	if( (AppDataPointer->TransMethodData.GPRSNet == 1)&&(AppDataPointer->TransMethodData.GPRSAttached == 1) )
	{
		OSBsp.Device.Usart2.WriteString("GPRS Init Succeess!\r\n");
	}
#endif
}

/*******************************************************************************
* 函数名		: g_Device_Establish_TCP_Connection
* 描述	    	: 建立TCP链接
* 输入参数  	: 无
* 返回参数  	: 无
*******************************************************************************/
void g_Device_Establish_TCP_Connection(const char *ip,uint32_t port)
{
//	//Saas
//	User_Printf("AT+CIPSTART=\"TCP\",\"114.55.93.183\",\"8081\"\r\n"); //建立TCP连接
	//OneNet      OneBoxLabDevice
	User_Printf("AT+CIPSTART=\"TCP\",\"%s\",\"%d\"\r\n",ip,port);   //建立TCP连接
}

/*******************************************************************************
* 函数名		: GprsHttpPost
* 描述	    	: http上报数据
* 输入参数  	: host,path,apikey,data,response,timeout
* 返回参数  	: code
*******************************************************************************/
int g_Device_http_post(const char *host,const char* path,const char *apikey,const char *data,
                      char *response,int timeout)
{
#ifdef SIM800C
    uint32_t datalen = 0;
	datalen = strlen(data);

	User_Printf("AT+CIPSEND\r\n");
	hal_Delay_ms(100);
	hal_Delay_ms(100);

    //Saas*******************************//
	User_Printf("POST %s HTTP/1.1\r\n",path);
	if(apikey != null){
		User_Printf("api-key:%s\r\n",apikey);
	}
	User_Printf("Host:%s\r\n",host);

	User_Printf("Accept:*/*\r\n");
	User_Printf("Content-Length:%d\r\n",datalen);
	User_Printf("Content-Type:text/html\r\n");
	User_Printf("Connection:close\r\n");
	User_Printf("\r\n");

    User_Printf("%s\r\n",data);

	hal_Delay_ms(100);
	OSBsp.Device.Usart0.WriteData(0x1A);      //发送数据完成结束符0x1A,16进制发送
	User_Printf("\r\n");

    OSBsp.Device.Usart2.WriteString(data);

	static char timetick = 0;
	while();
#endif

#ifdef AIR202
	static char postTick = 0;  
	uint32_t datalen = 0;

	if(PostTick == 0){      //设置Http会话参数
		if(AppDataPointer->TransMethodData.GPRSATStatus == GPRS_Waitfor_OK){
			User_Printf("AT+HTTPPARA=\"URL\",\"%s%s\"\r\n",host,path);
			hal_Delay_sec(2);
		}else{
			PostTick = 1;
			AppDataPointer->TransMethodData.GPRSATStatus = GPRS_Waitfor_OK;
		}
	}else if(PostTick == 1){      //
		if(AppDataPointer->TransMethodData.GPRSATStatus == GPRS_Waitfor_OK){
			datalen = strlen(data);
			User_Printf("AT+HTTPDATA=%d,100000\r\n",datalen);
			hal_Delay_sec(2);
			if(AppDataPointer->TransMethodData.GPRSATStatus == GPRS_Waitfor_Download){
				User_Printf("%s\r\n",data);
				PostTick = 2;
				hal_Delay_sec(1);
			}

			AppDataPointer->TransMethodData.GPRSATStatus = GPRS_Waitfor_OK;
		}
	}else if(PostTick == 2){      //
		if(AppDataPointer->TransMethodData.GPRSATStatus == GPRS_Waitfor_OK){
			User_Printf("AT+HTTPACTION=1\r\n");
			hal_Delay_sec(2);
		}else if(AppDataPointer->TransMethodData.GPRSATStatus == GPRS_Get_HTTPACT){
			PostTick = 3;
			User_Printf("AT+HTTPREAD\r\n");
			hal_Delay_sec(2);
			AppDataPointer->TransMethodData.GPRSATStatus = GPRS_Waitfor_OK;
		}
	}else if(PostTick == 3){
		if(AppDataPointer->TransMethodData.GPRSATStatus == GPRS_Waitfor_OK){
			User_Printf("AT+HTTPTERM\r\n");    //根据需要看是否需结束Http服务
			hal_Delay_sec(2);
		}else{
			PostTick = 0;
			HttpStart = 0;
			AppDataPointer->TransMethodData.GPRSATStatus = GPRS_Waitfor_OK;
			AppDataPointer->TransMethodData.GPRSStatus = GPRS_Init_Done;
		}
	}
#endif
}

void g_Device_check_Response(char *res)
{
	char response[256];
	memset(response,0x0,256);
	strcpy(response,res);

#ifdef SIM800C
	if(Hal_CheckString(response,"CONNECT OK")){
		OSBsp.Device.Usart2.WriteString("Connect OK\r\n");
		AppDataPointer->TransMethodData.GPRSConnect = 1;
	}else if(Hal_CheckString(response,"SEND OK")){
		// App.Data.GprsSendStatus = 1;
		OSBsp.Device.Usart2.WriteString("Send OK\r\n");
	} else if(Hal_CheckString(response,"CLOSED")){
		System.Device.Usart2.WriteString("Web Closed\r\n");
		// AllowSend=0;
		// GPRSReset;
		// Ctr_GPRS_OFF;  //鍏抽棴GPRS
	}else if(Hal_CheckString(response,"+CSQ:")){
		if (Hal_CheckString(response,"0,0")){
			OSBsp.Device.Usart2.WriteString("CSQ Faild!\r\n");
			AppDataPointer->TransMethodData.GPRSNet = 0;
		}else{
			OSBsp.Device.Usart2.WriteString("CSQ OK!\r\n");
			AppDataPointer->TransMethodData.GPRSNet = 1;
		}
	}else if(Hal_CheckString(response,"+CGATT:")){
		if (Hal_CheckString(response,"0")){
			OSBsp.Device.Usart2.WriteString("Attached Faild!\r\n");
			AppDataPointer->TransMethodData.GPRSAttached = 0;
		}else{
			OSBsp.Device.Usart2.WriteString("Attached OK!\r\n");
			AppDataPointer->TransMethodData.GPRSAttached = 1;
		}
	}
#endif

#ifdef AIR202
	if(AppDataPointer->TransMethodData.GPRSStatus == GPRS_Waitfor_SMSReady){
		if(Hal_CheckString(response,"SMS Ready")){
			AppDataPointer->TransMethodData.GPRSStatus = GPRS_Preinit;
		}else if(Hal_CheckString(response,"+CSQ:")){
			if (Hal_CheckString(response,"0,0")){
				AppDataPointer->TransMethodData.GPRSNet = 0;
			}else{
				AppDataPointer->TransMethodData.GPRSNet = 1;
				AppDataPointer->TransMethodData.GPRSStatus = GPRS_Preinit;
			}
		}
	}else if(AppDataPointer->TransMethodData.GPRSStatus == GPRS_Preinit){
		if(Hal_CheckString(response,"+CGATT:"))
		{
			if (Hal_CheckString(response,"0"))
			{
				System.Device.Usart2.WriteString("Attached Faild!\r\n");
				AppDataPointer->TransMethodData.GPRSAttached = 0;
			}else
			{
				System.Device.Usart2.WriteString("Attached OK!\r\n");
				AppDataPointer->TransMethodData.GPRSAttached = 1;
			}
		}
	}else if(AppDataPointer->TransMethodData.GPRSStatus == GPRS_Init_Done){
		if((Hal_CheckString(response,"OK"))||(Hal_CheckString(response,"DOWNLOAD"))){
			AppDataPointer->TransMethodData.GPRSATStatus = GPRS_Get_OK;
		}else if(Hal_CheckString(response,"+HTTPACTION: 1")){
			AppDataPointer->TransMethodData.GPRSATStatus = GPRS_Waitfor_Token;
			memset(response,0x0,256);
		}else if(Hal_CheckString(response,"iotToken")){
			memcpy(iotTokenBuf,response,strlen(response));
			AppDataPointer->TransMethodData.GPRSATStatus = GPRS_Get_OK;
		}
	}
	else if((AppDataPointer->TransMethodData.GPRSStatus == GPRS_Mqtt_Preinit)
	||(AppDataPointer->TransMethodData.GPRSStatus == GPRS_Hqtt_Preinit)||(AppDataPointer->TransMethodData.GPRSStatus == GPRS_Hqtt_Init_Done)){
		if(Hal_CheckString(response,"CONNECT OK")){
			AppDataPointer->TransMethodData.GPRSATStatus = GPRS_Connect_OK;
		}else if(Hal_CheckString(response,"CONNACK OK")){
			AppDataPointer->TransMethodData.GPRSATStatus = GPRS_Connack_OK;
		}else if(Hal_CheckString(response,"OK")){
			AppDataPointer->TransMethodData.GPRSATStatus = GPRS_Get_OK;
		}else if(Hal_CheckString(response,"+HTTPREAD:")){
			memset(response,0x0,256);
		}else if(Hal_CheckString(response,"+SAPBR:")){
			AppDataPointer->TransMethodData.GPRSATStatus = GPRS_Waitfor_OK;
			AppDataPointer->TransMethodData.Http_Cid = response[8] - 0x30;
		}else if(Hal_CheckString(response,"DOWNLOAD")){
			AppDataPointer->TransMethodData.GPRSATStatus = GPRS_Waitfor_Download;
		}else if(Hal_CheckString(response,"+HTTPACTION: 1")){
			AppDataPointer->TransMethodData.GPRSATStatus = GPRS_Get_HTTPACT;
		}
	}
	else if(AppDataPointer->TransMethodData.GPRSStatus == GPRS_Mqtt_Init_Done){
		if(Hal_CheckString(response,"SUBACK")){
			AppDataPointer->TransMethodData.GPRSATStatus = GPRS_Waitfor_Suback;
		}else if(Hal_CheckString(response,"OK")){
			AppDataPointer->TransMethodData.GPRSATStatus = GPRS_Get_OK;
		}
	}else if(AppDataPointer->TransMethodData.GPRSStatus == GPRS_MQTT_Alive){
		if(Hal_CheckString(response,"PUBACK")){
			AppDataPointer->TransMethodData.GPRSATStatus = GPRS_MQTT_Enable_Pulish;
		}else if(Hal_CheckString(response,"+MSUB: ")){
			memset(mqttbuf,0x0,512);
			memcpy(mqttbuf,response,strlen(response));
		}
	}
#endif
}

#endif //(TRANSMIT_TYPE == GPRS_Mode)

