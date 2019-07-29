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
* Filename      : g_DeviceIO.c
* Version       : V1.00
* Programmer(s) : GLZ
*********************************************************************************************************
*/
#include  <hal_layer_api.h>
#include  <bsp.h>


/*******************************************************************************
* 函数名		: SetAllSensorPowerOn
* 描述	    	: 打开/关闭传感器板所有传感器电源
* 输入参数  	: onoff
* 返回参数      : 无
*******************************************************************************/
static void SetAllSensorPowerOn(bool onoff)
{
	if(onoff == true){
		SenSor_3V_ON;        	//打开Sensor_3V3         //485芯片上电
		Sensor_12V_Base_ON;     //传感板12V总电源  Power ON
		Sensor_5V_Base_ON;      //打开传感板上5V总电压               //传感器接口5V总电压上电
		Sensor_12V_1_ON;        //打开传感板上12V_1        //1,2传感器接口12V上电
		Sensor_12V_2_ON;       	//打开传感板上12V_2        //3,4传感器接口12V上电
		Sensor_12V_3_ON;        //打开传感板上12V_3        //5,6传感器接口12V上电
		Sensor_5V_1_ON;        	//打开传感板上5V_1         //1,2传感器接口5V上电
		Sensor_5V_2_ON;        	//打开传感板上5V_2         //3,4传感器接口5V上电
		Sensor_5V_3_ON;        	//打开传感板上5V_3         //5,6传感器接口5V上电
		
#if (PRODUCT_TYPE == AliveNest_Station) 
		Send_485_Enable;
		hal_Delay_us(5);
		OSBsp.Device.Usart3.WriteNData(SwitchPMS5003T_Passive,PTCMDLength); //将状态切换为被动式
		hal_Delay_us(1);
		Recive_485_Enable;
		OSBsp.Device.Usart2.WriteString("PMS5003T Passive State Switch OK!\r\n");
#endif
	}else{
		SenSor_3V_OFF;          //关闭Sensor_3V3          //485芯片掉电
		Sensor_12V_base_OFF;    //关闭传感板上12V总电压    //传感器接口12V总电压掉电
		Sensor_5V_base_OFF; 	//关闭传感板上5V总电压     //传感器接口5V总电压掉电
		Sensor_12V_1_OFF;       //关闭传感板上12V_1        //1,2传感器接口12V掉电
		Sensor_12V_2_OFF;       //关闭传感板上12V_2        //3,4传感器接口12V掉电
		Sensor_12V_3_OFF;       //关闭传感板上12V_3        //5,6传感器接口12V掉电
		Sensor_5V_1_OFF;        //关闭传感板上5V_1         //1,2传感器接口5V掉电
		Sensor_5V_2_OFF;        //关闭传感板上5V_2         //3,4传感器接口5V掉电
		Sensor_5V_3_OFF;        //关闭传感板上5V_3         //5,6传感器接口5V掉电
	}
}


