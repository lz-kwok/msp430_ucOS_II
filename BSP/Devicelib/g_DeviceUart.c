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

#define ComData_MiniSize			4           //"AT\r\n"

const uint8_t USCIModulation[16] = {0x00,0x10,0x20,0x30,0x40,0x50,0x60,0x70,0x80,0x90,
		                          0xA0,0xB0,0xC0,0xD0,0xE0,0xF0};


//涓插彛鍚屾鏃堕棿鍙橀噺
uint8_t *Rcv_TimePoint;     //+++++++++++++//
uint8_t Rcv_TimeNum = 0;
uint8_t Rcv_TimeData[50];
uint8_t TimebuffNum = 0;
uint8_t TimeBuff_Hex[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //16杩涘埗鐨勬椂闂碆uffer  2018骞�鏈�5鍙�20鏃�0鍒�0绉�鏄熸湡4


char aRxBuff[aRxLength];		//UART0 receive data buff
uint8_t aRxNum=0;		        //UART0 receive data num

g_Device_Config_CMD bRxBuff;
// g_Device_Config_CMD cRxBuff;
uint8_t cRxBuff[cRxLength];
uint8_t cRxNum = 0;
uint8_t dRxBuff[dRxLength];		//UART3 receive data buff
uint8_t dRxNum=0;		        //UART3 receive data num

static Mutex_t gUartMutex = null;

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
* Function Name  : Clear_Buffer
* Description    :
* Input para     : *Cmd,*Len
* Output para    : None
*******************************************************************************/
void Clear_Buffer(unsigned char *Cmd,unsigned char *Len)
{
	unsigned char m;
	for(m=0;m<*Len;m++)
	{
		Cmd[m] = 0x00;
	}
	*Len=0;
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

	Mainclk = BSP_CPU_CLK_FREQ;

	P3SEL |= BIT3+BIT4;                       // P3.3,4 = USCI_A0 TXD/RXD
    UCA0CTL1 |= UCSWRST;                      // 澶嶄綅璁惧鐘舵�
    UCA0CTL1 |= UCSSEL_2;                     // 閫夋嫨鍙傝�鏃堕挓涓篠CMLK=16MHz

    MidValue = (float)Mainclk/16/BaudRate;
    tem = Mainclk/16/BaudRate;

    UCBRF = (MidValue- tem)*16 + 1;
    UCBRF = USCIModulation[UCBRF];

    UCA0BR0 = tem;
    UCA0BR1 = 0;                              //
	UCA0MCTL |= UCBRS_0 + UCBRF + UCOS16;     // Modulation UCBRSx=1, UCBRFx=3

    UCA0CTL1 &= ~UCSWRST;                     // 瀹屾垚USCI鍒濆鍖栭厤缃�
    UCA0IE |= UCRXIE;                         // 浣胯兘鎺ユ敹涓柇

    OSBsp.Device.Usart0.WriteData   = g_Device_SendByte_Uart0;
	OSBsp.Device.Usart0.WriteNData  = g_Device_SendNByte_Uart0;
	OSBsp.Device.Usart0.WriteString = g_Device_SendString_Uart0;

    am_util_stdio_printf_init((am_util_stdio_print_char_t)g_Device_SendString_Uart0); //涓插彛鎵撳嵃鏄犲皠鍒颁覆鍙�
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
		g_Device_SendByte_Uart1((uint8_t)*s++);
	}
}
/*******************************************************************************
* Function Name  : g_Device_Usart1_Init		
* Description    : BaudRate9600bps Used for GPS瀹氫綅
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
    UCA1CTL1 |= UCSWRST;                      // 澶嶄綅璁惧鐘舵�
    UCA1CTL1 |= UCSSEL_2;                     // 閫夋嫨鍙傝�鏃堕挓涓篠CMLK=16MHz

    MidValue = (float)Mainclk/16/BaudRate;
    tem = Mainclk/16/BaudRate;

    UCBRF = (MidValue- tem)*16 + 1;
    UCBRF = USCIModulation[UCBRF];

    UCA1BR0 = tem;
    UCA1BR1 = 0;                              //
	UCA1MCTL |= UCBRS_0 + UCBRF + UCOS16;     // Modulation UCBRSx=1, UCBRFx=3

    UCA1CTL1 &= ~UCSWRST;                     // 瀹屾垚USCI鍒濆鍖栭厤缃�
    UCA1IE |= UCRXIE;                         // 浣胯兘鎺ユ敹涓柇

    OSBsp.Device.Usart1.WriteData   = g_Device_SendByte_Uart1;
	OSBsp.Device.Usart1.WriteNData  = g_Device_SendNByte_Uart1;
	OSBsp.Device.Usart1.WriteString = g_Device_SendString_Uart1;

	memset(&bRxBuff,0x0,sizeof(g_Device_Config_CMD));
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
	Hal_MutexLock(gUartMutex);
	static uint8_t m = 0;
	for(m=0;m<len;m++)
	{
		UCA2TXBUF = data[m];
		while(!(UCA2IFG & UCTXIFG));
	}
	Hal_MutexUnlock(gUartMutex);
}
/*******************************************************************************
* Function Name  : g_Device_SendString_Uart2
* Description    :
* Input para     : *s
* Output para    : None
*******************************************************************************/
void g_Device_SendString_Uart2(char *s)
{
	Hal_MutexLock(gUartMutex);
	while(*s != '\0')
	{
		g_Device_SendByte_Uart2((uint8_t)*s++);
	}
	Hal_MutexUnlock(gUartMutex);
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
    UCA2CTL1 |= UCSWRST;                      // 澶嶄綅璁惧鐘舵�
    UCA2CTL1 |= UCSSEL_2;                     // 閫夋嫨鍙傝�鏃堕挓涓篠CMLK=16MHz

    MidValue = (float)Mainclk/16/BaudRate;
    tem = Mainclk/16/BaudRate;

    UCBRF = (MidValue- tem)*16 + 1;
    UCBRF = USCIModulation[UCBRF];

    UCA2BR0 = tem;
    UCA2BR1 = 0;                              //
	UCA2MCTL |= UCBRS_0 + UCBRF + UCOS16;   // Modulation UCBRSx=1, UCBRFx=3

    UCA2CTL1 &= ~UCSWRST;                     // 瀹屾垚USCI鍒濆鍖栭厤缃�
    UCA2IE |= UCRXIE;                         // 浣胯兘鎺ユ敹涓柇

    OSBsp.Device.Usart2.WriteData   = g_Device_SendByte_Uart2;
	OSBsp.Device.Usart2.WriteNData  = g_Device_SendNByte_Uart2;
	OSBsp.Device.Usart2.WriteString = g_Device_SendString_Uart2;

	gUartMutex = Hal_MutexCreate(LOWEST_TASK_PRIO-1);
    if (gUartMutex == null) {
        g_Printf_dbg("%s gUartMutex create Failed\r\n",__func__);
    }
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
    UCA3CTL1 |= UCSWRST;                      // 澶嶄綅璁惧鐘舵�
    UCA3CTL1 |= UCSSEL_2;                     // 閫夋嫨鍙傝�鏃堕挓涓篠CMLK=16MHz

    MidValue = (float)Mainclk/16/BaudRate;
    tem = Mainclk/16/BaudRate;

    UCBRF = (MidValue- tem)*16 + 1;
    UCBRF = USCIModulation[UCBRF];

    UCA3BR0 = tem;
    UCA3BR1 = 0;                              //
	UCA3MCTL |= UCBRS_0 + UCBRF + UCOS16;     // Modulation UCBRSx=1, UCBRFx=3

    UCA3CTL1 &= ~UCSWRST;                     // 瀹屾垚USCI鍒濆鍖栭厤缃�
    UCA3IE |= UCRXIE;                         // 浣胯兘鎺ユ敹涓柇

    OSBsp.Device.Usart3.WriteData   = g_Device_SendByte_Uart3;
	OSBsp.Device.Usart3.WriteNData  = g_Device_SendNByte_Uart3;
	OSBsp.Device.Usart3.WriteString = g_Device_SendString_Uart3;
//    am_util_stdio_printf_init((am_util_stdio_print_char_t)SendStringToLoRa); //涓插彛鎵撳嵃鏄犲皠鍒颁覆鍙�
}




