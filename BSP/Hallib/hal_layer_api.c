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
* Filename      : hal_layer_api.c
* Version       : V1.00
* Programmer(s) : GLZ
*********************************************************************************************************
*/
#include  <stdbool.h>
#include  <hal_layer_api.h>

/*******************************************************************************
*	        Variables Definitions										                                  											  *
*******************************************************************************/
const uint8_t CrcHighBlock[256] = {
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
};

const uint8_t CrcLowBlock[256] = {
0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04,
0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8,
0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3, 0x11, 0xD1, 0xD0, 0x10,
0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,
0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C,
0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26, 0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0,
0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C,
0xB4, 0x74, 0x75, 0xB5, 0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54,
0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98,
0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};

typedef struct {
    char systemLowpower;
}gHal_Device_Manager;

static gHal_Device_Manager gManager;
static Mutex_t gMutex = null;
static uint32_t mLastTimems = 0;
static struct hal_timeval mTimeVal;

/*
// C prototype : void Hex2Str(unsigned char *pbDest,uint32_t *pbSrc,unsigned char Len, unsigned char offset)
// parameter(s): [OUT] pbDest - 存放目标字符串
//	             [IN]  pbSrc  - 输入16进制数的起始地址
//	             [IN]  Len    - 16进制数的字节数
//	             [IN]  offset - 字节在存放目标字符串的偏移
// return value: None
// remarks : 将16进制数转化为字符串
*/
// void Hex2Str(unsigned char *pbDest,uint8_t *pbSrc,unsigned char Len, unsigned char offset)
void Hex2Str(unsigned char *pbDest,uint32_t *pbSrc,unsigned char Len, unsigned char offset)
{
	unsigned char i;
	for(i = 0; i<Len;i++)
	{
		pbDest[2*i + offset] = (uint8_t)pbSrc[i]>>4;
		pbDest[2*i+1 + offset] = (uint8_t)pbSrc[i]&0xf;
	}
	for(i = 0+offset;i < Len*2+offset;i++)
	{
		if (pbDest[i] < 10)
		{
			pbDest[i] = pbDest[i] + '0';
		}
		else
		{
			pbDest[i] = pbDest[i] -10 +'A';
		}
	}
}


uint8_t HexToBCD(uint8_t hex)
{
	uint8_t temp;
	temp = hex/10*16 + hex%10;
	return temp;
}


//Itoa函数
char* Itoa(int val,char* dst,int radix)
{
    char *_pdst = dst;   
    if (!val)//允许val等于0 
    {
        *_pdst = '0';
        *++_pdst = '\0';
        return dst;
    }           
    if(val <0)
    {
        *_pdst++ = '-';
        val = -val;
    }
    char *_first = _pdst;     
    char _cov;           
    unsigned int _rem;   
    while(val > 0)
    {
        _rem = (unsigned int)(val % radix);
        val /= radix;//每次计算一位 ，从低到高
        if  (_rem > 9)//16进制
            *_pdst++ = (char)(_rem - 10 + 'a'); 
        else
            *_pdst++ = (char)(_rem + '0');      
    }      
    *_pdst-- = '\0';
    do{ //由于数据是地位到高位储存的，需要转换位置
        _cov = *_pdst;
        *_pdst = *_first;
        *_first = _cov;
        _pdst--;
        _first++;        
    }while(_first < _pdst);  
    return dst;
}


/*******************************************************************************
* Function Name  : Crc16(uint8_t *bufferpoint,int16_t sum)
* Description    : X^16 + X^15 + X^2 +1
* Input para     : *bufferpoint,sum
* Output para    : None
*******************************************************************************/
uint16_t Crc16(uint8_t *bufferpoint,int16_t sum)
{
	uint8_t High = 0xFF;
	uint8_t Low = 0xFF;
	uint8_t index;
	uint16_t Result;

	while(sum--)
	{
		index = High ^ *bufferpoint++;
		High = Low ^ CrcHighBlock[index];
		Low = CrcLowBlock[index];
	}
    UshortToByte1(Result) = Low;
    UshortToByte0(Result) = High;

    return(Result);
}

