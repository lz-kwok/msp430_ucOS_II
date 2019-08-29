#ifndef G_DEVICENB_H
#define G_DEVICENB_H

#include <hal_layer_api.h>
#include <g_Platform.h>

#if (TRANSMIT_TYPE == NBIoT_BC95_Mode)

//断点续传使用
#define MaxLength	5
extern uint16_t BackupIndex;
extern uint16_t StartFile;
extern uint8_t FullFlag;
extern char RespFile[10];

enum NB_STATUS {
	NB_Power_off = 0,
	NB_Power_on = 1,
	NB_Boot,
	NB_Init_Done,
	NB_Get_IP,
	NB_Registered,
	NB_Send_Done,
	NB_Send_Error,
	NB_Get_Receive,
	NB_Check_Receive,
	NB_Idel
	
};

unsigned char NB_Config(unsigned char *c , unsigned char m, unsigned char t);
void g_Device_NB_Restart(void);
char  g_Device_NB_Init(void);
void g_Device_NB_GetIP(void);
void g_Device_NB_Send(uint32_t *data ,uint8_t length);
void g_Device_NBSignal(void);
void g_Device_NB_Receive(void);

void  TransmitTaskStart (void *p_arg);
#endif

#endif