g_Device_Config_CMD g_Device_Usart_UserCmd_Copy(G_UART_PORT Port)
{
	static int m = 0;
	g_Device_Config_CMD dst;
	memset(&dst,0x0,sizeof(g_Device_Config_CMD));
	if(Port == Usart2){
		dst.cmdLenth = cRxNum;
		cRxNum = 0;
		g_Printf_info("%s len:%d data:",__func__,dst.cmdLenth);
		for(m=0;m<dst.cmdLenth;m++){
			dst.hexcmd[m] = cRxBuff[m];
			g_Printf_info("%02x ",dst.hexcmd[m]);
		}
		g_Printf_info("\r\n");
		
		memset(cRxBuff,0x0,cRxLength);
	}else if(Port == Usart1){
		dst = bRxBuff;
		memset(&bRxBuff,0x0,sizeof(g_Device_Config_CMD));
	}

	return dst;
}

void UartRecTaskStart(void *p_arg)
{
	(void)p_arg;    
	OSTimeDlyHMSM(0u, 0u, 0u, 200u);
	static int RecLen = 0;
	g_Printf_info("%s ... ...\r\n",__func__);     
    while (DEF_TRUE) {   
		if(cRxNum != 0){
			RecLen = cRxNum;
			OSTimeDly(25);
			if(cRxNum == RecLen){
				g_Device_Config_QueuePost(G_CLIENT_CMD,(void *)"ClientCMD");
				RecLen = 0;
			}
		}
	}
}


