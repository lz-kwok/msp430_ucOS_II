/*
*********************************************************************************************************
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
*
*                                      Texas Instruments MSP430
*                                               on the
*                                          MSP-EXP430F5259LP
*                                          Evaluation Board
*
* Filename      : g_DeviceRTC.c
* Version       : V1.00
* Programmer(s) : GLZ
*********************************************************************************************************
*/
#include <bsp.h>


/*定义初始时间，BCD码格式，下面时间2018-03-15，15:20:50，星期四*/
uint8_t time_buf[8] = {0x20,0x18,0x03,0x15,0x20,0x50,0x00,0x04};
/*定义初始时间string格式*/
uint8_t time_string_buf[16] = {};
/*定义闹钟时间**************************************************/
uint8_t clock_buf[2] = {0x09,0x02};
uint8_t ds1302_ram[1] = {0};
uint8_t clock_flag;

//***********************************************************************
//                      向DS1302写入一字节数据
//***********************************************************************
void DS1302_write_byte(uint8_t addr, uint8_t d) 
{
	uint8_t i;
	RESET_SET;				    //启动DS1302总线
	/*写入目标地址：addr*/
	IO_OUT;
	addr = addr & 0xFE;         //最低位置零
	for (i = 0; i < 8; i ++)
	{
		if (addr & 0x01)
		{
			IO_SET;
		}
		else
		{
			IO_CLR;
		}
		SCK_SET;
		SCK_CLR;
		addr = addr >> 1;
	}
	/*写入数据：d*/
	IO_OUT;
	for (i = 0; i < 8; i ++)
	{
		if (d & 0x01)
		{
			IO_SET;
		}
		else
		{
			IO_CLR;
		}
		SCK_SET;
		SCK_CLR;
		d = d >> 1;
	}
	RESET_CLR;				//停止DS1302总线
}
//***********************************************************************
//                   从DS1302读出一字节数据
//***********************************************************************
uint8_t DS1302_read_byte(uint8_t addr)
{
	uint8_t i;
	uint8_t temp;
	RESET_SET;					//启动DS1302总线
	/*写入目标地址：addr*/
	IO_OUT;
	addr = addr | 0x01;         //最低位置高
	for (i = 0; i < 8; i ++)
	{
		if (addr & 0x01)
		{
		    IO_SET;
		}
		else
		{
		    IO_CLR;
		}
		SCK_SET;
		SCK_CLR;
		addr = addr >> 1;
	}
	/*输出数据：temp*/
	IO_IN;
	for (i = 0; i < 8; i ++)
	{
		temp = temp >> 1;
		if (IO_R)
		{
		    temp |= 0x80;
		}
		else
		{
		    temp &= 0x7F;
		}
		SCK_SET;
		SCK_CLR;
	}
	RESET_CLR;				//停止DS1302总线
	return temp;
}

//***********************************************************************
//                   向DS302写入时钟数据
//***********************************************************************
void DS1302_write_time(void)
{
  DS1302_write_byte(DS1302_control_add,0x00);		//关闭写保护 
  DS1302_write_byte(DS1302_sec_add,0x80);	        //暂停 
  //DS1302_write_byte(DS1302_charger_add,0xa9);		//涓流充电 
  DS1302_write_byte(DS1302_year_add,time_buf[1]);	//年 
  DS1302_write_byte(DS1302_month_add,time_buf[2]);	//月
  DS1302_write_byte(DS1302_date_add,time_buf[3]);	//日
  DS1302_write_byte(DS1302_hr_add,time_buf[4]);		//时
  DS1302_write_byte(DS1302_min_add,time_buf[5]);	//分
  DS1302_write_byte(DS1302_sec_add,time_buf[6]);	//秒
  DS1302_write_byte(DS1302_day_add,time_buf[7]);	//周
//  DS1302_write_byte(DS1302_clock_hr_add,clock_buf[0]);	//闹钟时
//  DS1302_write_byte(DS1302_clock_min_add,clock_buf[1]);	//闹钟分
//  DS1302_write_byte(DS1302_FLASH,ds1302_ram[0]);
  DS1302_write_byte(DS1302_control_add,0x80);		//打开写保护
}

//***********************************************************************
//                     从DS302读出时钟数据
//***********************************************************************

