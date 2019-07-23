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

#ifdef HAVE_GPRS_CONFIG

uint8_t GprsInitStatus = 0;
uint8_t WirelessStatus = (enum GprsStatus)PowerOff;

enum CoordinateSystem{
	WGS_84 = 1,
	GCJ_02 = 2
};

char HttpBody[200];


//static char TimeString[20] = "20170804 16:00:00";




/*******************************************************************************
* 函数名		: g_Device_GPRS_Init
* 描述	    	: GPRS模块初始化
* 输入参数  	: 无
* 返回参数  	: 无
*******************************************************************************/
void g_Device_GPRS_Init(void)
{
	unsigned char i;

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

	i = 0;
	while( (AppDataPointer->TransMethodData.GPRSNet == 0)&&(i<5) )
	{
		i++;
//		ResetFlag = 1;
		OSBsp.Device.Usart2.WriteString("AT+CSQ\r\n");
		User_Printf("AT+CSQ\r\n");   //查询信号质量
		hal_Delay_sec(2);
	}

	i = 0;
	while( (AppDataPointer->TransMethodData.GPRSAttached == 0)&&(i<5) )
	{
//		ResetFlag = 1;
		i++;
		OSBsp.Device.Usart2.WriteString("AT+CGATT?\r\n");
		User_Printf("AT+CGATT?\r\n"); //查询GPRS附着状态 ，0表示分离，1表示附着
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
}

/*******************************************************************************
* 函数名		: g_Device_Establish_TCP_Connection
* 描述	    	: 建立TCP链接
* 输入参数  	: 无
* 返回参数  	: 无
*******************************************************************************/
void g_Device_Establish_TCP_Connection(void)
{
//	//Saas
//	User_Printf("AT+CIPSTART=\"TCP\",\"114.55.93.183\",\"8081\"\r\n"); //建立TCP连接
	//OneNet      OneBoxLabDevice
	User_Printf("AT+CIPSTART=\"TCP\",\"183.230.40.33\",\"80\"\r\n");   //建立TCP连接
}
/*******************************************************************************
* 函数名		: GprsHttpPost
* 描述	    : http上报数据
* 输入参数  	: 无
* 返回参数  	: 无
*******************************************************************************/
void g_Device_http_post(DataStruct *DataPointer)
{
    char * jsonBody = (void *)0;
    uint32_t JsonLen = 0;

	User_Printf("AT+CIPSEND\r\n");
	hal_Delay_ms(100);
	hal_Delay_ms(100);

	jsonBody = MakeJsonBodyData(DataPointer);
    JsonLen = strlen(jsonBody);

    //Saas*******************************//
//	User_Printf("POST /api/Upload/data/ HTTP/1.1\r\n");
//	User_Printf("Host:114.55.93.183:8081\r\n");
	//OneNet******OneBoxLabDevice********//
//	User_Printf("POST /devices/31826723/datapoints?type=3 HTTP/1.1\r\n");
//	User_Printf("api-key:TJsvHaYjqKgGRi0O04bn17unPtk=\r\n");
//	User_Printf("Host:api.heclouds.com\r\n");
	//OneNet******OneBoxLabDevice2*******//
//	User_Printf("POST /devices/32151000/datapoints?type=3 HTTP/1.1\r\n");
//	User_Printf("api-key:iexYfESrOpwM3MjABGXDVcJm4rk=\r\n");
//	User_Printf("Host:api.heclouds.com\r\n");

	//OneNet******农田气象站*******//
	User_Printf("POST /devices/534702370/datapoints?type=3 HTTP/1.1\r\n");
	User_Printf("api-key:x5iu6iob5Eo8rwczXHwbp61zFig=\r\n");
	User_Printf("Host:api.heclouds.com\r\n");


	User_Printf("Accept:*/*\r\n");
	User_Printf("Content-Length:%d\r\n",JsonLen);
	User_Printf("Content-Type:text/html\r\n");
	User_Printf("Connection:close\r\n");

	User_Printf("\r\n");

    User_Printf("%s\r\n",jsonBody);

	hal_Delay_ms(100);
	OSBsp.Device.Usart0.WriteData(0x1A);      //发送数据完成结束符0x1A,16进制发送
	User_Printf("\r\n");

    sprintf(HttpBody,"%s\r\n",jsonBody);
    OSBsp.Device.Usart2.WriteString(HttpBody);

    free(jsonBody);
}



#endif