char Hal_CheckString(char *dst ,char *src)
{
    if(strstr((const char *)dst,(const char *)src) != null)
        return 1;
	else
	    return 0;
}

void *Hal_Malloc(int size)
{
    return malloc(size);
}

void *Hal_Calloc(int count, int size)
{
    return calloc(count, size);
}

void Hal_Free(void *ptr)
{
    if (ptr != NULL)
        free(ptr);
    g_Printf_dbg("malloc free ok\r\n");    
}

int Hal_ThreadCreate(void (*func)(void *p_arg), void *funcname,OS_STK *TaskStk, int priority)
{
    CPU_INT08U err;
    err = OSTaskCreate(func,                            /* Create the func task                                */
                       DEF_NULL,
                       TaskStk,
                       priority);

#if OS_TASK_NAME_EN > 0
    OSTaskNameSet(priority, funcname, &err);
#endif
    
    if (err != OS_ERR_NONE){
        g_Printf_info("%s failed\n",__func__);
		return -1;
	}

    return err;
}

int Hal_ThreadDestory(int priority)
{
    CPU_INT08U err;
    OSTimeDly(1);
    if (OSTaskDelReq(priority) == OS_ERR_TASK_DEL_REQ) {
        //Release any owned resources;
        //De-allocate any dynamic memory;
        err = OSTaskDel(priority);
        if (err != OS_ERR_NONE){
            g_Printf_dbg("%s failed\n",__func__);
            return -1;
	    }

        return err;
    }

    return -1;
}

Queue_t Hal_QueueCreate(void **start,int size)
{
    Queue_t queue = OSQCreate(start,size);
    return queue;
}

void Hal_QueueDestory(Queue_t queue)
{
    uint8_t err;
    Queue_t queasw;
    queasw = OSQDel(queue,OS_DEL_ALWAYS,&err);
    if(queasw != (OS_EVENT *)0){
        g_Printf_dbg("%s failed\n",__func__);
    }
}

int Hal_QueueSend(Queue_t queue, struct hal_message* msg, int timeout)
{
    uint8_t err;
    void *pMsg = (void *)msg;
	if(pMsg == null){
        return -1;
    }
	err = OSQPost (queue,msg);
    if(err != OS_ERR_NONE){
        g_Printf_dbg("%s failed\n",__func__);
        return -1;
    }
    
	return err;
}

 int Hal_QueueRecv(Queue_t queue, struct hal_message* msg, int timeout)
 {
     uint8_t err;
     void *pMsg;
     pMsg = OSQPend(queue, timeout, &err);
     if (OS_ERR_NONE == err){
         memcpy(msg, pMsg, sizeof(struct hal_message));
         return err;
     }

     return -1;
 }

int Hal_QueueNum_Waitfor_Pend(Queue_t queue)
 {
     uint8_t err;
     int num = 0;
     OS_Q_DATA Q_Data;
     err = OSQQuery(queue, &Q_Data);
     if (OS_ERR_NONE == err){
         num = Q_Data.OSNMsgs;
         return num;
     }

     return num;
 }

Mutex_t Hal_MutexCreate(int priority)
{
    Mutex_t pmutex;
    uint8_t err;
    pmutex = OSMutexCreate (priority, &err);
    if(pmutex == null){
    	g_Printf_dbg("%s failed\n",__func__);
    }

    return pmutex;
}

void Hal_MutexDestory(Mutex_t mutex)
{
    Mutex_t pmutex;
    uint8_t err;
    pmutex = OSMutexDel (mutex,OS_DEL_ALWAYS,&err);
    if(pmutex != (OS_EVENT *)0){
    	g_Printf_dbg("%s failed\n",__func__);
    }
}

void Hal_MutexLock(Mutex_t mutex)
{
    uint8_t err;
    OSMutexPend(mutex,100,&err);
}

void Hal_MutexUnlock(Mutex_t mutex)
{
    uint8_t err;
    err = OSMutexPost(mutex);
}

