/*******************************************************************************
*
*    File Name:  g_DeviceIO.h
*    Revision:  1.0
*
*    Description:  IO初始化
*
*
* *****************************************************************************/
#ifndef G_DEVICEIO_H
#define G_DEVICEIO_H

//**************485控制预定义***************************//
#define Send_485_Enable		    P2OUT |= BIT5	    //485发送使能
#define Recive_485_Enable	    P2OUT &=~BIT5	    //485接收使能
//**************485配件输出控制预定义*********************//
#define Out485_Send_Enable	  P1OUT |= BIT6	    //485配件发送使能
#define Out485_Recive_Enable  P1OUT &=~BIT6	    //485配件接收使能
//**************LED控制预定义***************************//
#define LED_OFF                 P2OUT &=~ BIT1    //关闭单片机工作状态指示LED
#define LED_ON	                P2OUT |= BIT1;    //打开单片机工作状态指示LED

//***********************电源控制预定义***************************//
//**************主板电源**************//
#define Base_5V_ON		      P5OUT |= BIT1	//打开主板上5V电压 //FP6717控制管脚使能,VBUS-->5V转换
#define Base_5V_OFF		      P5OUT &=~BIT1	//关闭主板上5V电压 //FP6717控制管脚失能,VBUS-->5V关闭

#define Socket_3V_ON		  P4OUT |= BIT3	//打开Socket_3V3 //传输板上插LoRa，NB模块时供电
#define Socket_3V_OFF		  P4OUT &=~BIT3	//关闭Socket_3V3 //传输板上插LoRa，NB模块时掉电
#define Socket_5V_ON		  P5OUT |= BIT0	//打开Socket_5V //传输板上插GPRS模块时供电
#define Socket_5V_OFF		  P5OUT &=~BIT0	//关闭Socket_5V //传输板上插GPRS模块时掉电

#define SD_3V_ON		      P4OUT |= BIT0	    //打开sd卡_3V3
#define SD_3V_OFF		      P4OUT &=~BIT0	    //关闭sd卡_3V3
#define GPS_3V_ON			  P4OUT |= BIT1	    //打开GPS_3V3
#define GPS_3V_OFF			  P4OUT &=~BIT1	    //关闭GPS_3V3
//**************传感板电源**************//
#define SenSor_3V_ON		  P4OUT |= BIT2     //打开Sensor_3V3         //485芯片上电
#define SenSor_3V_OFF		  P4OUT &=~BIT2     //关闭Sensor_3V3         //485芯片掉电

#define Sensor_12V_Base_ON    P6OUT |= BIT2	    //打开传感板上12V总电压 //传感器接口12V总电压上电
#define Sensor_12V_base_OFF	  P6OUT &=~BIT2 	//关闭传感板上12V总电压 //传感器接口12V总电压掉电
#define Sensor_5V_Base_ON     P3OUT |= BIT2	    //打开传感板上5V总电压 //传感器接口5V总电压上电
#define Sensor_5V_base_OFF	  P3OUT &=~BIT2 	//关闭传感板上5V总电压 //传感器接口5V总电压掉电

#define Sensor_12V_1_ON		  P1OUT |= BIT4	    //打开传感板上12V_1 //1,2传感器接口12V上电
#define Sensor_12V_1_OFF      P1OUT &=~BIT4 	//关闭传感板上12V_1 //1,2传感器接口12V掉电
#define Sensor_12V_2_ON		  P1OUT |= BIT5	    //打开传感板上12V_2 //3,4传感器接口12V上电
#define Sensor_12V_2_OFF	  P1OUT &=~BIT5 	//关闭传感板上12V_2 //3,4传感器接口12V掉电
#define Sensor_12V_3_ON		  P6OUT |= BIT6	    //打开传感板上12V_3 //5,6传感器接口12V上电
#define Sensor_12V_3_OFF	  P6OUT &=~BIT6 	//关闭传感板上12V_3 //5,6传感器接口12V掉电
#define Sensor_5V_1_ON		  P1OUT |= BIT3	    //打开传感板上5V_1 //1,2传感器接口5V上电
#define Sensor_5V_1_OFF		  P1OUT &=~BIT3 	//关闭传感板上5V_1 //1,2传感器接口5V掉电
#define Sensor_5V_2_ON		  P6OUT |= BIT4	    //打开传感板上5V_2 //3,4传感器接口5V上电
#define Sensor_5V_2_OFF		  P6OUT &=~BIT4 	//关闭传感板上5V_2 //3,4传感器接口5V掉电
#define Sensor_5V_3_ON		  P6OUT |= BIT5	    //打开传感板上5V_3 //5,6传感器接口5V上电
#define Sensor_5V_3_OFF		  P6OUT &=~BIT5 	//关闭传感板上5V_3 //5,6传感器接口5V掉电

#define Motor_Power_ON		  P6OUT |= BIT3	    //打开传感板上电机 //传感器接口电机上电
#define Motor_Power_OFF	      P6OUT &=~BIT3 	//关闭传感板上电机 //传感器接口电机掉电

#define AIR202_Power_ON		  P2OUT |= BIT4	    //打开传感板上电机 //传感器接口电机上电
#define AIR202_Power_OFF	  P2OUT &=~BIT4 	//关闭传感板上电机 //传感器接口电机掉电


void g_DeviceIO_Init(void);



#endif
