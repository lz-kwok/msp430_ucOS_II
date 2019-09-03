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
//断点续传使用
uint16_t BackupIndex = 0;
uint16_t StartFile = 1;
uint8_t FullFlag = 0;
char RespFile[10];
char Data_Backup[70];
uint8_t ResendData = 0;
uint8_t cacheBuf[7];


static unsigned char Singal_data[6]={0};
static unsigned char SINR_data[5]={0};
// static unsigned char ECL_data=0;

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
	uint8_t ii = 0;
	if(AppDataPointer->TransMethodData.NBStatus == NB_Boot)
	{
		NB_Config("AT+CFUN=0\r\n",5,5);
		OSTimeDly(100);
		NB_Config("ATE1\r\n",5,5);
		OSTimeDly(100);
		NB_Config("AT+NNMI=0\r\n",5,5);
		OSTimeDly(100);
		NB_Config("AT+NCDP=180.101.147.115\r\n",5,5);
		OSTimeDly(100);
		NB_Config("AT+CFUN=1\r\n",100,5);
		OSTimeDly(100);
		NB_Config("AT+CGDCONT=1,\"IP\",\"CTNB\"\r\n",5,5);
		OSTimeDly(100);
		NB_Config("AT+CGATT=1\r\n",5,5);
		OSTimeDly(100);
		Clear_Buffer(aRxBuff,&aRxNum);
		//等待入网
		while ((aRxNum<35) & (ii < 60))
		{
			// if(String_Chk(aRxBuff,"+QLWEVTIND:0") == 1 && String_Chk(aRxBuff,"+QLWEVTIND:3") ==1)
			// {
			// 	AppDataPointer->TransMethodData.NBStatus = NB_Registered;
			// 	break;
			// }
			Clear_Buffer(aRxBuff,&aRxNum);
			User_Printf("AT+CGPADDR\r\n");
			g_Printf_dbg("AT+CGPADDR\r\n");
			OSTimeDly(1000);
		//		System.Device.Usart2.WriteString(dRxBuff);
			ii++;

		}
		if(ii > 60)
		{
			// AppDataPointer->TransMethodData.NBStatus = NB_Power_on;
			g_Printf_dbg("GET IP Failed!\r\n");
		}
		else
		{
			// AppDataPointer->TransMethodData.NBStatus = NB_Get_IP;
			g_Printf_dbg("GET IP Succeed!\r\n");
			AppDataPointer->TransMethodData.NBNet = 1;
		}
		OSTimeDly(500);
		NB_Config("AT+NCONFIG=AUTOCONNECT,TRUE\r\n",2,5); //关闭自动连接
		OSTimeDly(500);
		AppDataPointer->TransMethodData.NBStatus = NB_Init_Done;
		return 1;
	}
	return 0;
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
	
	AppDataPointer->TransMethodData.NBStatus = NB_Registered;
}
/*******************************************************************************
* 函数名		: g_Device_NB_Send
* 描述	    	: NB上报数据Hex数组
* 输入参数  	: *data--准备上报内容，length--数据长度
* 返回参数  	: code
*******************************************************************************/
void g_Device_NB_Send(uint32_t *data ,uint8_t length)
{
	static unsigned char ii = 0;
	char buff[15];
	AppDataPointer->TransMethodData.NBSendStatus = 0;
	Clear_Buffer(aRxBuff,&aRxNum);
	sprintf(buff,"AT+NMGS=%d,",length);
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
/*******************************************************************************
* 函数名		: g_Device_NB_Send_Str
* 描述	    	: NB上报数据Hex数组
* 输入参数  	: *data--准备上报内容，length--数据长度
* 返回参数  	: code
*******************************************************************************/
void g_Device_NB_Send_Str(char *data ,uint8_t length)
{
	static unsigned char ii = 0;
	char buff[15];
	AppDataPointer->TransMethodData.NBSendStatus = 0;
	Clear_Buffer(aRxBuff,&aRxNum);
	sprintf(buff,"AT+NMGS=%d,",length);
	User_Printf(buff);
	User_Printf(data);
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
	uint8_t *Uart0_RxBuff;
	uint8_t Uart0_RxBuff_data[50];
	uint8_t Uart0_RxBuff_Num;
	uint16_t Temp_SendPeriod;
	unsigned char Flash_Tmp[14];  //flash操作中间变量
	unsigned char TimebuffNum=0;
	unsigned char TimeBuff_Hex[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //16进制的时间Buffer  2018年3月15号 20时50分00秒 星期4

	if(Hal_CheckString(aRxBuff,"OK"))
	{
		// AppDataPointer->TransMethodData.NBStatus = NB_Send_Done;
		AppDataPointer->TransMethodData.NBSendStatus = 1;
		g_Printf_dbg("NB send data ok\r\n");
		Clear_Buffer(aRxBuff,&aRxNum);
		Uart0_RxBuff_Num = 0;
		User_Printf("AT+NMGR\r\n");
		OSTimeDly(500);
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
					g_Printf_dbg("NB Set SendPeriod OK\r\n");
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
					g_Printf_dbg("NB Set SendPeriod Failed！\r\n");
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
					g_Printf_dbg("NB Time Set Done!\r\n");
				}
				else
				{
					g_Printf_dbg("NB Time Set Failed!\r\n");
				}

			}
			if(Hal_CheckString(aRxBuff,"FF0301")) //复位设备
			{
				g_Printf_dbg("NB Reset Device OK!\r\n");
				OSTimeDly(500);
				hal_Reboot(); //******软件复位*******//
			}
			aRxNum = 0;
		}
		else
		{
			g_Printf_dbg("No (correct) message downloaded!\r\n");
			aRxNum = 0;
		}
	}
	// AppDataPointer->TransMethodData.NBStatus = NB_Idel;
}
/*******************************************************************************
* 函数名            : CreatFileNum
* 描述	  : 生成存储文件名称
* 输入参数  : x	1——序号加；0——序号减
* 返回参数  : 无
*******************************************************************************/
void CreatFileNum(char x)
{
  if(x ==1)	//序号加
  {
    if(FullFlag == 0)
    {
		BackupIndex++;
		if(BackupIndex > MaxLength)
		{
			BackupIndex = 1;
			StartFile = BackupIndex + 1;
			FullFlag = 1;
		}
    }
    else
    {
		BackupIndex ++;
		if(BackupIndex > MaxLength)
		{
			BackupIndex = 1;
		}
		StartFile = BackupIndex + 1;
		if(StartFile > MaxLength)
		{
			StartFile = 1;
		}
    }
  }
  else			//序号减
  {
      if(FullFlag == 1)
      {
		BackupIndex--;
		if(BackupIndex == 0)
		{
			BackupIndex = MaxLength;
			FullFlag = 0;
		}
      }
      else
      {
//	  if(BackupIndex >= StartFile)
	  {
	      if(BackupIndex == StartFile)
	      {
			BackupIndex = 0;
			StartFile = 1;
	      }
	      else	//BackupIndex ！= StartFile 只能是大于关系
	      {
		  	BackupIndex --;
	      }
	  }
      }
  }
}
void Hex2Str(unsigned char *d,uint32_t *p,unsigned char Len, unsigned char offset)
{
	unsigned char i;
	for(i = 0; i<Len;i++)
	{
		d[2*i + offset] = (uint8_t)p[i]>>4;
		d[2*i+1 + offset] = (uint8_t)p[i]&0xf;
	}
	for(i = 0 + offset; i<Len*2 + offset;i++)
	{
	//	dst[i] = HexToChar(str[i]);
		if (d[i] < 10)
		{
			d[i] = d[i] + '0';
		}
		else
		{
			d[i] = d[i] -10 +'A';
		}
	}
}
void WriteStoreData(void)
{
	uint8_t tempBuffer[20];
	
	CreatFileNum(1);		//参数1   BackupIndex++;
	ltoa( (long)BackupIndex , RespFile);
	strcat(RespFile , ".txt");
	if(FullFlag)		//循环时需要先删除原来文件
	{
		del_txt("0:/INDEX",RespFile);
		// sprintf(tempBuffer,"Del file:%s\r\n",RespFile);
		// g_Printf_dbg(tempBuffer);
	}
	if(Write_ToDirTxt("0:/INDEX",RespFile,Data_Backup))			//临时存储，用于补发数据,需要存储成功
	{
		cacheBuf[0] = BackupIndex/256;
		cacheBuf[1] = BackupIndex%256;
		cacheBuf[2] = StartFile/256;
		cacheBuf[3] = StartFile%256;
		cacheBuf[4] = FullFlag%256;
		OSBsp.Device.InnerFlash.FlashRsvWrite(cacheBuf,5,infor_ChargeAddr,18);	//存储成功后保存文件名序号至Flash
		g_Printf_dbg(Data_Backup);
		g_Printf_dbg("\r\n");
		g_Printf_dbg(RespFile);
		g_Printf_dbg(" Write backup to SD\r\n");
	}
	else			//若存储失败则退回原来序号
	{
		if(FullFlag==0)
		{
			if(BackupIndex >= 1)
				BackupIndex --;
		}
		else
		{
			if(BackupIndex == 1)
				BackupIndex = MaxLength;	//超过一轮需转回最大值
			else
				BackupIndex --;
		}

	}
}
void GetStoreData(void)
{
	uint8_t temp = 0;
	while(BackupIndex >=1)
	{
		ltoa( (long)BackupIndex , RespFile);
		strcat(RespFile , ".txt");
		temp = Get_String("0:/INDEX" , RespFile , Data_Backup , 70);
		if( temp == 1)		
		{
			// BackupIndex--;
			ResendData = 1;		//补发数据标志位
			Data_Backup[68] = '\0';
			AppDataPointer->TransMethodData.NBStatus = NB_Init_Done;
			break;		//退出循环，准备发送数据
		}
		else			//没取到字符串
		{
			CreatFileNum(0);		//参数0   BackupIndex--;
		}
	}
	
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
                 if( AppDataPointer->TerminalInfoData.DeviceStatus == DEVICE_STATUS_POWER_SCAN_OVER)
				 {
					g_Printf_info("Scan over,data uploading\r\n");
					if(ResendData == 0)		//正常上报数据,需要累加SeqNum,采集电压，本地存储
					{
						char *data = Hal_Malloc(512*sizeof(char *));
						char response[128];
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
						memset(response,0x0,128);
						//检查信号质量
						g_Device_NBSignal();
						Hex2Str(Data_Backup,Send_Buffer,34,0);
					}
					//发送数据
					if(AppDataPointer->TransMethodData.NBNet == 1)
					{
						if(ResendData == 1)		//补发数据
						{
							g_Device_NB_Send_Str(Data_Backup,34);
							// ResendData = 0;
						}
						else					//正常上报数据
						{
							g_Device_NB_Send(Send_Buffer,34);
						}
						
						OSTimeDly(2500);	//等待5s
						g_Device_NB_Receive();
						if(AppDataPointer->TransMethodData.NBSendStatus == 1)	//确认帧发送成功,发送数据前会置0
						{
							if(ResendData)
							{
								ResendData = 0;
								del_txt("0:/INDEX",RespFile);				//删除临时存储，同时更改存储BackupIndex值
								CreatFileNum(0);		//参数0   BackupIndex++;
								cacheBuf[0] = BackupIndex/256;
								cacheBuf[1] = BackupIndex%256;
								cacheBuf[2] = StartFile/256;
								cacheBuf[3] = StartFile%256;
								cacheBuf[4] = FullFlag;
								OSBsp.Device.InnerFlash.FlashRsvWrite(cacheBuf,5,infor_ChargeAddr,18);
							}	
							if(BackupIndex >=1)
								GetStoreData();
							else
								AppDataPointer->TransMethodData.NBStatus = NB_Idel;
						}
						else
						{
							WriteStoreData();
							AppDataPointer->TransMethodData.NBStatus = NB_Idel;
						}
					}
					else
					{
						//存储数据等待下次补传
						WriteStoreData();
						AppDataPointer->TransMethodData.NBStatus = NB_Idel;
					}
                        
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