void Hal_GetTimeOfDay(struct hal_timeval* tv)
{
    uint32_t timems = 0; //it will roll over every 49 days, 17 hours.
    uint32_t timediff = 0;

	Hal_MutexLock(gMutex);
    //Gets time in milliseconds since RTOS start
    timems = OSTimeGet();
    if (timems < mLastTimems) {
        int32_t maxTime = -1;

        timediff = maxTime - mLastTimems;
        timediff += timems;
    } else {
        timediff = timems - mLastTimems;
    }

    mLastTimems = timems;
	if (mTimeVal.tv_msec == 0 && mTimeVal.tv_sec == 0) {
		mTimeVal.tv_sec = timediff / 500;
		mTimeVal.tv_msec = timediff % 500;
	}else {
		mTimeVal.tv_msec += timediff;
		if (mTimeVal.tv_msec >= 500) { // 1 second
			mTimeVal.tv_sec += mTimeVal.tv_msec / 500;
            mTimeVal.tv_msec = mTimeVal.tv_msec % 500;
		}
	}

    tv->tv_sec = mTimeVal.tv_sec;
    tv->tv_msec = mTimeVal.tv_msec;

    Hal_MutexUnlock(gMutex);
}

int Hal_Platform_Init(void)
{
    ScadaData_base_Init();
    Terminal_Para_Init();

    gMutex = Hal_MutexCreate(LOWEST_TASK_PRIO);
    if (gMutex == null) {
        g_Printf_dbg("%s mutex create Failed\r\n",__func__);
        return -1;
    }

    mTimeVal.tv_sec = 0;
    mTimeVal.tv_msec = 0;
	mLastTimems = 0;
    return 0;
}

int Hal_getProductName(char *proName)
{
#if (PRODUCT_TYPE == Air_Station) 
	strncpy(proName, "AirData", PRODUCT_NAMES_LEN-1);
	return 0;
#elif (PRODUCT_TYPE == Voc_Station) 
	strncpy(proName, "VocData", PRODUCT_NAMES_LEN-1);
	return 0;
#elif (PRODUCT_TYPE == Dust_Station) 
	strncpy(proName, "DustData", PRODUCT_NAMES_LEN-1);
	return 0;
#elif (PRODUCT_TYPE == WRain_Station) 
	strncpy(proName, "WRainData", PRODUCT_NAMES_LEN-1);
	return 0;
#elif (PRODUCT_TYPE == Wether_Station) 
	strncpy(proName, "WetherData", PRODUCT_NAMES_LEN-1);
	return 0;
#elif (PRODUCT_TYPE == Soil_Station) 
	strncpy(proName, "SoilData", PRODUCT_NAMES_LEN-1);
	return 0;
#elif (PRODUCT_TYPE == Agriculture_Station) 
	strncpy(proName, "AgricultureData", PRODUCT_NAMES_LEN-1);
	return 0;	
#elif (PRODUCT_TYPE == Flowmeter_Station) 
	strncpy(proName, "FlowmeterData", PRODUCT_NAMES_LEN-1);
	return 0;
#elif (PRODUCT_TYPE == Seeper_Station) 
	strncpy(proName, "SeeperData", PRODUCT_NAMES_LEN-1);
	return 0;
#elif (PRODUCT_TYPE == Flow_Station) 
	strncpy(proName, "FlowData", PRODUCT_NAMES_LEN-1);
	return 0;
#elif (PRODUCT_TYPE == Rain_Station) 
	strncpy(proName, "RainData", PRODUCT_NAMES_LEN-1);
	return 0;
#elif (PRODUCT_TYPE == IntegratedPitWell_Station)
	strncpy(proName, "IntegratedPitWellData", PRODUCT_NAMES_LEN-1);
	return 0;
#elif (PRODUCT_TYPE == InputmodeWell_Station)
	strncpy(proName, "InputmodeWellData", PRODUCT_NAMES_LEN-1);
	return 0;	
#elif (PRODUCT_TYPE == NoxiousGas_Station) 
	strncpy(proName, "NoxiousGasData", PRODUCT_NAMES_LEN-1);
	return 0;	
#elif (PRODUCT_TYPE == WetherSoil_Station) 
	strncpy(proName, "WetherSoilData", PRODUCT_NAMES_LEN-1);
	return 0;
#elif (PRODUCT_TYPE == Planting_Station) 
	strncpy(proName, "PlantingData", PRODUCT_NAMES_LEN-1);
	return 0;
#elif (PRODUCT_TYPE == LevelFlowrate_Station) 
	strncpy(proName, "LevelFlowrateData", PRODUCT_NAMES_LEN-1);
	return 0;
#elif (PRODUCT_TYPE == TankMonitor_Station) 
	strncpy(proName, "TankMonitorData", PRODUCT_NAMES_LEN-1);
	return 0;
#elif (PRODUCT_TYPE == MagicSTICK_Station) 
	strncpy(proName, "MagicSTICKData", PRODUCT_NAMES_LEN-1);
	return 0;
#elif (PRODUCT_TYPE == AliveNest_Station) 
	strncpy(proName, "AliveNestData", PRODUCT_NAMES_LEN-1);
	return 0;
#elif (PRODUCT_TYPE == CLCupboard_Station) 
	strncpy(proName, "CLCupboardData", PRODUCT_NAMES_LEN-1);
	return 0;
#elif (PRODUCT_TYPE == Custom_Station) 
	strncpy(proName, "CustomData", PRODUCT_NAMES_LEN-1);
	return 0;
#endif
}

