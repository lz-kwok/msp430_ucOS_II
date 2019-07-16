/*
*********************************************************************************************************
*                                         BOARD SUPPORT PACKAGE
*
*                            (c) Copyright 2014; Micrium, Inc.; Weston, FL
*
*               All rights reserved. Protected by international copyright laws.
*
*               BSP is provided in source form to registered licensees ONLY.  It is
*               illegal to distribute this source code to any third party unless you receive
*               written permission by an authorized Micrium representative.  Knowledge of
*               the source code may NOT be used to develop a similar product.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*
*               You can contact us at www.micrium.com.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                        BOARD SUPPORT PACKAGE
*
*                                 Texas Instruments MSP-EXP430F5529LP
*                                               LaunchPad
*
* Filename      : bsp.h
* Version       : V1.00
* Programmer(s) : HS
*********************************************************************************************************
*/

#ifndef  __BSP_H__
#define  __BSP_H__

#include <string.h>
#include <stdio.h>
#include <intrinsics.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <math.h>
#include <time.h>

#include <hal_device.h>
#include <hal_macros.h>
#include <hal_pmm.h>
#include <hal_tlv.h>
#include <hal_types.h>
#include <hal_ucs.h>

#include <g_DeviceRTC.h>
/*
*********************************************************************************************************
*                                               CONSTANTS
*********************************************************************************************************
*/

#define  BSP_CPU_CLK_FREQ       25000000u                       /* Frequency in Hz                                      */


/*
*********************************************************************************************************
*                                            GLOBAL VARIABLES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void BSP_Init(void);


#endif
