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


uint8_t time_string_buf[16] = {};
/*��������ʱ��**************************************************/
uint8_t clock_flag;

//***********************************************************************
//                      DS1302_write_byte
//***********************************************************************
static void DS1302_write_byte(uint8_t addr, uint8_t d) 
{
	uint8_t i;
	RESET_SET;				    //����DS1302����
	/*д��Ŀ���ַ��addr*/
	IO_OUT;
	addr = addr & 0xFE;         //���λ����
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
	/*д�����ݣ�d*/
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
	RESET_CLR;				//ֹͣDS1302����
}
//***********************************************************************
//                   DS1302_read_byte
//***********************************************************************
static uint8_t DS1302_read_byte(uint8_t addr)
{
	uint8_t i;
	uint8_t temp;
	RESET_SET;					//����DS1302����
	/*д��Ŀ���ַ��addr*/
	IO_OUT;
	addr = addr | 0x01;         //���λ�ø�
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
	/*������ݣ�temp*/
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
	RESET_CLR;				//ֹͣDS1302����
	return temp;
}

//***********************************************************************
//                   向DS302写入时钟数据
//***********************************************************************
void DS1302_write_time(uint8_t *data,uint8_t dataType)
{
	DS1302_write_byte(DS1302_control_add,0x00);		//关闭写保护 
	DS1302_write_byte(DS1302_sec_add,0x80);	        //暂停 
	//DS1302_write_byte(DS1302_charger_add,0xa9);		//涓流充电 
	switch(dataType){
		case RealTime:{
			DS1302_write_byte(DS1302_year_add,time[1]);	//年 
			DS1302_write_byte(DS1302_month_add,time[2]);	//月
			DS1302_write_byte(DS1302_date_add,time[3]);	//日
			DS1302_write_byte(DS1302_hr_add,time[4]);		//时
			DS1302_write_byte(DS1302_min_add,time[5]);	//分
			DS1302_write_byte(DS1302_sec_add,time[6]);	//秒
			DS1302_write_byte(DS1302_day_add,time[7]);	//周
		}
		break;
		case AlarmData:{
			DS1302_write_byte(DS1302_clock_hr_add,alarm[0]);	//闹钟时
			DS1302_write_byte(DS1302_clock_min_add,alarm[1]);	//闹钟分
		}
		break;
		case InnerFlash:{
			DS1302_write_byte(DS1302_FLASH,innerram[0]);
		}
		break;
	}

	DS1302_write_byte(DS1302_control_add,0x80);		//打开写保护
}

//***********************************************************************
//                     从DS302读出时钟数据
//***********************************************************************

void DS1302_read_time(uint8_t *data,uint8_t dataType)
{ 
	switch(dataType){
		case RealTime:{
			data[1]=DS1302_read_byte(DS1302_year_add);	    //年
			data[2]=DS1302_read_byte(DS1302_month_add);	    //月
			data[3]=DS1302_read_byte(DS1302_date_add);	    //日
			data[4]=DS1302_read_byte(DS1302_hr_add);		    //时
			data[5]=DS1302_read_byte(DS1302_min_add);		    //分
			data[6]=(DS1302_read_byte(DS1302_sec_add))&0x7F;  //秒
			data[7]=DS1302_read_byte(DS1302_day_add);		    //周
		}
		break;
		case AlarmData:{
			data[0]=DS1302_read_byte(DS1302_clock_hr_add);	//clock时
			data[1]=DS1302_read_byte(DS1302_clock_min_add);	//clock分
		}
		break;
		case InnerFlash:{
			data[0]=DS1302_read_byte(DS1302_FLASH);
		}
		break;
	}
}

