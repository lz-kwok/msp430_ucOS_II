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

#if (TRANSMIT_TYPE == NBIoT_BC95_Mode)
// const char *g_30000IoT_HOST = "30000iot.cn:9001"; 
// const char *g_30000IoT_PATH = "/api/Upload/data/";
// const char *g_30000IoT_HOST = "47.111.88.91:6096"; 
// const char *g_30000IoT_PATH = "/iot/data/receive";

// enum CoordinateSystem{
// 	WGS_84 = 1,
// 	GCJ_02 = 2
// };

static char NB_tick = 0;
// static uint32_t HTTP_Status_Code = 0;
static int g_has_response = 0;
static char g_response[256];
static unsigned char Singal_data[6]={0};
static unsigned char SINR_data[5]={0};
static unsigned char ECL_data=0;

//static char TimeString[20] = "20170804 16:00:00";
/*******************************************************************************
* 函数名		: NB_Config
* 描述	    	: 配置NB模组参数
* 输入参数  	: *c--需要配置的指令，m--指令等待时间（m*100ms），t--配置失败重新尝试次数
* 返回参数  	: 0--失败，1--成功
*******************************************************************************/
unsigned char NB_Config(unsigned char *c , unsigned char m, unsigned char t)
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
* 函数名		: g_Device_NB_Restart
* 描述	    	: NB模块重启---BC35G，配置不自动连接
* 输入参数  	: 无
* 返回参数  	: 无
*******************************************************************************/
void g_Device_NB_Restart(void)
{
	NB_Config("AT+NCONFIG=AUTOCONNECT,FALSE\r\n",2,5); //关闭自动连接
	OSTimeDly(200);

	User_Printf("AT+NRB\r\n");
	OSTimeDly(5000);
	
	if(NB_Config("AT\r\n",5,5))
	{
		AppDataPointer->TransMethodData.NBStatus = NB_Boot;
	}
	else
	{
		AppDataPointer->TransMethodData.NBStatus = NB_Power_on;
	}
}

