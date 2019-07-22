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