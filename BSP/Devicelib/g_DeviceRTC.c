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

static uint32_t g_MinuteTick = 0;

//***********************************************************************
//                      DS1302_write_byte
//***********************************************************************
static void DS1302_write_byte(uint8_t addr, uint8_t d) 
{
	uint8_t i;
	RESET_SET;				    //??????DS1302??????
	/*??????????????ddr*/
	IO_OUT;
	addr = addr & 0xFE;         //???????????�
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
	/*???????????d*/
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
	RESET_CLR;				//??DS1302??????
}
//***********************************************************************
//                   DS1302_read_byte
//***********************************************************************
static uint8_t DS1302_read_byte(uint8_t addr)
{
	uint8_t i;
	uint8_t temp;
	RESET_SET;					//??????DS1302??????
	/*??????????????ddr*/
	IO_OUT;
	addr = addr | 0x01;         //?????????�
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
	/*????????????emp*/
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
	RESET_CLR;				//??DS1302??????
	return temp;
}

//***********************************************************************
//                   ??S302?????????
//***********************************************************************
void DS1302_write_time(uint8_t *data,uint8_t dataType)
{
	DS1302_write_byte(DS1302_control_add,0x00);		//???????� 
	DS1302_write_byte(DS1302_sec_add,0x80);	        //??? 
	//DS1302_write_byte(DS1302_charger_add,0xa9);		//?????? 
	switch(dataType){
		case RealTime:{
			DS1302_write_byte(DS1302_year_add,data[1]);	//?� 
			DS1302_write_byte(DS1302_month_add,data[2]);	//?�
			DS1302_write_byte(DS1302_date_add,data[3]);	//?�
			DS1302_write_byte(DS1302_hr_add,data[4]);		//?�
			DS1302_write_byte(DS1302_min_add,data[5]);	//?�
			DS1302_write_byte(DS1302_sec_add,data[6]);	//?�
			DS1302_write_byte(DS1302_day_add,data[7]);	//?�
		}
		break;
		case AlarmData:{
			DS1302_write_byte(DS1302_clock_hr_add,data[0]);	//????�
			DS1302_write_byte(DS1302_clock_min_add,data[1]);	//????�
		}
		break;
		case InnerRam:{
			DS1302_write_byte(DS1302_FLASH,data[0]);
		}
		break;
	}

	DS1302_write_byte(DS1302_control_add,0x80);		//???????�
}

//***********************************************************************
//                     ??S302?????????
//***********************************************************************

void DS1302_read_time(uint8_t *data,uint8_t dataType)
{ 
	switch(dataType){
		case RealTime:{
			data[1]=DS1302_read_byte(DS1302_year_add);	    //?�
			data[2]=DS1302_read_byte(DS1302_month_add);	    //?�
			data[3]=DS1302_read_byte(DS1302_date_add);	    //?�
			data[4]=DS1302_read_byte(DS1302_hr_add);		    //?�
			data[5]=DS1302_read_byte(DS1302_min_add);		    //?�
			data[6]=(DS1302_read_byte(DS1302_sec_add))&0x7F;  //?�
			data[7]=DS1302_read_byte(DS1302_day_add);		    //?�
		}
		break;
		case AlarmData:{
			data[0]=DS1302_read_byte(DS1302_clock_hr_add);	//clock?�
			data[1]=DS1302_read_byte(DS1302_clock_min_add);	//clock?�
		}
		break;
		case InnerRam:{
			data[0]=DS1302_read_byte(DS1302_FLASH);
		}
		break;
	}
}

//***********************************************************************
//               DS302???????�
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
void g_Device_RTCstring_Creat(uint8_t *datetime,char *t_str)
{
	// t_str[0]=((datetime[0]>>4)&0x0f)+0x30;	    //year
	// t_str[1]=(datetime[0]&0x0f)+0x30;   	    //
	t_str[0]='2';	    //year
	t_str[1]='0';   	    //
	t_str[2]=((datetime[1]>>4)&0x0f)+0x30;	    //
	t_str[3]=(datetime[1]&0x0f)+0x30;			//

	t_str[4]=((datetime[2]>>4)&0x0f)+0x30;		//month
	t_str[5]=(datetime[2]&0x0f)+0x30; 			//
	
	t_str[6]=((datetime[3]>>4)&0x0f)+0x30;		//day
	t_str[7]=(datetime[3]&0x0f)+0x30; 			//
	t_str[8]=' ';
	t_str[9]=((datetime[4]>>4)&0x0f)+0x30;		 //hour
	t_str[10]=(datetime[4]&0x0f)+0x30; 			 //
	t_str[11]=':';
	t_str[12]=((datetime[5]>>4)&0x0f)+0x30;	 	 //min
	t_str[13]=(datetime[5]&0x0f)+0x30; 			 //
	t_str[14]=':';
	t_str[15]=((datetime[6]>>4)&0x0f)+0x30;	     //sec
	t_str[16]=(datetime[6]&0x0f)+0x30; 			 //
}

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

//????????TC???
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
    switch (__even_in_range(RTCIV, RTC_RT1PSIFG))
    {
        case RTC_NONE:
            break;

        case RTC_RTCTEVIFG:		//分钟
			{
				g_MinuteTick ++;
				g_Printf_info("g_MinuteTick wakeup %d times\r\n",g_MinuteTick);
				if(g_MinuteTick == App.Data.TerminalInfoData.SendPeriod){
					g_MinuteTick = 0;
					if(Hal_getCurrent_work_Mode() == 1){          //当前为低功耗状态
						__bic_SR_register_on_exit(LPM0_bits);
						WDTCTL  = WDT_MDLY_32;
						SFRIE1 |= 1;  
						Hal_ExitLowPower_Mode();
					}
					
				}
			} 
        	break;
        case  RTC_RTCRDYIFG:	//分钟
			{

        
			}
        	break;
        default:
            break;
    }
}
