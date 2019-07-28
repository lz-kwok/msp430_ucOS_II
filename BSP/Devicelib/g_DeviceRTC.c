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
/*????????????????**************************************************/
uint8_t clock_flag;

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
	RESET_CLR;			                        //RESET????�
	SCK_CLR;			                        //SCK????�
	RESET_OUT;			                        //RESET?????????
	SCK_OUT;			                        //SCK?????????
	DS1302_write_byte(DS1302_sec_add,0x00);	    //???

	OSBsp.Device.RTC.ReadExtTime = DS1302_read_time;
	OSBsp.Device.RTC.ConfigExtTime = DS1302_write_time;
}

//***********************************************************************
//??????????�
//***********************************************************************
void Create_TimeData(uint8_t *p1,uint8_t *p2)
{
	uint8_t time_buf[8];
	memset(time_buf,0x0,8);
	DS1302_read_time(time_buf,RealTime);

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
//??????????�
//***********************************************************************
void Create_TimeString(uint8_t *p1,uint8_t *p2)
{
	uint8_t time_buf[8];
	memset(time_buf,0x0,8);
	DS1302_read_time(time_buf,RealTime);

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

//??????RTC???????�
void g_Device_InnerRTC_Init(void)
{
    RTCCTL01 = RTCMODE + RTCBCD + RTCHOLD + RTCTEV_0 + RTCSSEL_0;		//  Min. changed interrupt
    RTCHOUR = 0x00;        //???????�2000?�0?�0?�00:00:00
    RTCMIN  = 0x00;
    RTCSEC  = 0x00;
    RTCDAY  = 0x00;
    RTCMON  = 0x00;
    RTCYEAR = 0x2000;
    RTCCTL01 &= ~RTCHOLD;//?????????

    RTCCTL0 |= RTCTEVIE;//?????????????�
//    RTCCTL0 |= RTCRDYIE;//?????????????�
}

//????????TC???
void Write_info_RTC(uint8_t *time)
{
    RTCCTL01 |= RTCHOLD;//?????????

    RTCYEAR_L   = time[1];
    RTCMON      = time[2];
    RTCDAY      = time[3];
    RTCHOUR     = time[4];
    RTCMIN      = time[5];
    RTCSEC      = time[6];
    RTCADOWDAY  = time[7];

    RTCCTL01 &= ~RTCHOLD;//?????????
//    RTCCTL0 |= RTCTEVIE;//?????????????�
//    RTCCTL0 |= RTCRDYIE;//?????????????�
}

////?????????RTC???
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

        case RTC_RTCTEVIFG://????????????????
			{

			} //RTC_RTCTEVIFG://????????????????
        	break;
        case  RTC_RTCRDYIFG://????????????????
			{
				//P1OUT ^= BIT7;
			}
        	break;
        default:
            break;
    }
}