void DS1302_read_time(void)  
{ 
  time_buf[1]=DS1302_read_byte(DS1302_year_add);	    //年
  time_buf[2]=DS1302_read_byte(DS1302_month_add);	    //月
  time_buf[3]=DS1302_read_byte(DS1302_date_add);	    //日
  time_buf[4]=DS1302_read_byte(DS1302_hr_add);		    //时
  time_buf[5]=DS1302_read_byte(DS1302_min_add);		    //分
  time_buf[6]=(DS1302_read_byte(DS1302_sec_add))&0x7F;  //秒
  time_buf[7]=DS1302_read_byte(DS1302_day_add);		    //周
  ds1302_ram[0]=DS1302_read_byte(DS1302_FLASH);
  clock_buf[0]=DS1302_read_byte(DS1302_clock_hr_add);	//clock时
  clock_buf[1]=DS1302_read_byte(DS1302_clock_min_add);	//clock分
}

//***********************************************************************
//               DS302初始化函数
//***********************************************************************
void DS1302_init(void) 
{
  RESET_CLR;			                        //RESET脚置低
  SCK_CLR;			                            //SCK脚置低
  RESET_OUT;			                        //RESET脚设置为输出
  SCK_OUT;			                            //SCK脚设置为输出
  DS1302_write_byte(DS1302_sec_add,0x00);	    //启动
}

//***********************************************************************
//生成时间戳函数
//***********************************************************************
void Create_TimeData(uint8_t *p1,uint8_t *p2)
{
	DS1302_read_time();//读取当前时间
	//数据日期合成
	p1[3]=p2[0]=((time_buf[0]>>4)&0x0f)+0x30;	    //2   year
	p1[4]=p2[1]=(time_buf[0]&0x0f)+0x30;   	        //0
	p1[5]=p2[2]=((time_buf[1]>>4)&0x0f)+0x30;	    //1
	p1[6]=p2[3]=(time_buf[1]&0x0f)+0x30;			//7

	p1[7]=p2[5]=((time_buf[2]>>4)&0x0f)+0x30;		//0		month
	p1[8]=p2[6]=(time_buf[2]&0x0f)+0x30; 			//3

	p1[9]=p2[8]=((time_buf[3]>>4)&0x0f)+0x30;		//0		day
	p1[10]=p2[9]=(time_buf[3]&0x0f)+0x30; 			//6

	p2[11]=((time_buf[4]>>4)&0x0f)+0x30;		    //2		hour
	p2[12]=(time_buf[4]&0x0f)+0x30; 			    //0

	p2[14]=((time_buf[5]>>4)&0x0f)+0x30;		    //5		min
	p2[15]=(time_buf[5]&0x0f)+0x30; 			    //6

	p2[17]=((time_buf[6]>>4)&0x0f)+0x30;	      	//0		sec
	p2[18]=(time_buf[6]&0x0f)+0x30; 			    //0
}
//***********************************************************************
//生成时间戳函数
//***********************************************************************
void Create_TimeString(uint8_t *p1,uint8_t *p2)
{
	DS1302_read_time();//读取当前时间
	//数据日期合成
	p1[3]=p2[0]=((time_buf[0]>>4)&0x0f)+0x30;	    //2   year
	p1[4]=p2[1]=(time_buf[0]&0x0f)+0x30;   	        //0
	p1[5]=p2[2]=((time_buf[1]>>4)&0x0f)+0x30;	    //1
	p1[6]=p2[3]=(time_buf[1]&0x0f)+0x30;			//7

	p1[7]=p2[4]=((time_buf[2]>>4)&0x0f)+0x30;		//0		month
	p1[8]=p2[5]=(time_buf[2]&0x0f)+0x30; 			//3

	p1[9]=p2[6]=((time_buf[3]>>4)&0x0f)+0x30;		//0		day
	p1[10]=p2[7]=(time_buf[3]&0x0f)+0x30; 			//6

	p2[9]=((time_buf[4]>>4)&0x0f)+0x30;		        //2		hour
	p2[10]=(time_buf[4]&0x0f)+0x30; 			    //0

	p2[12]=((time_buf[5]>>4)&0x0f)+0x30;		    //5		min
	p2[13]=(time_buf[5]&0x0f)+0x30; 			    //6

	p2[15]=((time_buf[6]>>4)&0x0f)+0x30;	      	//0		sec
	p2[16]=(time_buf[6]&0x0f)+0x30; 			    //0
}

