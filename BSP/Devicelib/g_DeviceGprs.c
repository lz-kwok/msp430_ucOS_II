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

enum CoordinateSystem{
	WGS_84 = 1,
	GCJ_02 = 2
};

static char gprs_tick = 0;


//static char TimeString[20] = "20170804 16:00:00";

/*******************************************************************************
* 函数名		: g_Device_GPRS_Init
* 描述	    	: GPRS模块初始化
* 输入参数  	: 无
* 返回参数  	: 无
*******************************************************************************/
void g_Device_GPRS_Init(void)
{
	if(AppDataPointer->TransMethodData.GPRSStatus == GPRS_Waitfor_SMSReady){
		g_Printf_info("Start Init GPRS\r\n");
		hal_Delay_ms(10);
		g_Printf_info("Please Wait ...\r\n");
		hal_Delay_ms(10);

		g_Printf_dbg("AT\r\n");
		User_Printf("AT\r\n");           //AT同步，与主机端同步波特率
		OSTimeDly(500);

		g_Printf_dbg("ATE0\r\n");
		User_Printf("ATE0\r\n");         //关闭回显，初始化阶段关闭
		OSTimeDly(500);
		AppDataPointer->TransMethodData.GPRSStatus == GPRS_Preinit;
	}else if(AppDataPointer->TransMethodData.GPRSStatus == GPRS_Preinit){
		if((AppDataPointer->TransMethodData.GPRSNet == 0)&&
				(AppDataPointer->TransMethodData.GPRSAttached == 0)){
			g_Printf_dbg("AT+CSQ\r\n");
			User_Printf("AT+CSQ\r\n");   //查询信号质量		
			gprs_tick ++;
			OSTimeDly(1000);
			if(gprs_tick == 5){
				gprs_tick = 0;
				g_Printf_info("%s failed : Poor signal quality\r\n",__func__);
			}	
		}else if((AppDataPointer->TransMethodData.GPRSNet == 1)&&
				(AppDataPointer->TransMethodData.GPRSAttached == 0)){
			g_Printf_dbg("AT+CGATT?\r\n");
			User_Printf("AT+CGATT?\r\n"); 		//查询GPRS附着状态 ，0表示分离，1表示附着
			gprs_tick ++;
			OSTimeDly(1000);	
			if((gprs_tick == 5)&&(AppDataPointer->TransMethodData.GPRSAttached == 0)){
				gprs_tick = 0;
				g_Printf_info("Manual gain access to network\r\n");
				g_Printf_dbg("AT+CGATT=1\r\n"); //手动附着GPRS网络
				User_Printf("AT+CGATT=1\r\n");
				OSTimeDly(1000);
			}	
		}else if((AppDataPointer->TransMethodData.GPRSNet == 1)&&
				(AppDataPointer->TransMethodData.GPRSAttached == 1)){
#ifdef SIM800C
			g_Printf_dbg("AT+CIPSHUT\r\n");
			User_Printf("AT+CIPSHUT\r\n");
#endif
			OSTimeDly(2000);
			AppDataPointer->TransMethodData.GPRSStatus = GPRS_Http_Init_Done;
#ifdef AIR202
			AppDataPointer->TransMethodData.GPRSStatus = GPRS_Http_Preinit;
			AppDataPointer->TransMethodData.GPRSATStatus = GPRS_Waitfor_OK;
#endif
		}	
	}
#ifdef AIR202
	else if(AppDataPointer->TransMethodData.GPRSStatus == GPRS_Http_Preinit){
		gprs_tick = 0;
		while(gprs_tick<7) 
		{
			if(gprs_tick == 0){
				if(AppDataPointer->TransMethodData.GPRSATStatus == GPRS_Waitfor_OK){
					g_Printf_dbg("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\r\n");
					User_Printf("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\r\n");
					OSTimeDly(1000);
				}else{
					gprs_tick = 1;
					AppDataPointer->TransMethodData.GPRSATStatus = GPRS_Waitfor_OK;
				}
			}else if(gprs_tick == 1){
				if(AppDataPointer->TransMethodData.GPRSATStatus == GPRS_Waitfor_OK){
					g_Printf_dbg("AT+SAPBR=3,1,\"APN\",\"CMNET\"\r\n");
					User_Printf("AT+SAPBR=3,1,\"APN\",\"CMNET\"\r\n");
					OSTimeDly(1000);
				}else{
					gprs_tick = 2;
					AppDataPointer->TransMethodData.GPRSATStatus = GPRS_Waitfor_OK;
				}
			}else if(gprs_tick == 2){
				if(AppDataPointer->TransMethodData.GPRSATStatus == GPRS_Waitfor_OK){
					g_Printf_dbg("AT+SAPBR=5,1\r\n");
					User_Printf("AT+SAPBR=5,1\r\n");
					OSTimeDly(1000);
				}else{
					gprs_tick = 3;
					AppDataPointer->TransMethodData.GPRSATStatus = GPRS_Waitfor_OK;
				}
			}else if(gprs_tick == 3){
				if(AppDataPointer->TransMethodData.GPRSATStatus == GPRS_Waitfor_OK){
					g_Printf_dbg("AT+SAPBR =1,1\r\n");
					User_Printf("AT+SAPBR =1,1\r\n");
					OSTimeDly(1000);
				}else{
					gprs_tick = 4;
					AppDataPointer->TransMethodData.GPRSATStatus = GPRS_Waitfor_GetIP;
				}
			}else if(gprs_tick == 4){         
				if(AppDataPointer->TransMethodData.GPRSATStatus == GPRS_Waitfor_GetIP){
					AppDataPointer->TransMethodData.Http_Cid = 0;
					memset(aRxBuff,0x0,256);
					aRxNum = 0;
					g_Printf_dbg("AT+SAPBR=2,1\r\n");
					User_Printf("AT+SAPBR=2,1\r\n");
					OSTimeDly(1000);
				}else{
					gprs_tick = 5;
					AppDataPointer->TransMethodData.GPRSATStatus = GPRS_Waitfor_OK;
				}
			}else if(gprs_tick == 5){         
				if(AppDataPointer->TransMethodData.GPRSATStatus == GPRS_Waitfor_OK){
					g_Printf_dbg("AT+HTTPINIT\r\n");
					User_Printf("AT+HTTPINIT\r\n");
					OSTimeDly(1000);
				}else{
					gprs_tick = 6;
					AppDataPointer->TransMethodData.GPRSATStatus = GPRS_Waitfor_OK;
				}
			}else if(gprs_tick == 6){          
				if(AppDataPointer->TransMethodData.GPRSATStatus == GPRS_Waitfor_OK){
					User_Printf("AT+HTTPPARA=\"CID\",%d\r\n",AppDataPointer->TransMethodData.Http_Cid);
					OSTimeDly(1000);
				}else{
					gprs_tick ++;
					AppDataPointer->TransMethodData.GPRSATStatus = GPRS_Waitfor_OK;
					AppDataPointer->TransMethodData.GPRSStatus = GPRS_Http_Init_Done;
				}
			}
		}
		gprs_tick = 0;
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
* 输入参数  	: host,path,apikey,data,response,timeout(sec)
* 返回参数  	: code
*******************************************************************************/
int g_Device_http_post(const char *host,const char* path,const char *apikey,const char *data,
                      char *response,int timeout)
{
	int timer = 0;
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
	uint32_t datalen = 0;
	int g_err = 0;
	struct hal_timeval now;		
	struct hal_timeval tmp_tv;
	int32_t	sub_timeout_sec;
	Hal_GetTimeOfDay(&now);
	while(g_err == 0){
		if(gprs_tick == 0){      //设置Http会话参数
			if(AppDataPointer->TransMethodData.GPRSATStatus == GPRS_Waitfor_OK){
				User_Printf("AT+HTTPPARA=\"URL\",\"%s%s\"\r\n",host,path);
				OSTimeDly(1000);
			}else{
				gprs_tick = 1;
				AppDataPointer->TransMethodData.GPRSATStatus = GPRS_Waitfor_OK;
			}
		}else if(gprs_tick == 1){      //
			if(AppDataPointer->TransMethodData.GPRSATStatus == GPRS_Waitfor_OK){
				datalen = strlen(data);
				User_Printf("AT+HTTPDATA=%d,100000\r\n",datalen);
				OSTimeDly(1000);
				if(AppDataPointer->TransMethodData.GPRSATStatus == GPRS_Waitfor_Download){
					User_Printf("%s\r\n",data);
					gprs_tick = 2;
					OSTimeDly(1000);
				}

				AppDataPointer->TransMethodData.GPRSATStatus = GPRS_Waitfor_OK;
			}
		}else if(gprs_tick == 2){      //
			if(AppDataPointer->TransMethodData.GPRSATStatus == GPRS_Waitfor_OK){
				User_Printf("AT+HTTPACTION=1\r\n");
				OSTimeDly(1000);
			}else if(AppDataPointer->TransMethodData.GPRSATStatus == GPRS_Get_HTTPACT){
				gprs_tick = 3;
				User_Printf("AT+HTTPREAD\r\n");
				OSTimeDly(1000);
				AppDataPointer->TransMethodData.GPRSATStatus = GPRS_Waitfor_OK;
			}
		}else if(gprs_tick == 3){
			if(AppDataPointer->TransMethodData.GPRSATStatus == GPRS_Waitfor_OK){
				User_Printf("AT+HTTPTERM\r\n");    //根据需要看是否需结束Http服务
				OSTimeDly(1000);
			}else{
				gprs_tick = 0;
				// HttpStart = 0;
				AppDataPointer->TransMethodData.GPRSATStatus = GPRS_Waitfor_OK;
				AppDataPointer->TransMethodData.GPRSStatus = GPRS_Init_Done;

				g_err = 1;
			}
		}

		Hal_GetTimeOfDay(&tmp_tv);
		sub_timeout_sec = tmp_tv.tv_sec - now.tv_sec;
		if(sub_timeout_sec > timeout){
			if((gprs_tick < 2)&&(gprs_tick > 0))){
				g_Printf_info("%s para set timerout\r\n",__func__);						
			}else if(gprs_tick == 2){
				g_Printf_info("%s failed . http timerout\r\n",__func__);
				gprs_tick = 3;
				User_Printf("AT+HTTPTERM\r\n");   //结束Http服务
				OSTimeDly(1000);
				AppDataPointer->TransMethodData.GPRSATStatus = GPRS_Waitfor_OK;

				g_err = -1;
			}
		}
	}

	return g_err;
#endif
}

void g_Device_check_Response(char *res)
{
	char response[256];
	memset(response,0x0,256);
	strcpy(response,res);

#ifdef SIM800C
	if(Hal_CheckString(response,"CONNECT OK")){
		g_Printf_dbg("Connect OK\r\n");
		AppDataPointer->TransMethodData.GPRSConnect = 1;
	}else if(Hal_CheckString(response,"SEND OK")){
		// App.Data.GprsSendStatus = 1;
		g_Printf_dbg("Send OK\r\n");
	} else if(Hal_CheckString(response,"CLOSED")){
		g_Printf_dbg("Web Closed\r\n");
		// AllowSend=0;
		// GPRSReset;
		// Ctr_GPRS_OFF;  //鍏抽棴GPRS
	}else if(Hal_CheckString(response,"+CSQ:")){
		if (Hal_CheckString(response,"0,0")){
			g_Printf_dbg("CSQ Faild!\r\n");
			AppDataPointer->TransMethodData.GPRSNet = 0;
		}else{
			g_Printf_dbg("CSQ OK!\r\n");
			AppDataPointer->TransMethodData.GPRSNet = 1;
			gprs_tick = 0;
		}
	}else if(Hal_CheckString(response,"+CGATT:")){
		if (Hal_CheckString(response,"0")){
			g_Printf_dbg("Attached Faild!\r\n");
			AppDataPointer->TransMethodData.GPRSAttached = 0;
		}else{
			g_Printf_dbg("Attached OK!\r\n");
			AppDataPointer->TransMethodData.GPRSAttached = 1;
			gprs_tick = 0;
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
				g_Printf_dbg("Attached Faild!\r\n");
				AppDataPointer->TransMethodData.GPRSAttached = 0;
			}else
			{
				g_Printf_dbg("Attached OK!\r\n");
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
			// memcpy(iotTokenBuf,response,strlen(response));
			AppDataPointer->TransMethodData.GPRSATStatus = GPRS_Get_OK;
		}
	}
	else if((AppDataPointer->TransMethodData.GPRSStatus == GPRS_Mqtt_Preinit)
	||(AppDataPointer->TransMethodData.GPRSStatus == GPRS_Http_Preinit)||(AppDataPointer->TransMethodData.GPRSStatus == GPRS_Http_Init_Done)){
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
			// memset(mqttbuf,0x0,512);
			// memcpy(mqttbuf,response,strlen(response));
		}
	}
#endif
}

#endif //(TRANSMIT_TYPE == GPRS_Mode)