static void Power(ControlPower type)
{
	switch(type){
		case BaseBoard_Power_On:
			P5OUT |= BIT1;	//打开主板上5V电压 //FP6717控制管脚使能,VBUS-->5V转换
		break;
		case BaseBoard_Power_Off:
			P5OUT &=~BIT1;	//关闭主板上5V电压 //FP6717控制管脚失能,VBUS-->5V关闭
		break;
		case LPModule_Power_On:
			P4OUT |= BIT3;	//打开Socket_3V3 //传输板上插LoRa，NB模块时供电
		break;
		case LPModule_Power_Off:
			P4OUT &=~BIT3;	//关闭Socket_3V3 //传输板上插LoRa，NB模块时掉电
		break;
		case GPRS_Power_On:
			P5OUT |= BIT0;	//打开Socket_5V //传输板上插GPRS模块时供电
		break;
		case GPRS_Power_Off:
			P5OUT &=~BIT0;	//关闭Socket_5V //传输板上插GPRS模块时掉电
		break;
		case SDCARD_Power_On:
			P4OUT |= BIT0;	    //打开sd卡_3V3
		break;
		case SDCARD_Power_Off:
			P4OUT &=~BIT0;	    //关闭sd卡_3V3
		break;
		case GPS_Power_On:
			P4OUT |= BIT1;
		break;
		case GPS_Power_Off:
			P4OUT &=~BIT1;	    //关闭sd卡_3V3
		break;
		case SenSor_Power_On:
			P4OUT |= BIT2;      //打开Sensor_3V3         //485芯片上电
			P3OUT |= BIT2;	    //打开传感板上5V总电压 //传感器接口5V总电压上电
			P6OUT |= BIT2;	    //打开传感板上12V总电压 //传感器接口12V总电压上电
			P1OUT |= BIT4;	    //打开传感板上12V_1 //1,2传感器接口12V上电
			P1OUT |= BIT5;	    //打开传感板上12V_2 //3,4传感器接口12V上电
			P6OUT |= BIT6;	    //打开传感板上12V_3 //5,6传感器接口12V上电
			P1OUT |= BIT3;	    //打开传感板上5V_1 //1,2传感器接口5V上电
			P6OUT |= BIT4;	    //打开传感板上5V_2 //3,4传感器接口5V上电
			P6OUT |= BIT5;	    //打开传感板上5V_3 //5,6传感器接口5V上电	  
		break;
		case SenSor_Power_Off:
			P4OUT &=~BIT2;      //关闭Sensor_3V3         //485芯片掉电
			P3OUT &=~BIT2;	 	//关闭传感板上5V总电压 //传感器接口5V总电压掉电
			P6OUT &=~BIT2;	 	//关闭传感板上12V总电压 //传感器接口12V总电压掉电
			P1OUT &=~BIT4; 		//关闭传感板上12V_1 //1,2传感器接口12V掉电
			P1OUT &=~BIT5; 		//关闭传感板上12V_2 //3,4传感器接口12V掉电
			P6OUT &=~BIT6; 		//关闭传感板上12V_3 //5,6传感器接口12V掉电
			P1OUT &=~BIT3; 		//关闭传感板上5V_1 //1,2传感器接口5V掉电
			P6OUT &=~BIT4; 		//关闭传感板上5V_2 //3,4传感器接口5V掉电
			P6OUT &=~BIT5; 		//关闭传感板上5V_3 //5,6传感器接口5V掉电
		break;
		case Motor_Power_On:
			P6OUT |= BIT3;	    //打开传感板上电机 //传感器接口电机上电
		break;
		case Motor_Power_Off:
			P6OUT &=~BIT3;	 	//关闭传感板上电机 //传感器接口电机掉电
		break;
		case AIR202_Power_On:
			P2OUT |= BIT4;	    //打开传感板上电机 //传感器接口电机上电
			hal_Delay_sec(2);
			P2OUT &=~BIT4; 
		break;
		case AIR202_Power_Off:
			P2OUT &=~BIT4; 		//关闭传感板上电机 //传感器接口电机掉电
		break;
	}
}

static void ResetWirelesModule(void)
{
	/***********reset模块复位脚*******************/
	P2OUT &=~BIT4;  //复位管脚  IO_0管脚，接模块的NRST管脚
	hal_Delay_ms(100);hal_Delay_ms(100);hal_Delay_ms(100);
	P2OUT |= BIT4;
	hal_Delay_ms(100);hal_Delay_ms(100);hal_Delay_ms(100);
}

/*******************************************************************************
* 函数名      	: g_DeviceIO_Init
* 描述	  		: GPIO口初始化
* 输入参数  	: 无
* 返回参数  	: 无
*******************************************************************************/
void g_DeviceIO_Init(void)
{
	//Initialization of ports (all unused pins as outputs with low-level)
	P1OUT = 0x00;
	P1DIR = 0xFF;
	P2OUT = 0x00;
	P2DIR = 0xFF;
	P3OUT = 0x00;
	P3DIR = 0xFF;
	P4OUT = 0x00;
	P4DIR = 0xFF;        //1:输出模式  0：输入模式
	P5OUT = 0x00;
	P5DIR = 0xFF;
	P6OUT = 0x00;
	P6DIR = 0xFF;

	LED_ON;//打开指示灯

	//**************485控制预定义***************************//
	Recive_485_Enable;	  	    //485传感器接收使能
	Out485_Recive_Enable;  	    //485配件接收使能
	/*********************输入端口IO配置*******************/
	P6DIR &=~ (BIT0+BIT1); //IO口方式选择，1:输出模式  0：输入模式                 AD0-BAT   AD1-TEMP

    /*********************关掉无用的电源*******************/
	//*******主板电源******//
	Power(LPModule_Power_Off);	        //关闭Socket_3V3        //传输板上插LoRa，NB模块时掉电
	Power(GPRS_Power_Off);      		//关闭Socket_5V         //传输板上插GPRS模块时掉电
	Power(SDCARD_Power_Off);	        //关闭sd卡_3V3
	Power(GPS_Power_Off);          		//关闭GPS_3V3
	//******传感板电源*****//
	Power(SenSor_Power_Off);    		//关闭所有传感器电源
	Power(Motor_Power_Off);      		//关闭传感板上电机                       //传感器接口电机掉电
	Power(AIR202_Power_Off);
	/*********************打开需要用到的电源****************/
	Power(SDCARD_Power_On);            	//打开sd卡_3V3
	

	OSBsp.Device.IOControl.PowerSet = Power;
	OSBsp.Device.IOControl.ResetWirelesModule = ResetWirelesModule;
}