uint32_t Hal_getManufactureDate(void)
{
    uint32_t temp =0;
    temp = OSBsp.Device.InnerFlash.innerFLASHRead(0,infor_ChargeAddr);
	temp = temp*100;
	temp += OSBsp.Device.InnerFlash.innerFLASHRead(1,infor_ChargeAddr);
	temp = temp*100;
	temp += OSBsp.Device.InnerFlash.innerFLASHRead(2,infor_ChargeAddr);
    g_Printf_info("%s %d\r\n",__func__,temp);
    return temp;
}

uint32_t Hal_getSerialNumber(void)
{
    uint32_t temp = 0,temp1 = 0;
    temp = OSBsp.Device.InnerFlash.innerFLASHRead(3,infor_ChargeAddr);
    temp = temp<<16;
    temp1 = OSBsp.Device.InnerFlash.innerFLASHRead(4,infor_ChargeAddr);
	temp1 = temp1<<8;
	temp += temp1 + OSBsp.Device.InnerFlash.innerFLASHRead(5,infor_ChargeAddr);
    g_Printf_info("%s %d\r\n",__func__,temp);
    return temp;
}

uint32_t Hal_getDeviceID(void)
{
    uint32_t temp = 0,temp1 = 0;
    temp = OSBsp.Device.InnerFlash.innerFLASHRead(6,infor_ChargeAddr);
    temp = temp<<16;
    temp1 = OSBsp.Device.InnerFlash.innerFLASHRead(7,infor_ChargeAddr);
	temp1 = temp1<<8;
	temp += temp1 + OSBsp.Device.InnerFlash.innerFLASHRead(8,infor_ChargeAddr);
    g_Printf_info("%s %d\r\n",__func__,temp);
    return temp;
}

uint32_t Hal_getFirmwareVersion(void)
{
    uint32_t temp =0;
    temp = OSBsp.Device.InnerFlash.innerFLASHRead(14,infor_BootAddr);
    g_Printf_info("%s %d\r\n",__func__,temp);
    return temp;
}

uint32_t Hal_getTransmitPeriod(void)
{
    uint32_t temp =0;
    temp = OSBsp.Device.InnerFlash.innerFLASHRead(11,infor_ChargeAddr);
	temp = temp<<8;
	temp += OSBsp.Device.InnerFlash.innerFLASHRead(12,infor_ChargeAddr);
    if(temp>360 && temp<=5){
        temp = 15;           
    }     
    g_Printf_info("%s %d\r\n",__func__,temp);
    return temp;
}

