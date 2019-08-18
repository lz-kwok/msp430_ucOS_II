/*******************************************************************************
*    File Name:  g_DeviceUart.h
*    Revision:  1.0
*    Description:  串口程序头文件
* *****************************************************************************/
#ifndef G_DEVICEUART_H
#define G_DEVICEUART_H

#include <stdint.h>
#include <g_DeviceConfig.h>

extern uint8_t Do_Flag_Uart3;
extern uint8_t Uart_0_Flag;


#define aRxLength 255		//UART0 buff length,最大255，否则aRxNum溢出
#define bRxLength 200		//UART1 buff length
#define cRxLength 100		//UART2 buff length
#define dRxLength 50		//UART3 buff length

typedef enum {
	Usart0 = 0x1,		
	Usart1,
    Usart2,
    Usart3
}G_UART_PORT;

extern char aRxBuff[];
extern uint8_t aRxNum;
// extern g_Device_Config_CMD cRxBuff;
extern uint8_t dRxBuff[];
extern uint8_t dRxNum;


void Clear_CMD_Buffer(uint8_t *data,uint8_t Len);
void Clear_Buffer(unsigned char *Cmd,unsigned char *Len);
void g_Device_Usart0_Init(uint32_t BaudRate);     //LoRa
void g_Device_Usart1_Init(uint32_t BaudRate);     //GPS
void g_Device_Usart2_Init(uint32_t BaudRate);     //Debug
void g_Device_Usart3_Init(uint32_t BaudRate);     //485
g_Device_Config_CMD g_Device_Usart_UserCmd_Copy(G_UART_PORT Port);
void UartRecTaskStart(void *p_arg);






#endif