/*******************************************************************************
* 函数名		: g_Device_NB_Init
* 描述	    	: NB模块初始化---BC35G
* 输入参数  	: 无
* 返回参数  	: 初始化结果  0----失败；1----成功
*******************************************************************************/
char g_Device_NB_Init(void)
{
	if(AppDataPointer->TransMethodData.NBStatus == NB_Boot)
	{
		if(NB_Config("AT+CFUN=0\r\n",5,5) == 0) //关机
		{
			AppDataPointer->TransMethodData.NBStatus = NB_Power_on;
			return 0;
		}
		OSTimeDly(20);

		if(NB_Config("AT+NCDP=180.101.147.115\r\n",5,5) == 0) //配置NCDP
		{
			AppDataPointer->TransMethodData.NBStatus = NB_Power_on;
			return 0;
		}
		OSTimeDly(20);
		
		if(NB_Config("AT+CFUN=1\r\n",60,5) == 0) //开机
		{
			AppDataPointer->TransMethodData.NBStatus = NB_Power_on;
			return 0;
		}
		OSTimeDly(20);

		if(NB_Config("AT+CGDCONT=1,\"IP\",\"CTNB\"\r\n",5,5) == 0) //APN
		{
			AppDataPointer->TransMethodData.NBStatus = NB_Power_on;
			return 0;
		}
		OSTimeDly(20);

		if(NB_Config("AT+CGATT=1\r\n",5,5) == 0) //激活网络
		{
			AppDataPointer->TransMethodData.NBStatus = NB_Power_on;
			return 0;
		}
		OSTimeDly(20);
		AppDataPointer->TransMethodData.NBStatus = NB_Init_Done;
		return 1;
		//等待入网
	}
	
}
/*******************************************************************************
* 函数名		: g_Device_NB_GetIP
* 描述	    	: NB模块入网  BC35G
* 输入参数  	: 无
* 返回参数  	: 无
*******************************************************************************/
void g_Device_NB_GetIP(void)
{
	uint8_t ii = 0;
	//等待注册成功信息
	//+QLWEVTIND:0
	//+QLWEVTIND:3 
	Clear_Buffer(aRxBuff,&aRxNum);
	while ((aRxNum<35) & (ii < 30))
	{
		// if(String_Chk(aRxBuff,"+QLWEVTIND:0") == 1 && String_Chk(aRxBuff,"+QLWEVTIND:3") ==1)
		// {
		// 	AppDataPointer->TransMethodData.NBStatus = NB_Registered;
		// 	break;
		// }
		Clear_Buffer(aRxBuff,&aRxNum);
		User_Printf("AT+CGPADDR\r\n");
		g_Printf_dbg("AT+CGPADDR\r\n");
		OSTimeDlyHMSM(0u, 0u, 2u, 0u);
//		System.Device.Usart2.WriteString(dRxBuff);
		ii++;

	}
	if(ii > 30)
	{
		AppDataPointer->TransMethodData.NBStatus = NB_Power_on;
		g_Printf_dbg("GET IP Failed!\r\n");
	}
	else
	{
		AppDataPointer->TransMethodData.NBStatus = NB_Get_IP;
		g_Printf_dbg("GET IP Succeed!\r\n");
	}
	AppDataPointer->TransMethodData.NBStatus = NB_Registered;
	AppDataPointer->TransMethodData.NBNet = 1;
	
}
/*******************************************************************************
* 函数名		: g_Device_NB_Send
* 描述	    	: NB上报数据
* 输入参数  	: *data--准备上报内容，length--数据长度
* 返回参数  	: code
*******************************************************************************/
void g_Device_NB_Send(uint32_t *data ,uint8_t length)
{
	static unsigned char ii = 0;
	char buff[15];
	Clear_Buffer(aRxBuff,&aRxNum);
	sprintf(buff,"AT+NMGS=%d,",length);
	User_Printf(buff);
    // User_Printf("AT+NMGS=34,");
    for(ii=0;ii<34;ii++)
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
/*******************************************************************************
* 函数名		: g_Device_NBSignal
* 描述	    	: NB查看信号质量
* 输入参数  	: 无
* 返回参数  	: 无
*******************************************************************************/
void g_Device_NBSignal(void)
{
	unsigned char *a;
	unsigned char i=0,n=0;
	int dataTemp=0;
	static uint8_t Singal_Less=0;
	Clear_Buffer(aRxBuff,&aRxNum);

	User_Printf("AT+NUESTATS\r\n");
	OSTimeDly(200);			//等待串口接收
	
	//处理信号值，若多次无信号，则复位NB，重新入网
	//get the RSRP
	a=strstr(aRxBuff,"Signal power:");//用于判断字符串str2是否是str1的子串。如果是，则该函数返回str2在str1中首次出现的地址；否则，返回NULL。
	if(a!=NULL)
	{
		while(*(a+13)!='\r')
		{
			Singal_data[i]=*(a+13);
			i++;
			a++;
		}
		Singal_data[i]='\n';
		dataTemp=0;
		for(n=1;n<i;n++)
		{
			dataTemp=dataTemp*10 + Singal_data[n]-0x30;
		}
		dataTemp=0-dataTemp;
		Send_Buffer[27] = (dataTemp & 0xFF00) >> 8;
		Send_Buffer[28] = dataTemp & 0xFF;
	}
	if(dataTemp==-32768)		//无信号
	{
		Singal_Less++;
	}
	if(Singal_Less>=2)
	{
		Singal_Less=0;
		AppDataPointer->TransMethodData.NBNet = 0;
		AppDataPointer->TransMethodData.NBStatus = NB_Power_on;
		g_Device_NB_Init();
	}
	//get the ECL
	i=0;
	dataTemp = 0;
	a=strstr(aRxBuff,"ECL:");
	if(a!=NULL)
	{
		dataTemp=*(a+4)-0x30;
		Send_Buffer[31] = (uint8_t)dataTemp;
	}

	//get the SINR
	i=0;
	dataTemp = 0;
	a=strstr(aRxBuff,"SNR:");
	if(a!=NULL)
	{
		while(*(a+4)!='\r')
		{
			SINR_data[i]=*(a+4);
			i++;
			a++;
		}
		SINR_data[i]='\n';
		dataTemp=0;
		if(SINR_data[0]=='-')
		{
			for(n=1;n<i;n++)
			{
				dataTemp=dataTemp*10 + SINR_data[n]-0x30;
			}
			dataTemp=0-dataTemp;
		}
		else
		{
			for(n=0;n<i;n++)
			{
				dataTemp=dataTemp*10 + SINR_data[n]-0x30;
			}
		}
		Send_Buffer[29] = (dataTemp & 0xFF00) >> 8;
		Send_Buffer[30] = dataTemp & 0xFF;
	}
}
/*******************************************************************************
* 函数名		: g_Device_NB_Receive
* 描述	    	: NB接收下发数据
* 输入参数  	: 无
* 返回参数  	: 无
*******************************************************************************/
void g_Device_NB_Receive(void)
{
	if(Hal_CheckString(aRxBuff,"OK"))
	{
		AppDataPointer->TransMethodData.NBStatus = NB_Send_Done;
		g_Printf_dbg("NB send data ok\r\n");
		Clear_Buffer(aRxBuff,&aRxNum);
		User_Printf("AT+NMGR\r\n");
		OSTimeDlyHMSM(0u, 0u, 0u, 200u);
	}
	AppDataPointer->TransMethodData.NBStatus = NB_Idel;
}


void  TransmitTaskStart (void *p_arg)
{
    (void)p_arg;   
    OSTimeDlyHMSM(0u, 0u, 0u, 100u);      
    g_Printf_info("%s ... ...\n",__func__);           
    while (DEF_TRUE) {               /* Task body, always written as an infinite loop.       */
        if(Hal_getCurrent_work_Mode() == 0){
            if(AppDataPointer->TransMethodData.NBStatus == NB_Power_off)
			{
				//NB-IoT 第一次开机时对NB上电操作，后续进入低功耗不关电
				g_Printf_dbg("Turn ON NB Power\r\n");
                OSBsp.Device.IOControl.PowerSet(LPModule_Power_On);		//打开NB电源
				//reset脚电平
               OSTimeDly(5000);
                // OSBsp.Device.IOControl.PowerSet(AIR202_Power_On);
                AppDataPointer->TransMethodData.NBStatus = NB_Power_on;
				g_Printf_dbg("NB Power ON\r\n");
                
            }
			else if(AppDataPointer->TransMethodData.NBStatus == NB_Power_on)
			{
				g_Device_NB_Restart();
            }
			else if(AppDataPointer->TransMethodData.NBStatus == NB_Boot)
			{
                g_Device_NB_Init();
            }
			else if(AppDataPointer->TransMethodData.NBStatus == NB_Init_Done)
			{
				g_Device_NB_GetIP();
			}
			else if(AppDataPointer->TransMethodData.NBStatus == NB_Registered)
			{
                 if( AppDataPointer->TerminalInfoData.DeviceStatus == DEVICE_STATUS_POWER_SCAN_OVER){
                     char *data = Hal_Malloc(512*sizeof(char *));
                     char response[128];
                     data = MakeJsonBodyData(AppDataPointer);		//组包json并存储SD卡
                     g_Printf_info("data:%s\r\n",data);
                     memset(response,0x0,128);
					 //检查信号质量
					 g_Device_NBSignal();
					 //发送数据
					 g_Device_NB_Send(Send_Buffer,34);
					 OSTimeDly(500);
					 g_Device_NB_Receive();
                    //  int code = g_Device_http_post(g_30000IoT_HOST,g_30000IoT_PATH,null,data,response,15);
                    //  Hal_Free(data);
                    //  if(code == 200)
					//  {
                    //      g_Printf_info("response : %s \r\n",response);   //对response解析，可以执行配置或ota操作
                    //      AppDataPointer->TransMethodData.GPRSStatus = GPRS_Http_Post_Done;
                    //  }
					//  else
					//  {    //这里可以做失败重发操作
                    //      g_Printf_dbg("http_post failed\r\n");
                    //  }                        
                 }    
            }
			else if(AppDataPointer->TransMethodData.NBStatus == NB_Idel)
			{
                // OSBsp.Device.IOControl.PowerSet(AIR202_Power_On);
                Hal_EnterLowPower_Mode();
            }
            OSTimeDlyHMSM(0u, 0u, 0u, 200u);  
        }
    }
}

#endif //(TRANSMIT_TYPE == NBIoT_BC95_Mode)