uint32_t Hal_getSensorFlashStatus(void)
{
    uint8_t str[12];
    uint32_t temp =0;
    temp = OSBsp.Device.InnerFlash.innerFLASHRead(21,infor_ChargeAddr);
	temp = temp<<8;
	temp += OSBsp.Device.InnerFlash.innerFLASHRead(22,infor_ChargeAddr);
    if(AppDataPointer->TerminalInfoData.SensorFlashWriteStatusPrintf == SENSOR_STATUS_WRITEFLASH_PRINTF_ENABLE) {
        AppDataPointer->TerminalInfoData.SensorFlashWriteStatusPrintf = SENSOR_STATUS_WRITEFLASH_PRINTF_DISABLE;
        AppDataPointer->TerminalInfoData.SensorFlashStatus = temp; 
        // g_Printf_info("%s %x\n",__func__,temp);

        Itoa(temp,str,2); //2进制输出
        g_Printf_info("%s binary format: %0s\r\n",__func__,str);
    }
    return temp;
}

uint16_t Hal_getBackupIndex(void)
{
    uint32_t temp =0;
    temp = OSBsp.Device.InnerFlash.innerFLASHRead(18,infor_ChargeAddr);
	temp = temp<<8;
	temp += OSBsp.Device.InnerFlash.innerFLASHRead(19,infor_ChargeAddr);
    if(temp == 0xFFFF)
        temp = 0;
    g_Printf_info("%s %d\n",__func__,temp);
    return (uint16_t)temp;
}

uint16_t Hal_getStartFile(void)
{
    uint32_t temp =0;
    temp = OSBsp.Device.InnerFlash.innerFLASHRead(20,infor_ChargeAddr);
	temp = temp<<8;
	temp += OSBsp.Device.InnerFlash.innerFLASHRead(21,infor_ChargeAddr);
    if(temp == 0xFFFF)
        temp = 0;
    g_Printf_info("%s %d\n",__func__,temp);
    return (uint16_t)temp;
}

uint8_t Hal_getFullFlag(void)
{
    uint32_t temp =0;
    temp = OSBsp.Device.InnerFlash.innerFLASHRead(22,infor_ChargeAddr);
    if(temp == 0xFF)
        temp = 0;
    g_Printf_info("%s %d\n",__func__,temp);
    return (uint8_t)temp;
}

#ifdef AIR202
int Hal_getProductKey(char *produckey)
{
    uint32_t keyLen =0;
    keyLen = OSBsp.Device.InnerFlash.innerFLASHRead(64,infor_ChargeAddr);
	if(keyLen == 0xff){
        g_Printf_info(("please set aliIot ProductKey first\r\n"));
		return -1;
	}
    char midTem[PRODUCT_KEY_LEN];
    int i = 0;
	memset(produckey,0x0,sizeof(produckey));
	memset(midTem,0x0,PRODUCT_KEY_LEN);
	for(i=0;i<keyLen;i++)
		midTem[i] = OSBsp.Device.InnerFlash.innerFLASHRead(65+i,infor_ChargeAddr);
    
    strncpy(produckey, midTem, keyLen);
    
    return 0;
}

int Hal_getDeviceName(char *devName)
{
    uint32_t nameLen =0;
    nameLen = OSBsp.Device.InnerFlash.innerFLASHRead(80,infor_ChargeAddr);
	if(nameLen == 0xff){
        g_Printf_info("please set aliIot DeviceName first\r\n");
		return -1;
	}
    char midTem[DEVICE_NAME_LEN];
    int i = 0;
	memset(devName,0x0,sizeof(devName));
	memset(midTem,0x0,DEVICE_NAME_LEN);
	for(i=0;i<nameLen;i++)
		midTem[i] = OSBsp.Device.InnerFlash.innerFLASHRead(81+i,infor_ChargeAddr);
    
    strncpy(devName, midTem, nameLen);
    
    return 0;
}

