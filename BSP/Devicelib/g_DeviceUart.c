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
* Filename      : g_DeviceUart.c
* Version       : V1.00
* Programmer(s) : GLZ
*********************************************************************************************************
*/
#include <bsp.h>


const uint8_t USCIModulation[16] = {0x00,0x10,0x20,0x30,0x40,0x50,0x60,0x70,0x80,0x90,
		                          0xA0,0xB0,0xC0,0xD0,0xE0,0xF0};


//串口同步时间变量
uint8_t *Rcv_TimePoint;     //+++++++++++++//
uint8_t Rcv_TimeNum = 0;
uint8_t Rcv_TimeData[50];
uint8_t TimebuffNum = 0;
uint8_t TimeBuff_Hex[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //16进制的时间Buffer  2018年3月15号 20时50分00秒 星期4


uint8_t Do_Flag_Uart3=0;
uint8_t Uart_0_Flag=0;		//串口接受完，在接受中断会置0，接受完1s后定时内置1
uint8_t Uart_1_Flag=0;		//串口接受完，在接受中断会置0，接受完1s后定时内置1
uint8_t Uart_2_Flag=0;		//串口接受完，在接受中断会置0，接受完1s后定时内置1


uint8_t aRxBuff[aRxLength];		//UART0 receive data buff
uint8_t aRxNum=0;		        //UART0 receive data num

uint8_t bRxBuff[bRxLength];		//UART1 receive data buff
uint8_t bRxNum=0;		        //UART1 receive data num

uint8_t cRxBuff[cRxLength];		//UART2 receive data buff
uint8_t cRxNum=0;		        //UART2 receive data num

uint8_t dRxBuff[dRxLength];		//UART3 receive data buff
uint8_t dRxNum=0;		        //UART3 receive data num

/*******************************************************************************
* Function Name  : Clear_CMD_Buffer
* Description    :
* Input para     : *Cmd,Len
* Output para    : None
*******************************************************************************/
void Clear_CMD_Buffer(uint8_t *data,uint8_t Len)
{
	uint8_t m;
	for(m=0;m<Len;m++)
	{
		data[m] = 0x00;
	}
}
/*******************************************************************************
* Function Name  : g_Device_SendByte_Uart0
* Description    :
* Input para     : Chr
* Output para    : None
*******************************************************************************/
void g_Device_SendByte_Uart0(uint8_t Chr)
{
	UCA0TXBUF = Chr;
	while(!(UCA0IFG & UCTXIFG));
}
/*******************************************************************************
* Function Name  : g_Device_SendNByte_Uart0
* Description    :
* Input para     : data,len
* Output para    : None
*******************************************************************************/
void g_Device_SendNByte_Uart0(uint8_t *data,uint8_t len)
{
	static uint8_t m = 0;
	for(m=0;m<len;m++)
	{
		UCA0TXBUF = data[m];
		while(!(UCA0IFG & UCTXIFG));
	}
}
/*******************************************************************************
* Function Name  : SendStringToUart0
* Description    :
* Input para     : *s
* Output para    : None
*******************************************************************************/
void g_Device_SendString_Uart0(char *s)
{
	while(*s != '\0')
	{
		g_Device_SendByte_Uart0((uint8_t)*s++);
	}
}
/*******************************************************************************
* Function Name  : g_Device_Usart0_Init		
* Description    : BaudRate9600bps,Used for LoRa
* Input para     : BaudRate
* Output para    : None
*******************************************************************************/
void g_Device_Usart0_Init(uint32_t BaudRate)
{
	float MidValue;
	uint32_t Mainclk;
	uint32_t tem;
	uint8_t UCBRF;

	Mainclk = MCLK_FREQ;

	P3SEL |= BIT3+BIT4;                       // P3.3,4 = USCI_A0 TXD/RXD
    UCA0CTL1 |= UCSWRST;                      // 复位设备状态
    UCA0CTL1 |= UCSSEL_2;                     // 选择参考时钟为SCMLK=16MHz

    MidValue = (float)Mainclk/16/BaudRate;
    tem = Mainclk/16/BaudRate;

    UCBRF = (MidValue- tem)*16 + 1;
    UCBRF = USCIModulation[UCBRF];

    UCA0BR0 = tem;
    UCA0BR1 = 0;                              //
	UCA0MCTL |= UCBRS_0 + UCBRF + UCOS16;     // Modulation UCBRSx=1, UCBRFx=3

    UCA0CTL1 &= ~UCSWRST;                     // 完成USCI初始化配置
    UCA0IE |= UCRXIE;                         // 使能接收中断

    OSBsp.Device.Usart0.WriteData   = g_Device_SendByte_Uart0;
	OSBsp.Device.Usart0.WriteNData  = g_Device_SendNByte_Uart0;
	OSBsp.Device.Usart0.WriteString = g_Device_SendString_Uart0;

    am_util_stdio_printf_init((am_util_stdio_print_char_t)g_Device_SendString_Uart0); //串口打印映射到串口0
}
/*******************************************************************************
* Function Name  : g_Device_SendByte_Uart1
* Description    :
* Input para     : Chr
* Output para    : None
*******************************************************************************/
void g_Device_SendByte_Uart1(uint8_t Chr)
{
	UCA1TXBUF = Chr;
	while(!(UCA1IFG & UCTXIFG));
}
/*******************************************************************************
* Function Name  : g_Device_SendNByte_Uart1
* Description    :
* Input para     : data,len
* Output para    : None
*******************************************************************************/
void g_Device_SendNByte_Uart1(uint8_t *data,uint8_t len)
{
	static uint8_t m = 0;
	for(m=0;m<len;m++)
	{
		UCA1TXBUF = data[m];
		while(!(UCA1IFG & UCTXIFG));
	}
}
/*******************************************************************************
* Function Name  : g_Device_SendString_Uart1
* Description    :
* Input para     : *s
* Output para    : None
*******************************************************************************/
void g_Device_SendString_Uart1(char *s)
{
	while(*s != '\0')
	{
		SendByteToUart1((uint8_t)*s++);
	}
}
/*******************************************************************************
* Function Name  : g_Device_Usart1_Init		
* Description    : BaudRate9600bps Used for GPS定位
* Input para     : None
* Output para    : None
*******************************************************************************/
void g_Device_Usart1_Init(uint32_t BaudRate)
{
	float MidValue;
	uint32_t Mainclk;
	uint32_t tem;
	uint8_t UCBRF;

	Mainclk = BSP_CPU_CLK_FREQ;

	P4SEL |= BIT4+BIT5;                       // P4.4,5 = USCI_A1 TXD/RXD
    UCA1CTL1 |= UCSWRST;                      // 复位设备状态
    UCA1CTL1 |= UCSSEL_2;                     // 选择参考时钟为SCMLK=16MHz

    MidValue = (float)Mainclk/16/BaudRate;
    tem = Mainclk/16/BaudRate;

    UCBRF = (MidValue- tem)*16 + 1;
    UCBRF = USCIModulation[UCBRF];

    UCA1BR0 = tem;
    UCA1BR1 = 0;                              //
	UCA1MCTL |= UCBRS_0 + UCBRF + UCOS16;     // Modulation UCBRSx=1, UCBRFx=3

    UCA1CTL1 &= ~UCSWRST;                     // 完成USCI初始化配置
    UCA1IE |= UCRXIE;                         // 使能接收中断

    System.Device.Usart1.WriteData   = g_Device_SendByte_Uart1;
	System.Device.Usart1.WriteNData  = g_Device_SendNByte_Uart1;
	System.Device.Usart1.WriteString = g_Device_SendString_Uart1;
}
/*******************************************************************************
* Function Name  : g_Device_SendByte_Uart2
* Description    :
* Input para     : Chr
* Output para    : None
*******************************************************************************/
void g_Device_SendByte_Uart2(uint8_t Chr)
{
	UCA2TXBUF = Chr;
	while(!(UCA2IFG & UCTXIFG));
}
/*******************************************************************************
* Function Name  : g_Device_SendNByte_Uart2
* Description    :
* Input para     : data,len
* Output para    : None
*******************************************************************************/
void g_Device_SendNByte_Uart2(uint8_t *data,uint8_t len)
{
	static uint8_t m = 0;
	for(m=0;m<len;m++)
	{
		UCA2TXBUF = data[m];
		while(!(UCA2IFG & UCTXIFG));
	}
}
/*******************************************************************************
* Function Name  : g_Device_SendString_Uart2
* Description    :
* Input para     : *s
* Output para    : None
*******************************************************************************/
void g_Device_SendString_Uart2(char *s)
{
	while(*s != '\0')
	{
		g_Device_SendByte_Uart2((uint8_t)*s++);
	}
}
/*******************************************************************************
* Function Name  : g_Device_Usart2_Init		
* Description    : BaudRate115200bps Used for Debug
* Input para     : BaudRate
* Output para    : None
*******************************************************************************/
void g_Device_Usart2_Init(uint32_t BaudRate)
{
	float MidValue;
	uint32_t Mainclk;
	uint32_t tem;
	uint8_t UCBRF;

	Mainclk = BSP_CPU_CLK_FREQ;

	P7SEL |= BIT0+BIT1;                       // P7.0,1 = USCI_A1 TXD/RXD
    UCA2CTL1 |= UCSWRST;                      // 复位设备状态
    UCA2CTL1 |= UCSSEL_2;                     // 选择参考时钟为SCMLK=16MHz

    MidValue = (float)Mainclk/16/BaudRate;
    tem = Mainclk/16/BaudRate;

    UCBRF = (MidValue- tem)*16 + 1;
    UCBRF = USCIModulation[UCBRF];

    UCA2BR0 = tem;
    UCA2BR1 = 0;                              //
	UCA2MCTL |= UCBRS_0 + UCBRF + UCOS16;   // Modulation UCBRSx=1, UCBRFx=3

    UCA2CTL1 &= ~UCSWRST;                     // 完成USCI初始化配置
    UCA2IE |= UCRXIE;                         // 使能接收中断

    System.Device.Usart2.WriteData   = g_Device_SendByte_Uart2;
	System.Device.Usart2.WriteNData  = g_Device_SendNByte_Uart2;
	System.Device.Usart2.WriteString = g_Device_SendString_Uart2;
//    am_util_stdio_printf_init((am_util_stdio_print_char_t)SendStringToUart2); //串口打印映射到串口0
}
/*******************************************************************************
* Function Name  : g_Device_SendByte_Uart3
* Description    :
* Input para     : Chr
* Output para    : None
*******************************************************************************/
void g_Device_SendByte_Uart3(uint8_t Chr)
{
	UCA3TXBUF = Chr;
	while(!(UCA3IFG & UCTXIFG));
}
/*******************************************************************************
* Function Name  : g_Device_SendNByte_Uart3
* Description    :
* Input para     : data,len
* Output para    : None
*******************************************************************************/
void g_Device_SendNByte_Uart3(uint8_t *data,uint8_t len)
{
	static uint8_t m = 0;
	for(m=0;m<len;m++)
	{
		UCA3TXBUF = data[m];
		while(!(UCA3IFG & UCTXIFG));
	}
}
/*******************************************************************************
* Function Name  : g_Device_SendString_Uart3
* Description    :
* Input para     : *s
* Output para    : None
*******************************************************************************/
void g_Device_SendString_Uart3(char *s)
{
	while(*s != '\0')
	{
		g_Device_SendByte_Uart3(*s++);
	}
}
/*******************************************************************************
* Function Name  : g_Device_Usart3_Init			
* Description    : BaudRate9600bps Used for 485
* Input para     : BaudRate
* Output para    : None
*******************************************************************************/
void g_Device_Usart3_Init(uint32_t BaudRate)
{
	float MidValue;
	uint32_t Mainclk;
	uint32_t tem;
	uint8_t UCBRF;

	Mainclk = BSP_CPU_CLK_FREQ;

	P4SEL |= BIT6+BIT7;                       // P7.0,1 = USCI_A1 TXD/RXD
    UCA3CTL1 |= UCSWRST;                      // 复位设备状态
    UCA3CTL1 |= UCSSEL_2;                     // 选择参考时钟为SCMLK=16MHz

    MidValue = (float)Mainclk/16/BaudRate;
    tem = Mainclk/16/BaudRate;

    UCBRF = (MidValue- tem)*16 + 1;
    UCBRF = USCIModulation[UCBRF];

    UCA3BR0 = tem;
    UCA3BR1 = 0;                              //
	UCA3MCTL |= UCBRS_0 + UCBRF + UCOS16;     // Modulation UCBRSx=1, UCBRFx=3

    UCA3CTL1 &= ~UCSWRST;                     // 完成USCI初始化配置
    UCA3IE |= UCRXIE;                         // 使能接收中断

    System.Device.Usart3.WriteData   = g_Device_SendByte_Uart3;
	System.Device.Usart3.WriteNData   = g_Device_SendNByte_Uart3;
	System.Device.Usart3.WriteString = g_Device_SendString_Uart3;
//    am_util_stdio_printf_init((am_util_stdio_print_char_t)SendStringToLoRa); //串口打印映射到串口3
}




//------USCI_A0中断服务服务函数-------------------------------------------------+
#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
{
	switch(__even_in_range(UCA0IV,4))
	{
		case 0:break;                             // Vector 0 - no interrupt
		case 2:                                   // Vector 2 - RXIFG
		    __bic_SR_register_on_exit(LPM0_bits);	//退出低功耗
			while(!(UCA0IFG&UCTXIFG));            // USCI_A3 TX buffer ready?
			{
				Uart_0_Flag=1;		//串口接受完标志置1
				switch(CommunicationIndex)
				{
					case GPRS_Mode:
// 						OSBsp.Device.Usart2.WriteData(UCA0RXBUF);  //Debug口
// 						aRxBuff[aRxNum++] = UCA0RXBUF;
// 						if((aRxBuff[aRxNum-2] == 0x0D)&&(aRxBuff[aRxNum-1] == 0x0A))
// 						{
// 							aRxNum = 0;

// 							if(CheckString(aRxBuff,"CONNECT OK"))
// 							{
// 								System.Device.Usart2.WriteString("Connect OK\r\n");
// 								AppDataPointer->TransMethodData.GPRSConnect = 1;
// 							}
// 							else if(CheckString(aRxBuff,"SEND OK"))
// 							{
// //								App.Data.GprsSendStatus = 1;
// 								System.Device.Usart2.WriteString("Send OK\r\n");
// 							}
// 							else if(CheckString(aRxBuff,"CLOSED"))
// 							{
// 								System.Device.Usart2.WriteString("Web Closed\r\n");
// //								AllowSend=0;
// //								GPRSReset;
// //								Ctr_GPRS_OFF;  //关闭GPRS
// 							}
// 							else if(CheckString(aRxBuff,"+CSQ:"))
// 							{
// 								if (CheckString(aRxBuff,"0,0"))
// 								{
// 									System.Device.Usart2.WriteString("CSQ Faild!\r\n");
// 									AppDataPointer->TransMethodData.GPRSNet = 0;
// 								}else
// 								{
// 									System.Device.Usart2.WriteString("CSQ OK!\r\n");
// 									AppDataPointer->TransMethodData.GPRSNet = 1;
// 								}
// 							}
// 							else if(CheckString(aRxBuff,"+CGATT:"))
// 							{
// 								if (CheckString(aRxBuff,"0"))
// 								{
// 									System.Device.Usart2.WriteString("Attached Faild!\r\n");
// 									AppDataPointer->TransMethodData.GPRSAttached = 0;
// 								}else
// 								{
// 									System.Device.Usart2.WriteString("Attached OK!\r\n");
// 									AppDataPointer->TransMethodData.GPRSAttached = 1;
// 								}
// 							}
// 						}
						break;

					case NBIoT_BC95_Mode:
						// System.Device.Usart2.WriteData(UCA0RXBUF);  //Debug口
						// aRxBuff[aRxNum++] = UCA0RXBUF;

						// if(aRxNum>aRxLength)
						// 	aRxNum=0;
						 break;////20181006 ML增加

					case LoRa_F8L10D_Mode:
// 						System.Device.Usart2.WriteData(UCA0RXBUF);  //Debug口
// 						aRxBuff[aRxNum++] = UCA0RXBUF;

// 						if((aRxNum > 3)&&(aRxBuff[aRxNum-1] == '\n'))
// 						{
// 							if(LoRaNet)
// 							{
// //								if(CheckString(aRxBuff,"+TXDONE"))
// //								{
// //									System.Device.Usart2.WriteString("Send data to LoRa done...\r\n");
// //								}
// 								if(CheckString(aRxBuff,"+STI"))
// 								{
// 									Flag_AutomaticTime = 1;//自动校时标志
// 									Rcv_TimePoint = strstr(aRxBuff,"+STI");         //判断接收到的数据是否有效
// 									while(*(Rcv_TimePoint+6) != '\r')
// 									{
// 										Rcv_TimeData[Rcv_TimeNum] = *(Rcv_TimePoint+6);
// 										Rcv_TimeNum++;
// 										Rcv_TimePoint++;
// 									}
// 									for(TimebuffNum=1;TimebuffNum<7;TimebuffNum++)
// 									{
// 										TimeBuff_Hex[TimebuffNum] = (Rcv_TimeData[(TimebuffNum-1)*3]-0x30)*10
// 												                  + (Rcv_TimeData[(TimebuffNum-1)*3+1]-0x30)*1;
// 									}

// 									for(TimebuffNum=1;TimebuffNum<7;TimebuffNum++)
// 									{
// 										time_buf[TimebuffNum]= HexToBCD(TimeBuff_Hex[TimebuffNum]);    //存“年月日时分秒周”
// 									}
// 									DS1302_write_time();   //写入时间
// 									System.Device.Usart2.WriteString("LoRa Automatic Time OK\r\n");
// 								}
// 							}
// 							if(!LoRaNet)
// 							{
// 								if(CheckString(aRxBuff,"AT_BUSY_ERROR"))
// 								{
// 									System.Device.Usart2.WriteString("internal state is busy...\r\n");
// 								}
// 								else if(CheckString(aRxBuff,"+NJS:1") | CheckString(aRxBuff,"Joined"))
// 								{
// 									if(SelfTest)
// 									{
// 										SelfTest=0;
// //										AllowSend=1;
// 									}
// 									System.Device.Usart2.WriteString("LoRa is on Net...\r\n");
// 									LoRaNet = 1;		//实际入网后置位
// 									LoRaReJoin = 0;
// 								}
// 								else if(CheckString(aRxBuff,"+JON:") & CheckString(aRxBuff,"OK"))
// 								{
// 									if(SelfTest)
// 									{
// 										SelfTest=0;
// //										AllowSend=1;
// 									}
// 									System.Device.Usart2.WriteString("LoRa is on Net...\r\n");
// 									LoRaNet = 1;		//实际入网后置位
// 									LoRaReJoin = 0;
// 								}
// 								else if(CheckString(aRxBuff,"+NJS:0"))
// 								{
// 									LoRaNet = 0;
// 									LoRa_Err++;
// 									if(LoRa_Err>=3)
// 									{
// 										LoRa_Err=0;
// 										LoRaReJoin=1;
// 									}
// 									System.Device.Usart2.WriteString("LoRa status is unjoined...\r\n");
// 								}
// 								else if(CheckString(aRxBuff,"+JON:") & CheckString(aRxBuff,"Failed"))
// 								{
// 									//  LoRa_Err++;
// 									System.Device.Usart2.WriteString("Join NET Failed...\r\n");
// 								}
// 								else if(CheckString(aRxBuff,"ACK"))
// 								{
// 									System.Device.Usart2.WriteString("Get Server ACK...\r\n");
// 								}

// 							}
// 						}
// 						if(aRxNum>aRxLength)
// 							aRxNum=0;
						break;
					case LoRa_OM402_Mode:
// //						System.Device.Usart2.WriteData(UCA0RXBUF);  //Debug口
// 						aRxBuff[aRxNum++] = UCA0RXBUF;
// 						if( (aRxNum > 3)&&(aRxBuff[aRxNum-1] == 0x40) )
// 						{
// 							aRxNum = 0;
// 							if( (aRxBuff[2] == 0x63)&&(aRxBuff[3] == 0x01)&&(aRxBuff[4] == 0x40) )
// 							{
// 								System.Device.Usart2.WriteString("LoRa Set Status Success\r\n");
// 							}
// //							else if( (aRxBuff[2] == 0x66)&&(aRxBuff[3] == 0x01)&&(aRxBuff[4] == 0x00) )
// //							{
// //								System.Device.Usart2.WriteString("LoRa Join NET White Success\r\n");
// //							}
// 							else if( (aRxBuff[2] == 0x50)&&(aRxBuff[3] == 0x01)&&(aRxBuff[4] == 0x11) )
// 							{
// 								System.Device.Usart2.WriteString("LoRa Join NET Failed...\r\n");
// 								AppDataPointer->TransMethodData.LoRaJoinNET = 0;
// 							}
// 							else if( (aRxBuff[2] == 0x70)&&(aRxBuff[3] == 0x01)&&(aRxBuff[4] == 0x00) )
// 							{
// 								System.Device.Usart2.WriteString("LoRa Join NET OK\r\n");
// 								AppDataPointer->TransMethodData.LoRaJoinNET = 1;
// 							}
// 							else if( (aRxBuff[2] == 0x71)&&(aRxBuff[3] == 0x01)&&(aRxBuff[5] == 0x40) )
// 							{
// 								System.Device.Usart2.WriteString("LoRa Send data OK\r\n");
// 							}
// 							else if( (aRxBuff[2] == 0x50)&&(aRxBuff[3] == 0x01)&&(aRxBuff[5] == 0xD0) )
// 							{
// 								System.Device.Usart2.WriteString("LoRa Send data Failed...\r\n");
// 							}
// 							else if( (aRxBuff[2] == 0x48)&&(aRxBuff[3] == 0x01)&&(aRxBuff[4] == 0x05) )
// 							{
// 								System.Device.Usart2.WriteString("OM402 Wrong Command!!!\r\n");
// 							}
// 						}
// 						if(aRxNum>aRxLength)
// 							aRxNum=0;
						break;////20181006 ML增加
					case GPRS_AIR202_Mode:
						// System.Device.Usart2.WriteData(UCA0RXBUF);  //Debug口
						// aRxBuff[aRxNum++] = UCA0RXBUF;
						// if((aRxBuff[aRxNum-2] == 0x0D)&&(aRxBuff[aRxNum-1] == 0x0A))
						// {
						// 	aRxNum = 0;
						// 	if(AppDataPointer->TransMethodData.GPRSStatus == GPRS_Waitfor_SMSReady){
						// 		if(CheckString(aRxBuff,"SMS Ready")){
						// 			AppDataPointer->TransMethodData.GPRSStatus = GPRS_Preinit;
						// 		}else if(CheckString(aRxBuff,"+CSQ:")){
						// 			if (CheckString(aRxBuff,"0,0")){
						// 				AppDataPointer->TransMethodData.GPRSNet = 0;
						// 			}else{
						// 				AppDataPointer->TransMethodData.GPRSNet = 1;
						// 				AppDataPointer->TransMethodData.GPRSStatus = GPRS_Preinit;
						// 			}
						// 		}
						// 	}else if(AppDataPointer->TransMethodData.GPRSStatus == GPRS_Preinit){
						// 		if(CheckString(aRxBuff,"+CGATT:"))
						// 		{
						// 			if (CheckString(aRxBuff,"0"))
						// 			{
						// 				System.Device.Usart2.WriteString("Attached Faild!\r\n");
						// 				AppDataPointer->TransMethodData.GPRSAttached = 0;
						// 			}else
						// 			{
						// 				System.Device.Usart2.WriteString("Attached OK!\r\n");
						// 				AppDataPointer->TransMethodData.GPRSAttached = 1;
						// 			}
						// 		}
						// 	}else if(AppDataPointer->TransMethodData.GPRSStatus == GPRS_Init_Done){
						// 		if((CheckString(aRxBuff,"OK"))||(CheckString(aRxBuff,"DOWNLOAD"))){
						// 			AppDataPointer->TransMethodData.GPRSATStatus = GPRS_Get_OK;
						// 		}else if(CheckString(aRxBuff,"+HTTPACTION: 1")){
						// 			AppDataPointer->TransMethodData.GPRSATStatus = GPRS_Waitfor_Token;
						// 			memset(aRxBuff,0x0,256);
						// 		}else if(CheckString(aRxBuff,"iotToken")){
						// 			memcpy(iotTokenBuf,aRxBuff,strlen(aRxBuff));
						// 			AppDataPointer->TransMethodData.GPRSATStatus = GPRS_Get_OK;
						// 		}
						// 	}
						// 	else if((AppDataPointer->TransMethodData.GPRSStatus == GPRS_Mqtt_Preinit)
						// 	||(AppDataPointer->TransMethodData.GPRSStatus == GPRS_Hqtt_Preinit)||(AppDataPointer->TransMethodData.GPRSStatus == GPRS_Hqtt_Init_Done)){
						// 		if(CheckString(aRxBuff,"CONNECT OK")){
						// 			AppDataPointer->TransMethodData.GPRSATStatus = GPRS_Connect_OK;
						// 		}else if(CheckString(aRxBuff,"CONNACK OK")){
						// 			AppDataPointer->TransMethodData.GPRSATStatus = GPRS_Connack_OK;
						// 		}else if(CheckString(aRxBuff,"OK")){
						// 			AppDataPointer->TransMethodData.GPRSATStatus = GPRS_Get_OK;
						// 		}else if(CheckString(aRxBuff,"+HTTPREAD:")){
						// 			memset(aRxBuff,0x0,256);
						// 		}else if(CheckString(aRxBuff,"+SAPBR:")){
						// 			AppDataPointer->TransMethodData.GPRSATStatus = GPRS_Waitfor_OK;
						// 			AppDataPointer->TransMethodData.Http_Cid = aRxBuff[8] - 0x30;
						// 		}else if(CheckString(aRxBuff,"DOWNLOAD")){
						// 			AppDataPointer->TransMethodData.GPRSATStatus = GPRS_Waitfor_Download;
						// 		}else if(CheckString(aRxBuff,"+HTTPACTION: 1")){
						// 			AppDataPointer->TransMethodData.GPRSATStatus = GPRS_Get_HTTPACT;
						// 		}
						// 	}
						// 	else if(AppDataPointer->TransMethodData.GPRSStatus == GPRS_Mqtt_Init_Done){
						// 		if(CheckString(aRxBuff,"SUBACK")){
						// 			AppDataPointer->TransMethodData.GPRSATStatus = GPRS_Waitfor_Suback;
						// 		}else if(CheckString(aRxBuff,"OK")){
						// 			AppDataPointer->TransMethodData.GPRSATStatus = GPRS_Get_OK;
						// 		}
						// 	}else if(AppDataPointer->TransMethodData.GPRSStatus == GPRS_MQTT_Alive){
						// 		if(CheckString(aRxBuff,"PUBACK")){
						// 			AppDataPointer->TransMethodData.GPRSATStatus = GPRS_MQTT_Enable_Pulish;
						// 		}else if(CheckString(aRxBuff,"+MSUB: ")){
						// 			memset(mqttbuf,0x0,512);
						// 			memcpy(mqttbuf,aRxBuff,strlen(aRxBuff));
						// 		}
						// 	}

						// 	memset(aRxBuff,0x0,256);
						// }
						break;
				} //switch
			}//while
			break;
		case 4:break;                             // Vector 4 - TXIFG
		default: break;
	}
}


//------USCI_A1中断服务服务函数-------------------------------------------------+
#pragma vector=USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void)
{
	switch(__even_in_range(UCA1IV,4))
	{
		case 0:break;                             // Vector 0 - no interrupt
		case 2:                                   // Vector 2 - RXIFG
	        __bic_SR_register_on_exit(LPM0_bits);	//退出低功耗
			while(!(UCA1IFG&UCTXIFG));            // USCI_A1 TX buffer ready?
			{
			    switch(AccessoryIndex)   //配件类型
			    {
			    	case GPS_Mode:
						TA1R=0;
						GPSRxBuff = UCA1RXBUF;
						if(GPSRxBuff == '$')
						{
							bRxNum = 0;
						}
						bRxBuff[bRxNum++] = GPSRxBuff;
						if( (bRxBuff[0] == '$')&&(bRxBuff[3] == 'G')&&(bRxBuff[4] == 'L')&&(bRxBuff[5] == 'L') )
						{
							if(GPSRxBuff == '\n')
							{
								//保存数据
								for(GPSRxNum=0;GPSRxNum<bRxNum;GPSRxNum++)
								{
									GPSLngLat_data[GPSRxNum] = bRxBuff[GPSRxNum];
								}
								//
								bRxNum = 0;
								Uart_1_Flag=1;
							}
						}
						break;
			    	case RS485_Mode:
			    	case RS232_Mode:
						TA1R=0;
						Uart_1_Flag=1;		//串口接受完标志置0，接受完1s后定时内置1
						if(bRxNum<bRxLength)
						{
						  bRxBuff[bRxNum++] = UCA1RXBUF;
						}
						else
						{
						  bRxNum=0;
						}
						break;
					default:
						break;
			    }
			}
			break;
		case 4:break;                            // Vector 4 - TXIFG
		default: break;
	}
}
//------USCI_A2中断服务服务函数-------------------------------------------------+
#pragma vector=USCI_A2_VECTOR
__interrupt void USCI_A2_ISR(void)
{
  switch(__even_in_range(UCA2IV,4))
  {
  case 0:break;                             // Vector 0 - no interrupt
  case 2:                                   // Vector 2 - RXIFG
	  __bic_SR_register_on_exit(LPM0_bits);	//退出低功耗
      while(!(UCA2IFG&UCTXIFG));            // USCI_A1 TX buffer ready?
	  {
    	  TA1R=0;
    	  Uart_2_Flag=1;		//串口接受完标志置0，接受完1s后定时内置1
    	  if(cRxNum<cRxLength)
    	  {
    		  cRxBuff[cRxNum++] = UCA2RXBUF;
    	  }
    	  else
    	  {
    		  cRxNum=0;
    	  }
	  }
      break;
  case 4:break;                             // Vector 4 - TXIFG
  default: break;
  }
}
//------USCI_A3中断服务服务函数-------------------------------------------------+
#pragma vector=USCI_A3_VECTOR
__interrupt void USCI_A3_ISR(void)
{
	switch(__even_in_range(UCA3IV,4))
	{
		case 0:break;                             // Vector 0 - no interrupt
		case 2:                                   // Vector 2 - RXIFG
		    __bic_SR_register_on_exit(LPM0_bits);	//退出低功耗
			while(!(UCA3IFG&UCTXIFG));            // USCI_A3 TX buffer ready?
			{
				TA1R=0;
				Do_Flag_Uart3=1;
				if(dRxNum<dRxLength)
				{
				  dRxBuff[dRxNum++] = UCA3RXBUF;
				}
				else
				{
				  dRxNum=0;
				}
			}
			break;
		case 4:break;                             // Vector 4 - TXIFG
		default: break;
	}
}