//系统自带RTC初始化函数
void g_Device_RTC_Init(void)
{
    RTCCTL01 = RTCMODE + RTCBCD + RTCHOLD + RTCTEV_0 + RTCSSEL_0;		//  Min. changed interrupt
    RTCHOUR = 0x00;        //初始时间：2000年0月0日00:00:00
    RTCMIN  = 0x00;
    RTCSEC  = 0x00;
    RTCDAY  = 0x00;
    RTCMON  = 0x00;
    RTCYEAR = 0x2000;
    RTCCTL01 &= ~RTCHOLD;//启动实时时钟

    RTCCTL0 |= RTCTEVIE;//打开每分钟中断标志
//    RTCCTL0 |= RTCRDYIE;//打开每秒钟中断标志
}

//写系统自带RTC函数
void Write_info_RTC(uint8_t *time)
{
    RTCCTL01 |= RTCHOLD;//关闭实时时钟

    RTCYEAR_L   = time[1];
    RTCMON      = time[2];
    RTCDAY      = time[3];
    RTCHOUR     = time[4];
    RTCMIN      = time[5];
    RTCSEC      = time[6];
    RTCADOWDAY  = time[7];

    RTCCTL01 &= ~RTCHOLD;//启动实时时钟
//    RTCCTL0 |= RTCTEVIE;//打开每分钟中断标志
//    RTCCTL0 |= RTCRDYIE;//打开每秒钟中断标志
}

//读系统自带的RTC函数
void Read_info_RTC(uint8_t *time)
{
    time[0] = (uint8_t)RTCYEAR;
    time[1] = RTCMON;
    time[2] = RTCDAY;
    time[3] = RTCHOUR;
    time[4] = RTCMIN;
    time[5] = RTCSEC;
}