//------USCI_A0中断服务函数 无线通信模组接口------------------------------------------------+
#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
{
	switch(__even_in_range(UCA0IV,4))
	{
		case 0:break;                             // Vector 0 - no interrupt
		case 2:                                   // Vector 2 - RXIFG
		    __bic_SR_register_on_exit(LPM0_bits);	//
			while(!(UCA0IFG&UCTXIFG));            // USCI_A3 TX buffer ready?
			{
				OSBsp.Device.Usart2.WriteData(UCA0RXBUF);  //
				aRxBuff[aRxNum++] = UCA0RXBUF;
#if(TRANSMIT_TYPE == NBIoT_BC95_Mode)
				if(aRxNum >= aRxLength)
					aRxNum = 0;
#endif //(TRANSMIT_TYPE == NBIoT_BC95_Mode)
#if(TRANSMIT_TYPE == LoRa_F8L10D_Mode)
				if(aRxNum >= aRxLength)
					aRxNum = 0;
					if(AppDataPointer->TransMethodData.LoRaStatus>=LoRa_Init_Done)
					{
						if((aRxBuff[aRxNum-2] == 0x0D)&&(aRxBuff[aRxNum-1] == 0x0A)){
						aRxNum = 0;
						g_Device_check_Response(aRxBuff);
						memset(aRxBuff,0x0,256);
						}
					}	
#endif	//(TRANSMIT_TYPE == LoRa_F8L10D_Mode)
#if(TRANSMIT_TYPE == GPRS_Mode)
				if((aRxBuff[aRxNum-2] == 0x0D)&&(aRxBuff[aRxNum-1] == 0x0A)){
					aRxNum = 0;
					g_Device_check_Response(aRxBuff);
					memset(aRxBuff,0x0,256);
					}
#endif	//(TRANSMIT_TYPE == GPRS_Mode)
				
			}//while
			break;
		case 4:break;                             // Vector 4 - TXIFG
		default: break;
	}
}


