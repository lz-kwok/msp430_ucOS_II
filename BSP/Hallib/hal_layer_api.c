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
#include  <ucos_ii.h>
#include  <cpu.h>
#include  <lib_def.h>
#include  <app_cfg.h>
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