#pragma vector=RTC_VECTOR
__interrupt void RTC_ISR(void)
{
//     switch (__even_in_range(RTCIV, RTC_RT1PSIFG))
//     {
//         case RTC_NONE:
//             break;

//         case RTC_RTCTEVIFG://每分钟进一次中断
// 			{
// 				min++;
// 				if(Hour_Mod == 1){
// 					Min_Real ++;
// 					if(Min_Real >= 240)
// 						Min_Real = 0;
// 				}

// //				if(min==5)
// //				{
// //					switch(CommunicationIndex)
// //					{
// //						case LoRa_OM402_Mode:
// //							Flag_LoRa_OM402_JoinReset = 1;
// //							break;
// //						default:
// //							break;
// //					}
// //				}
// 				if(min>=60)//每一小时
// 				{
// 					min=0;
// 					hour++;
// 	//				Flag_ResetMotor = 1;  //每天进行一次电机复位

// 	//				if(Flag_GPSPositionOK == 1)
// 	//				{
// 	//					Flag_GPSPositionOK = 0;
// 	//					GPS_3V_ON;  //打开GPS_3V3的电源
// 	//				}
// 					if(hour==1)
// 					{
// 						switch(CommunicationIndex)
// 						{
// 							case LoRa_OM402_Mode:
// 								Flag_LoRa_OM402_JoinReset = 1;
// 								break;
// 							default:
// 								break;
// 						}
// 					}

// 					if(hour==5 | hour==17)//每天上午五点、下午五点，重新入网
// 					{
// 	//					LoRaReJoin=1;
// 	//					LoRaNet=0;
// 					}
// 					if(hour>=24)//新的一天
// 					{
// 						hour=0;
// 						AppDataPointer->TransMethodData.SeqNumber = 0;    //发送数据序号清0

// 						//******magicSTICK每天自清洁一次*******//
// 						switch(TerminalIndex)
// 						{
// 							case MagicSTICK_Station:
// 								Flag_ResetMotor = 1;  //每天进行一次电机复位
// 								break;
// 							default:
// 								break;
// 						}
// 						//******magicSTICK每天自清洁一次到此结束*******//
// 						switch(AccessoryIndex)   //配件类型
// 						{
// 							case GPS_Mode:
// 								//********清0前一天的GPS定位信息*********//
// 	//							AppDataPointer->TransMethodData.GPSLat_Point = 0; //纬度
// 	//							AppDataPointer->TransMethodData.GPSLng_Point = 0; //经度
// 								GPS_3V_ON;  //打开GPS_3V3的电源
// 								break;
// 							default:
// 								break;
// 						}
// 					} //if(hour>=24)//新的一天
// 				}
// 				switch(CommunicationIndex)
// 				{
// //					case GPRS_Mode:
// //						switch(min % App.Data.TerminalInfoData.SendPeriod)//根据SendPeriod周期工作
// //						{
// //						//注意：中断里面最好只置标志位，串口打印，端口操作最好都放入while 1中
// //							//******退出低功耗*******//
// //							case 0x00:
// //								__bic_SR_register_on_exit(LPM0_bits);	//退出低功耗
// //								LowPower=0;
// //								break;
// //							//******打开电源，传感器供电开始工作
// //							case 0x01:
// //								System.Device.Usart2.WriteString("Exit Low Power!\r\n");
// //								System.Device.Usart2.WriteString("Sensor Power On\r\n");
// //								AllSensorPowerOn();    //打开所有传感器电源
// //								SenSor_3V_ON;          //打开Sensor_3V3，485芯片上电
// //								System.Device.Usart2.WriteString("Start to Scan Sensor\r\n");
// //								System.Device.Usart2.WriteString("GPRS Power On\r\n");
// //								ScanSensor = 1;	    //允许读取sensor的操作
// //								RestartGprs = 1;    //重启GPRS  ??放置位置需要考虑 可以考虑在这边断电，上电，将初始化放在connect之前
// //								SD_3V_ON;		    //打开读取及存储需要的电源
// //								break;
// //							case 0x02:			    //关闭传感器电源，准备发送数据
// //								System.Device.Usart2.WriteString("Stop to Scan Sensor\r\n");
// //								System.Device.Usart2.WriteString("Sensor Power Off\r\n");
// //								SenSor_3V_OFF;         //关闭Sensor_3V3，485芯片掉电
// //								AllSensorPowerOff();   //关掉所有传感器电源
// //
// //								switch(AccessoryIndex) //配件类型
// //								{
// //									case GPS_Mode:
// //										GPS_3V_OFF;    //配件为GPS时关闭GPS电源
// //										break;
// //									default:
// //										break;
// //								}
// //								//×××××××气体传感器专用×××××××//
// //	//							AllSensorPowerOn();    //打开所有传感器电源
// //								//×××××××气体传感器END×××××××//
// //								ScanSensor=0;	       //禁止读取sensor的操作
// //
// //								AllowSend=1;	       //允许发送数据
// //								LowPower=1;		       //操作完毕后进入低功耗
// //								break;
// //							default:
// //								break;
// //						}
// //						break;

// 					case GPRS_Mode:
// 						switch(min % App.Data.TerminalInfoData.SendPeriod)//根据SendPeriod周期工作
// 						{
// 						//注意：中断里面最好只置标志位，串口打印，端口操作最好都放入while 1中
// 							//******退出低功耗*******//
// 							case 0x01:
// 								__bic_SR_register_on_exit(LPM0_bits);	//退出低功耗
// 								LowPower=0;
// 								break;
// 							//******打开电源，传感器供电开始工作
// 							case 0x02:
// 								System.Device.Usart2.WriteString("Exit Low Power!\r\n");
// 								System.Device.Usart2.WriteString("Sensor Power On\r\n");
// 								AllSensorPowerOn();    //打开所有传感器电源
// 								SenSor_3V_ON;          //打开Sensor_3V3，485芯片上电
// 								break;
// 							case 0x03:			    //单片机启动读取传感器数据
// 								System.Device.Usart2.WriteString("Start to Scan Sensor\r\n");
// 								System.Device.Usart2.WriteString("GPRS Power On\r\n");
// 								ScanSensor = 1;	    //允许读取sensor的操作
// 								RestartGprs = 1;    //重启GPRS  ??放置位置需要考虑 可以考虑在这边断电，上电，将初始化放在connect之前
// 								SD_3V_ON;		    //打开读取及存储需要的电源
// 								break;
// 							case 0x04:			    //关闭传感器电源，准备发送数据
// 								System.Device.Usart2.WriteString("Stop to Scan Sensor\r\n");
// 								System.Device.Usart2.WriteString("Sensor Power Off\r\n");
// 								SenSor_3V_OFF;         //关闭Sensor_3V3，485芯片掉电
// 								AllSensorPowerOff();   //关掉所有传感器电源

// 								switch(AccessoryIndex) //配件类型
// 								{
// 									case GPS_Mode:
// 										GPS_3V_OFF;    //配件为GPS时关闭GPS电源
// 										break;
// 									default:
// 										break;
// 								}
// 								//×××××××气体传感器专用×××××××//
// 	//							AllSensorPowerOn();    //打开所有传感器电源
// 								//×××××××气体传感器END×××××××//
// 								ScanSensor=0;	       //禁止读取sensor的操作

// 								AllowSend=1;	       //允许发送数据
// 								LowPower=1;		       //操作完毕后进入低功耗
// 								break;
// 							default:
// 								break;
// 						}
// 						break;
// 					case NBIoT_BC95_Mode:
// 						switch(min % App.Data.TerminalInfoData.SendPeriod)//根据SendPeriod周期工作
// 						{
// 							//******退出低功耗*******//
// 							case 0x01:
// 								__bic_SR_register_on_exit(LPM0_bits);	//退出低功耗
// 								LowPower=0;
// 								break;
// 							//******打开电源，传感器供电开始工作
// 							case 0x02:
// 								System.Device.Usart2.WriteString("Exit Low Power!\r\n");
// 								System.Device.Usart2.WriteString("Sensor Power On\r\n");
// 								AllSensorPowerOn();    //打开所有传感器电源
// 								SenSor_3V_ON;          //打开Sensor_3V3，485芯片上电
// 								break;
// 							case 0x03:			    //单片机启动读取传感器数据
// 								System.Device.Usart2.WriteString("Start to Scan Sensor\r\n");
// 	//							System.Device.Usart2.WriteString("GPRS Power On\r\n");
// 								ScanSensor = 1;	    //允许读取sensor的操作
// 	//							RestartGprs = 1;    //重启GPRS  ??放置位置需要考虑 可以考虑在这边断电，上电，将初始化放在connect之前
// 								SD_3V_ON;		    //打开读取及存储需要的电源
// 								break;
// 							case 0x04:			    //关闭传感器电源，准备发送数据
// 								System.Device.Usart2.WriteString("Stop to Scan Sensor\r\n");
// 								System.Device.Usart2.WriteString("Sensor Power Off\r\n");
// 								SenSor_3V_OFF;         //关闭Sensor_3V3，485芯片掉电
// 								AllSensorPowerOff();   //关掉所有传感器电源

// 								switch(AccessoryIndex) //配件类型
// 								{
// 									case GPS_Mode:
// 										GPS_3V_OFF;    //配件为GPS时关闭GPS电源
// 										break;
// 									default:
// 										break;
// 								}
// 								//×××××××气体传感器专用×××××××//
// 		//							AllSensorPowerOn();    //打开所有传感器电源
// 								//×××××××气体传感器END×××××××//
// 								ScanSensor=0;	       //禁止读取sensor的操作

// 								AllowSend=1;	       //允许发送数据
// 								LowPower=1;		       //操作完毕后进入低功耗
// 								break;
// 							default:
// 								break;
// 						}
// 						break;

// 					case LoRa_F8L10D_Mode:
// 					case LoRa_S78S_Mode:
// 					case LoRa_OM402_Mode:
// 						switch(min % App.Data.TerminalInfoData.SendPeriod)//根据SendPeriod周期工作
// 						{
// 							//******退出低功耗*******//
// 							case 0x01:
// 								__bic_SR_register_on_exit(LPM0_bits);	//退出低功耗
// 								LowPower=0;
// 								System.Device.Usart2.WriteString("Exit Low Power!\r\n");
// 								if(Flag_LoRa_OM402_JoinReset == 1)
// 								{
// 									Flag_LoRa_OM402_JoinReset = 0;
// 									Flag_OM402_JoinReset = 1;
// 								}
// 								break;
// 							//******打开电源，传感器供电开始工作
// 							case 0x02:
// 								System.Device.Usart2.WriteString("Exit Low Power!\r\n");
// 								System.Device.Usart2.WriteString("Sensor Power On\r\n");
// 								AllSensorPowerOn();    //打开所有传感器电源
// 								SenSor_3V_ON;          //打开Sensor_3V3，485芯片上电
// 								break;
// 							case 0x03:			    //单片机启动读取传感器数据
// 								System.Device.Usart2.WriteString("Start to Scan Sensor\r\n");
// 	//							System.Device.Usart2.WriteString("GPRS Power On\r\n");
// 								ScanSensor = 1;	    //允许读取sensor的操作
// 	//							RestartGprs = 1;    //重启GPRS  ??放置位置需要考虑 可以考虑在这边断电，上电，将初始化放在connect之前
// 								SD_3V_ON;		    //打开读取及存储需要的电源
// 								break;
// 							case 0x04:			    //关闭传感器电源，准备发送数据
// 								System.Device.Usart2.WriteString("Stop to Scan Sensor\r\n");
// 								System.Device.Usart2.WriteString("Sensor Power Off\r\n");
// 								SenSor_3V_OFF;         //关闭Sensor_3V3，485芯片掉电
// 								AllSensorPowerOff();   //关掉所有传感器电源

// 								switch(AccessoryIndex) //配件类型
// 								{
// 									case GPS_Mode:
// 										GPS_3V_OFF;    //配件为GPS时关闭GPS电源
// 										break;
// 									default:
// 										break;
// 								}
// 								//×××××××气体传感器专用×××××××//
// 		//							AllSensorPowerOn();    //打开所有传感器电源
// 								//×××××××气体传感器END×××××××//
// 								ScanSensor=0;	       //禁止读取sensor的操作

// 								AllowSend=1;	       //允许发送数据
// 								LowPower=1;		       //操作完毕后进入低功耗
// 								break;
// 							default:
// 								break;
// 						}
// 						break;
// 					case GPRS_AIR202_Mode:
// 						if(Hour_Mod == 1){
// 							Min_Mod = Min_Real;
// 						}else{
// 							Min_Mod = min;
// 						}
						
// 						switch(Min_Mod % App.Data.TerminalInfoData.SendPeriod)//根据SendPeriod周期工作
// 						{
// 							case 0x01:
// 								__bic_SR_register_on_exit(LPM0_bits);	//退出低功耗
// 								LowPower=0;
// 								ForceLowPower = 0;
// 								System.Device.Usart2.WriteString("Exit Low Power!\r\n");
// 								System.Device.Usart2.WriteString("Sensor Power On\r\n");
// 								delay_ms(10);
// 								AllSensorPowerOn();    //打开所有传感器电源及485电源
// 								break;
// 							//******打开电源，传感器供电开始工作
// 							case 0x02:
// 								System.Device.Usart2.WriteString("Start to Scan Sensor\r\n");
// 								ScanSensor = 1; 	//允许读取sensor的操作
// 								break;
// 							case 0x03:
// 								ScanSensor = 0;
// 								SenSor_3V_OFF;		   //关闭Sensor_3V3，485芯片掉电
// 								AllSensorPowerOff();   //关掉所有传感器电源
// 								SD_3V_ON;			//打开读取及存储需要的电源
// 								System.Device.Usart2.WriteString("Stop to Scan Sensor\r\n");
// 								System.Device.Usart2.WriteString("Sensor Power Off\r\n");
// 								AllowSend=1;		   //允许发送数据
// 								RestartGprs = 1;
// //								System.Device.Usart2.WriteString("GPRS Power On\r\n");
// 								break;
// 							case 0x05:				//单片机启动读取传感器数据
// 								switch(AccessoryIndex) //配件类型
// 								{
// 									case GPS_Mode:
// 										GPS_3V_OFF;    //配件为GPS时关闭GPS电源
// 										break;
// 									default:
// 										break;
// 								}
// 								LowPower = 1; 		   //操作完毕后进入低功耗
// 								break;
// 							case 0x06:
// 								System.Device.Usart2.WriteString("ForceLowPower\r\n");
// 								ForceLowPower = 1;
// 								break;
// 							case 0x07:
// 								Socket_5V_OFF;
// 				                Socket_3V_OFF;
// 								break;
// 							default:
// 								break;
// 						}
// 						break;

// 					default:
// 						break;
// 				}   //switch(CommunicationIndex)
// 			} //RTC_RTCTEVIFG://每分钟进一次中断
//         	break;
//         case  RTC_RTCRDYIFG://每秒钟进一次中断
// 			{
// 				//P1OUT ^= BIT7;
// 			}
//         	break;
//         default:
//             break;
//     }
}