//------USCI_A1中断服务函数 配件232/484/GPS模组接口-------------------------------------------------+
#pragma vector=USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void)
{
	switch(__even_in_range(UCA1IV,4))
	{
		case 0:break;                             // Vector 0 - no interrupt
		case 2:                                   // Vector 2 - RXIFG
	        __bic_SR_register_on_exit(LPM0_bits);	
			while(!(UCA1IFG&UCTXIFG));            // USCI_A1 TX buffer ready?
			{
#if (ACCESSORY_TYPR == GPS_Mode)
				if(UCA1RXBUF == '$'){
					memset(&bRxBuff,0x0,sizeof(g_Device_Config_CMD));
				}
				bRxBuff.strcmd[bRxBuff.cmdLenth++] = UCA1RXBUF;
				if( (bRxBuff.strcmd[0] == '$')&&(bRxBuff.strcmd[3] == 'G')&&
						(bRxBuff.strcmd[4] == 'L')&&(bRxBuff.strcmd[5] == 'L') )
				{
					if(UCA1RXBUF == '\n')
					{
						OSIntEnter();
						g_Device_Config_QueuePost(G_WIRELESS_UPLAOD,(void *)"GPS_Info");	
						OSIntExit();
					}
				}
#else 	
				if(bRxBuff.cmdLenth<bRxLength){
					bRxBuff.hexcmd[bRxBuff.cmdLenth++] = UCA1RXBUF;
				  	if(bRxBuff.cmdLenth == 1){
					  OSIntEnter();
					  g_Device_Config_QueuePost(G_WIRELESS_UPLAOD,(void *)"SerialBus");	
					  OSIntExit();
				 	}		
				}else{
				  	bRxBuff.cmdLenth = 0;
				}
#endif
			}
			break;
		case 4:break;                            // Vector 4 - TXIFG
		default: break;
	}
}
//------USCI_A2中断服务函数 Debug接口------------------------------------------------+
#pragma vector=USCI_A2_VECTOR
__interrupt void USCI_A2_ISR(void)
{
	switch(__even_in_range(UCA2IV,4))
	{
		case 0:break;                             // Vector 0 - no interrupt
		case 2:                                   // Vector 2 - RXIFG
			__bic_SR_register_on_exit(LPM0_bits);	
			while(!(UCA2IFG&UCTXIFG));            // USCI_A1 TX buffer ready?
			{
				if(cRxNum < cRxLength){
					cRxBuff[cRxNum++] = UCA2RXBUF;
					// if(cRxNum == ComData_MiniSize){
					// 	OSIntEnter();
					// 	g_Device_Config_QueuePost(G_CLIENT_CMD,(void *)"ClientCMD");
					// 	OSIntExit();	
					// }	
				}else{
					cRxNum = 0;
				}
			}
		break;
		case 4:break;                             // Vector 4 - TXIFG
		default: break;
	}
}
//------USCI_A3中断服务函数 传感器采集接口-----------------------------------------------+
#pragma vector=USCI_A3_VECTOR
__interrupt void USCI_A3_ISR(void)
{
	switch(__even_in_range(UCA3IV,4))
	{
		case 0:break;                             // Vector 0 - no interrupt
		case 2:                                   // Vector 2 - RXIFG
		    __bic_SR_register_on_exit(LPM0_bits);	//退出中断
			while(!(UCA3IFG&UCTXIFG));            // USCI_A3 TX buffer ready?
			{
				TA1R=0;
				if(dRxNum<dRxLength){
				  dRxBuff[dRxNum++] = UCA3RXBUF;
				}else{
				  dRxNum=0;
				}
			}
			break;
		case 4:break;                             // Vector 4 - TXIFG
		default: break;
	}
}
