/*******************************************************************************
*    File Name:  g_DeviceUart.h
*    Revision:  1.0
*    Description:  串口程序头文件
* *****************************************************************************/
#ifndef G_DEVICEUART_H
#define G_DEVICEUART_H

#include <bsp.h>


extern uint8_t Do_Flag_Uart3;
extern uint8_t Uart_0_Flag;
extern uint8_t Uart_1_Flag;
extern uint8_t Uart_2_Flag;


#define aRxLength 256		//UART0 buff length
#define bRxLength 200		//UART1 buff length
#define cRxLength 100		//UART2 buff length
#define dRxLength 50		//UART3 buff length

extern char aRxBuff[];
extern uint8_t aRxNum;
extern uint8_t dRxBuff[];
extern uint8_t dRxNum;


void Clear_CMD_Buffer(uint8_t *data,uint8_t Len);
void g_Device_Usart0_Init(uint32_t BaudRate);     //LoRa
void g_Device_Usart1_Init(uint32_t BaudRate);     //GPS
void g_Device_Usart2_Init(uint32_t BaudRate);     //Debug
void g_Device_Usart3_Init(uint32_t BaudRate);     //485
void g_Device_Usart_Rxbuff_Copy(g_Device_Config_CMD dst);





#endif