int Hal_getDeviceSecret(char *devSecret)
{
    uint32_t SecretLen =0;
    SecretLen = OSBsp.Device.InnerFlash.innerFLASHRead(100,infor_ChargeAddr);
	if(SecretLen == 0xff){
        g_Printf_info("please set aliIot DeviceSecret first\r\n");
		return -1;
	}
    char midTem[DEVICE_SECRET_LEN];
    int i = 0;
	memset(devSecret,0x0,sizeof(devSecret));
	memset(midTem,0x0,DEVICE_SECRET_LEN);
	for(i=0;i<SecretLen;i++)
		midTem[i] = OSBsp.Device.InnerFlash.innerFLASHRead(101+i,infor_ChargeAddr);
    
    strncpy(devSecret, midTem, SecretLen);
    
    return 0;
}

#endif

void Hal_EnterLowPower_Mode(void)
{
    static int m = 0;
    g_Printf_info("Enter Low Power!\r\n");
    hal_Delay_ms(100);
#if (TRANSMIT_TYPE == GPRS_Mode)
    // OSBsp.Device.IOControl.PowerSet(Max485_Power_Off);
    OSBsp.Device.IOControl.PowerSet(BaseBoard_Power_Off);
    // OSBsp.Device.IOControl.PowerSet(Sensor_Power_Off);
    // OSBsp.Device.IOControl.PowerSet(SDCard_Power_Off);
    OSBsp.Device.IOControl.PowerSet(AIR202_Power_Off);
    OSBsp.Device.IOControl.PowerSet(LPModule_Power_Off);
    OSBsp.Device.IOControl.PowerSet(Motor_Power_Off);
#endif
#if (TRANSMIT_TYPE == NBIoT_BC95_Mode || TRANSMIT_TYPE == LoRa_F8L10D_Mode )
    // OSBsp.Device.IOControl.PowerSet(Max485_Power_Off);
    OSBsp.Device.IOControl.PowerSet(BaseBoard_Power_Off);
    // OSBsp.Device.IOControl.PowerSet(Sensor_Power_Off);
    // OSBsp.Device.IOControl.PowerSet(SDCard_Power_Off);
    OSBsp.Device.IOControl.PowerSet(AIR202_Power_Off);
    // OSBsp.Device.IOControl.PowerSet(LPModule_Power_Off);
    OSBsp.Device.IOControl.PowerSet(Motor_Power_Off);	
#endif
   
    AppDataPointer->TransMethodData.GPRSNet = 0;        //ML 20190829
    AppDataPointer->TransMethodData.GPRSAttached = 0;

    gManager.systemLowpower = 1;
    LED_OFF;
    WDTCTL = WDTPW + WDTHOLD;             //CloseWatchDog
    SFRIE1 &= 0;
    for(m=0;m<1000;m++);
    __bis_SR_register(LPM0_bits + GIE);   //进入低功耗
}

void Hal_ExitLowPower_Mode(void)
{
    hal_Delay_ms(100);
    g_Printf_info("Exit Low Power!\r\n");
    gManager.systemLowpower = 0;
    // OSBsp.Device.IOControl.PowerSet(BaseBoard_Power_On);
    // OSBsp.Device.IOControl.PowerSet(Sensor_Power_On);
    // OSBsp.Device.IOControl.PowerSet(Max485_Power_On);
    AppDataPointer->TerminalInfoData.DeviceStatus = DEVICE_STATUS_POWER_OFF;
#if (TRANSMIT_TYPE == GPRS_Mode)
    AppDataPointer->TransMethodData.GPRSStatus = GPRS_Power_off;
#endif
#if (TRANSMIT_TYPE == NBIoT_BC95_Mode)
    AppDataPointer->TransMethodData.NBStatus = NB_Init_Done;
#endif
#if (TRANSMIT_TYPE == LoRa_F8L10D_Mode)
    if(AppDataPointer->TransMethodData.LoRaNet)
        AppDataPointer->TransMethodData.LoRaStatus = LoRa_Join_Over;
    else    //进低功耗前入网失败，出低功耗后继续入网
    {
        AppDataPointer->TransMethodData.LoRaStatus = LoRa_Power_on;
    }
#endif
}

char Hal_getCurrent_work_Mode(void)
{
    return gManager.systemLowpower;
}