//***********************************************************************
//               DS302初始化函数
//***********************************************************************
void g_Device_ExtRTC_Init(void) 
{
	RESET_CLR;			                        //RESET脚置低
	SCK_CLR;			                        //SCK脚置低
	RESET_OUT;			                        //RESET脚设置为输出
	SCK_OUT;			                        //SCK脚设置为输出
	DS1302_write_byte(DS1302_sec_add,0x00);	    //启动

	OSBsp.Device.RTC.ReadExtTime = DS1302_read_time;
	OSBsp.Device.RTC.ConfigExtTime = DS1302_write_time;
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
void g_Device_InnerRTC_Init(void)
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

////读系统自带的RTC函数
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

//         case RTC_RTCTEVIFG://ÿ���ӽ�һ���ж�
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
// 				if(min>=60)//ÿһСʱ
// 				{
// 					min=0;
// 					hour++;
// 	//				Flag_ResetMotor = 1;  //ÿ�����һ�ε����λ

// 	//				if(Flag_GPSPositionOK == 1)
// 	//				{
// 	//					Flag_GPSPositionOK = 0;
// 	//					GPS_3V_ON;  //��GPS_3V3�ĵ�Դ
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

// 					if(hour==5 | hour==17)//ÿ��������㡢������㣬��������
// 					{
// 	//					LoRaReJoin=1;
// 	//					LoRaNet=0;
// 					}
// 					if(hour>=24)//�µ�һ��
// 					{
// 						hour=0;
// 						AppDataPointer->TransMethodData.SeqNumber = 0;    //�������������0

// 						//******magicSTICKÿ�������һ��*******//
// 						switch(TerminalIndex)
// 						{
// 							case MagicSTICK_Station:
// 								Flag_ResetMotor = 1;  //ÿ�����һ�ε����λ
// 								break;
// 							default:
// 								break;
// 						}
// 						//******magicSTICKÿ�������һ�ε��˽���*******//
// 						switch(AccessoryIndex)   //�������
// 						{
// 							case GPS_Mode:
// 								//********��0ǰһ���GPS��λ��Ϣ*********//
// 	//							AppDataPointer->TransMethodData.GPSLat_Point = 0; //γ��
// 	//							AppDataPointer->TransMethodData.GPSLng_Point = 0; //����
// 								GPS_3V_ON;  //��GPS_3V3�ĵ�Դ
// 								break;
// 							default:
// 								break;
// 						}
// 					} //if(hour>=24)//�µ�һ��
// 				}
// 				switch(CommunicationIndex)
// 				{
// //					case GPRS_Mode:
// //						switch(min % App.Data.TerminalInfoData.SendPeriod)//����SendPeriod���ڹ���
// //						{
// //						//ע�⣺�ж��������ֻ�ñ�־λ�����ڴ�ӡ���˿ڲ�����ö�����while 1��
// //							//******�˳��͹���*******//
// //							case 0x00:
// //								__bic_SR_register_on_exit(LPM0_bits);	//�˳��͹���
// //								LowPower=0;
// //								break;
// //							//******�򿪵�Դ�����������翪ʼ����
// //							case 0x01:
// //								System.Device.Usart2.WriteString("Exit Low Power!\r\n");
// //								System.Device.Usart2.WriteString("Sensor Power On\r\n");
// //								AllSensorPowerOn();    //�����д�������Դ
// //								SenSor_3V_ON;          //��Sensor_3V3��485оƬ�ϵ�
// //								System.Device.Usart2.WriteString("Start to Scan Sensor\r\n");
// //								System.Device.Usart2.WriteString("GPRS Power On\r\n");
// //								ScanSensor = 1;	    //������ȡsensor�Ĳ���
// //								RestartGprs = 1;    //����GPRS  ??����λ����Ҫ���� ���Կ�������߶ϵ磬�ϵ磬����ʼ������connect֮ǰ
// //								SD_3V_ON;		    //�򿪶�ȡ���洢��Ҫ�ĵ�Դ
// //								break;
// //							case 0x02:			    //�رմ�������Դ��׼����������
// //								System.Device.Usart2.WriteString("Stop to Scan Sensor\r\n");
// //								System.Device.Usart2.WriteString("Sensor Power Off\r\n");
// //								SenSor_3V_OFF;         //�ر�Sensor_3V3��485оƬ����
// //								AllSensorPowerOff();   //�ص����д�������Դ
// //
// //								switch(AccessoryIndex) //�������
// //								{
// //									case GPS_Mode:
// //										GPS_3V_OFF;    //���ΪGPSʱ�ر�GPS��Դ
// //										break;
// //									default:
// //										break;
// //								}
// //								//�����������������崫����ר�á�������������//
// //	//							AllSensorPowerOn();    //�����д�������Դ
// //								//�����������������崫����END��������������//
// //								ScanSensor=0;	       //��ֹ��ȡsensor�Ĳ���
// //
// //								AllowSend=1;	       //������������
// //								LowPower=1;		       //������Ϻ����͹���
// //								break;
// //							default:
// //								break;
// //						}
// //						break;

// 					case GPRS_Mode:
// 						switch(min % App.Data.TerminalInfoData.SendPeriod)//����SendPeriod���ڹ���
// 						{
// 						//ע�⣺�ж��������ֻ�ñ�־λ�����ڴ�ӡ���˿ڲ�����ö�����while 1��
// 							//******�˳��͹���*******//
// 							case 0x01:
// 								__bic_SR_register_on_exit(LPM0_bits);	//�˳��͹���
// 								LowPower=0;
// 								break;
// 							//******�򿪵�Դ�����������翪ʼ����
// 							case 0x02:
// 								System.Device.Usart2.WriteString("Exit Low Power!\r\n");
// 								System.Device.Usart2.WriteString("Sensor Power On\r\n");
// 								AllSensorPowerOn();    //�����д�������Դ
// 								SenSor_3V_ON;          //��Sensor_3V3��485оƬ�ϵ�
// 								break;
// 							case 0x03:			    //��Ƭ��������ȡ����������
// 								System.Device.Usart2.WriteString("Start to Scan Sensor\r\n");
// 								System.Device.Usart2.WriteString("GPRS Power On\r\n");
// 								ScanSensor = 1;	    //������ȡsensor�Ĳ���
// 								RestartGprs = 1;    //����GPRS  ??����λ����Ҫ���� ���Կ�������߶ϵ磬�ϵ磬����ʼ������connect֮ǰ
// 								SD_3V_ON;		    //�򿪶�ȡ���洢��Ҫ�ĵ�Դ
// 								break;
// 							case 0x04:			    //�رմ�������Դ��׼����������
// 								System.Device.Usart2.WriteString("Stop to Scan Sensor\r\n");
// 								System.Device.Usart2.WriteString("Sensor Power Off\r\n");
// 								SenSor_3V_OFF;         //�ر�Sensor_3V3��485оƬ����
// 								AllSensorPowerOff();   //�ص����д�������Դ

// 								switch(AccessoryIndex) //�������
// 								{
// 									case GPS_Mode:
// 										GPS_3V_OFF;    //���ΪGPSʱ�ر�GPS��Դ
// 										break;
// 									default:
// 										break;
// 								}
// 								//�����������������崫����ר�á�������������//
// 	//							AllSensorPowerOn();    //�����д�������Դ
// 								//�����������������崫����END��������������//
// 								ScanSensor=0;	       //��ֹ��ȡsensor�Ĳ���

// 								AllowSend=1;	       //������������
// 								LowPower=1;		       //������Ϻ����͹���
// 								break;
// 							default:
// 								break;
// 						}
// 						break;
// 					case NBIoT_BC95_Mode:
// 						switch(min % App.Data.TerminalInfoData.SendPeriod)//����SendPeriod���ڹ���
// 						{
// 							//******�˳��͹���*******//
// 							case 0x01:
// 								__bic_SR_register_on_exit(LPM0_bits);	//�˳��͹���
// 								LowPower=0;
// 								break;
// 							//******�򿪵�Դ�����������翪ʼ����
// 							case 0x02:
// 								System.Device.Usart2.WriteString("Exit Low Power!\r\n");
// 								System.Device.Usart2.WriteString("Sensor Power On\r\n");
// 								AllSensorPowerOn();    //�����д�������Դ
// 								SenSor_3V_ON;          //��Sensor_3V3��485оƬ�ϵ�
// 								break;
// 							case 0x03:			    //��Ƭ��������ȡ����������
// 								System.Device.Usart2.WriteString("Start to Scan Sensor\r\n");
// 	//							System.Device.Usart2.WriteString("GPRS Power On\r\n");
// 								ScanSensor = 1;	    //������ȡsensor�Ĳ���
// 	//							RestartGprs = 1;    //����GPRS  ??����λ����Ҫ���� ���Կ�������߶ϵ磬�ϵ磬����ʼ������connect֮ǰ
// 								SD_3V_ON;		    //�򿪶�ȡ���洢��Ҫ�ĵ�Դ
// 								break;
// 							case 0x04:			    //�رմ�������Դ��׼����������
// 								System.Device.Usart2.WriteString("Stop to Scan Sensor\r\n");
// 								System.Device.Usart2.WriteString("Sensor Power Off\r\n");
// 								SenSor_3V_OFF;         //�ر�Sensor_3V3��485оƬ����
// 								AllSensorPowerOff();   //�ص����д�������Դ

// 								switch(AccessoryIndex) //�������
// 								{
// 									case GPS_Mode:
// 										GPS_3V_OFF;    //���ΪGPSʱ�ر�GPS��Դ
// 										break;
// 									default:
// 										break;
// 								}
// 								//�����������������崫����ר�á�������������//
// 		//							AllSensorPowerOn();    //�����д�������Դ
// 								//�����������������崫����END��������������//
// 								ScanSensor=0;	       //��ֹ��ȡsensor�Ĳ���

// 								AllowSend=1;	       //������������
// 								LowPower=1;		       //������Ϻ����͹���
// 								break;
// 							default:
// 								break;
// 						}
// 						break;

// 					case LoRa_F8L10D_Mode:
// 					case LoRa_S78S_Mode:
// 					case LoRa_OM402_Mode:
// 						switch(min % App.Data.TerminalInfoData.SendPeriod)//����SendPeriod���ڹ���
// 						{
// 							//******�˳��͹���*******//
// 							case 0x01:
// 								__bic_SR_register_on_exit(LPM0_bits);	//�˳��͹���
// 								LowPower=0;
// 								System.Device.Usart2.WriteString("Exit Low Power!\r\n");
// 								if(Flag_LoRa_OM402_JoinReset == 1)
// 								{
// 									Flag_LoRa_OM402_JoinReset = 0;
// 									Flag_OM402_JoinReset = 1;
// 								}
// 								break;
// 							//******�򿪵�Դ�����������翪ʼ����
// 							case 0x02:
// 								System.Device.Usart2.WriteString("Exit Low Power!\r\n");
// 								System.Device.Usart2.WriteString("Sensor Power On\r\n");
// 								AllSensorPowerOn();    //�����д�������Դ
// 								SenSor_3V_ON;          //��Sensor_3V3��485оƬ�ϵ�
// 								break;
// 							case 0x03:			    //��Ƭ��������ȡ����������
// 								System.Device.Usart2.WriteString("Start to Scan Sensor\r\n");
// 	//							System.Device.Usart2.WriteString("GPRS Power On\r\n");
// 								ScanSensor = 1;	    //������ȡsensor�Ĳ���
// 	//							RestartGprs = 1;    //����GPRS  ??����λ����Ҫ���� ���Կ�������߶ϵ磬�ϵ磬����ʼ������connect֮ǰ
// 								SD_3V_ON;		    //�򿪶�ȡ���洢��Ҫ�ĵ�Դ
// 								break;
// 							case 0x04:			    //�رմ�������Դ��׼����������
// 								System.Device.Usart2.WriteString("Stop to Scan Sensor\r\n");
// 								System.Device.Usart2.WriteString("Sensor Power Off\r\n");
// 								SenSor_3V_OFF;         //�ر�Sensor_3V3��485оƬ����
// 								AllSensorPowerOff();   //�ص����д�������Դ

// 								switch(AccessoryIndex) //�������
// 								{
// 									case GPS_Mode:
// 										GPS_3V_OFF;    //���ΪGPSʱ�ر�GPS��Դ
// 										break;
// 									default:
// 										break;
// 								}
// 								//�����������������崫����ר�á�������������//
// 		//							AllSensorPowerOn();    //�����д�������Դ
// 								//�����������������崫����END��������������//
// 								ScanSensor=0;	       //��ֹ��ȡsensor�Ĳ���

// 								AllowSend=1;	       //������������
// 								LowPower=1;		       //������Ϻ����͹���
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
						
// 						switch(Min_Mod % App.Data.TerminalInfoData.SendPeriod)//����SendPeriod���ڹ���
// 						{
// 							case 0x01:
// 								__bic_SR_register_on_exit(LPM0_bits);	//�˳��͹���
// 								LowPower=0;
// 								ForceLowPower = 0;
// 								System.Device.Usart2.WriteString("Exit Low Power!\r\n");
// 								System.Device.Usart2.WriteString("Sensor Power On\r\n");
// 								delay_ms(10);
// 								AllSensorPowerOn();    //�����д�������Դ��485��Դ
// 								break;
// 							//******�򿪵�Դ�����������翪ʼ����
// 							case 0x02:
// 								System.Device.Usart2.WriteString("Start to Scan Sensor\r\n");
// 								ScanSensor = 1; 	//������ȡsensor�Ĳ���
// 								break;
// 							case 0x03:
// 								ScanSensor = 0;
// 								SenSor_3V_OFF;		   //�ر�Sensor_3V3��485оƬ����
// 								AllSensorPowerOff();   //�ص����д�������Դ
// 								SD_3V_ON;			//�򿪶�ȡ���洢��Ҫ�ĵ�Դ
// 								System.Device.Usart2.WriteString("Stop to Scan Sensor\r\n");
// 								System.Device.Usart2.WriteString("Sensor Power Off\r\n");
// 								AllowSend=1;		   //������������
// 								RestartGprs = 1;
// //								System.Device.Usart2.WriteString("GPRS Power On\r\n");
// 								break;
// 							case 0x05:				//��Ƭ��������ȡ����������
// 								switch(AccessoryIndex) //�������
// 								{
// 									case GPS_Mode:
// 										GPS_3V_OFF;    //���ΪGPSʱ�ر�GPS��Դ
// 										break;
// 									default:
// 										break;
// 								}
// 								LowPower = 1; 		   //������Ϻ����͹���
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
// 			} //RTC_RTCTEVIFG://ÿ���ӽ�һ���ж�
//         	break;
//         case  RTC_RTCRDYIFG://ÿ���ӽ�һ���ж�
// 			{
// 				//P1OUT ^= BIT7;
// 			}
//         	break;
//         default:
//             break;
//     }
}
