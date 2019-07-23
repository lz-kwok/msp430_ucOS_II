#ifndef G_DEVICEGPRS_H
#define G_DEVICEGPRS_H

#include  <hal_layer_api.h>


extern uint8_t GprsInitStatus;
extern uint8_t WirelessStatus;



enum GprsStatus
{
	PowerOff     = 0,
	InitDone     = 1,
	SyncNetRtc   = 2,
	NormalPost   = 3,
	OnNet        = 4,
	DisConnect   = 5
};



void g_Device_GPRS_Init(void);
void g_Device_Establish_TCP_Connection(void);
void g_Device_http_post(DataStruct *DataPointer);



#endif
