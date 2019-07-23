/*
*********************************************************************************************************
*                                              EXAMPLE CODE
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
*                                            EXAMPLE CODE
*
*                                      Texas Instruments MSP430
*                                               on the
*                                          MSP-EXP430F5259LP
*                                          Evaluation Board
*
* Filename      : app.c
* Version       : V1.00
* Programmer(s) : GLZ
*********************************************************************************************************
*/

#include  <ucos_ii.h>
#include  <cpu.h>
#include  <bsp.h>
#include  <lib_def.h>
#include  <app_cfg.h>
#include  <msp430.h>
#include  <hal_layer_api.h>


/*
*********************************************************************************************************
*                                                 DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                                VARIABLES
*********************************************************************************************************
*/

static OS_STK ScadaTaskStartStk[DEFAULT_TASK_STK_SIZE];


/*
*********************************************************************************************************
*                                            FUNCTION PROTOTYPES
*********************************************************************************************************
*/
static  void  ScadaTaskStart(void *p_arg);


/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary CPU and C initialization.
*
* Arguments   : none
*********************************************************************************************************
*/

void  main (void)
{
    OSInit();                                /* Initialize "uC/OS-II, The Real-Time Kernel"          */
    OSBsp.Init();                            /* Initialize BSP functions                             */
#if (OS_TASK_STAT_EN > 0)
    OSStatInit();                            /* Determine CPU capacity                               */
#endif

    Hal_ThreadCreate(ScadaTaskStart,
                    (void *)"ScadaTaskStart",
                    &ScadaTaskStartStk[DEFAULT_TASK_STK_SIZE-1u],
                    SCADA_TASK_TASK_PRIO);

    OSStart();                               /* Start multitasking (i.e. give control to uC/OS-II)   */
}


/*
*********************************************************************************************************
*                                               AppTaskStart()
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Arguments   : p_arg   is the argument passed to 'AppStartTask()' by 'OSTaskCreate()'.
*
* Note(s)     : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*               2) Interrupts are enabled once the task start because the I-bit of the CCR register was
*                  set to 0 by 'OSTaskCreate()'.
*********************************************************************************************************
*/

static  void  ScadaTaskStart (void *p_arg)
{
    (void)p_arg;                   

    while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.       */
        APP_TRACE_INFO(("Hello, world!\n"));
        OSTimeDlyHMSM(0u, 0u, 1u, 0u);
    }
}



