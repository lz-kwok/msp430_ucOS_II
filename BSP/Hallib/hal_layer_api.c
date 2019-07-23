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
#include  <hal_layer_api.h>

typedef struct {
    char product_name[PRODUCT_NAMES_LEN]; 
}gHal_Device_Manager;


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
}

int Hal_ThreadCreate(void *func, void *funcname,void *TaskStk, int stack_size,int priority)
{
    CPU_INT08U err;
    err = OSTaskCreate(func,                            /* Create the func task                                */
                       DEF_NULL,
                       TaskStk[stack_size - 1u],
                       priority);

#if OS_TASK_NAME_EN > 0
    OSTaskNameSet(priority, funcname, &err);
#endif
    
    if (err != OS_ERR_NONE){
        APP_TRACE_INFO(("%s failed\n",__func__));
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
            APP_TRACE_INFO(("%s failed\n",__func__));
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
        TRACE_LEVEL_DBG(("%s failed\n",__func__));
    }
}

int Hal_QueueSend(Queue_t queue, struct hal_message* msg, int timeout)
{
    uint8_t err;
    void *pMsg = (void *)msg;
	if(pMsg == null){
        return -1;
    }
	err = OSQPost (queue,pMsg);
    if(err != OS_ERR_NONE){
        TRACE_LEVEL_DBG(("%s failed\n",__func__));
        return -1;
    }
    
	return err;
}

int Hal_QueueRecv(Queue_t queue, struct hal_message* msg, int timeout)
{
    uint8_t err;
    void* pMsg = OSQPend(queue, timeout, &err);
    if (OS_ERR_NONE == err){
        memcpy(msg, pMsg, sizeof(struct hal_message));
        return err;
    }

    return -1;
}

Mutex_t Hal_MutexCreate(int priority)
{
    Mutex_t pmutex;
    uint8_t err;
    pmutex = OSMutexCreate (priority, &err);
    if(pmutex == null){
        TRACE_LEVEL_DBG(("%s failed\n",__func__));
    }

    return pmutex;
}

void Hal_MutexDestory(Mutex_t mutex)
{
    Mutex_t pmutex;
    uint8_t err;
    pmutex = OSMutexDel (mutex,OS_DEL_ALWAYS,&err);
    if(pmutex != (OS_EVENT *)0){
        TRACE_LEVEL_DBG(("%s failed\n",__func__));
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
<<<<<<< HEAD
}
//add test 190723
//add 19-07-23 14:05
=======
}
>>>>>>> 7e8fee30c75149c0169735522aef5f5cf09b30e2
