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
* Filename      : bsp.c
* Version       : V1.00
* Programmer(s) : GLZ
*********************************************************************************************************
*/

#include  <bsp.h>
#include  <msp430.h>


/*
*********************************************************************************************************
*                                               CONSTANTS
*********************************************************************************************************
*/

#define  BSP_DLY_CONST      (BSP_CPU_CLK_FREQ / 1000000u)
#define  BSP_DLY_MAX        (0xFFFFFFFF / BSP_DLY_CONST)


/*
*********************************************************************************************************
*                                               DATA TYPES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            GLOBAL VARIABLES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               PROTOTYPES
*********************************************************************************************************
*/

static  void  BSP_OSTickInit(void);
static void BSP_OSClockInit(void);
static void BSP_OSCloseWatchDog(void);
/*
******************************************************************************************************************************
******************************************************************************************************************************
**                                         Global Functions
******************************************************************************************************************************
******************************************************************************************************************************
*/

/*
*********************************************************************************************************
*                                                BSP_Init()
*
* Description : This function should be called by your application code before you make use of any of the
*               functions found in this module.
*
* Arguments   : None
*
* Returns     : None
*********************************************************************************************************
*/

void  BSP_Init(void)
{
    __disable_interrupt();                                      /* Disable all int. until we are ready to accept them   */
    
    BSP_OSCloseWatchDog();
    BSP_OSClockInit();
    BSP_OSTickInit();                                           /* Initialize the OS tick timer   */

    g_Device_InnerRTC_Init();   
    g_DeviceIO_Init();
    g_Device_Usart1_Init(9600);
    g_Device_Usart2_Init(115200);     
    g_Device_Usart3_Init(9600);      
    g_Device_ADC_Init(); 
    g_Device_SD_Init();
    // g_Device_SPI2_Init();
    g_Device_SPI3_Init();
    g_Device_InnerFlash_Init();
    hal_Delay_ms(100);
    g_Printf_info("BSP init over\r\n");
    g_Device_ExtRTC_Init();
    hal_Delay_ms(100);
    g_Printf_info("RTC init over\r\n");

#ifdef SDCard
    g_Device_SDCard_Check();               
    hal_Delay_ms(100);
    g_Printf_info("SD init over\r\n");
#endif

    Recive_485_Enable;
    ScadaData_base_Init();
    hal_Delay_ms(100);
}


/*
*********************************************************************************************************
*                                            BSP_OSTickInit()
*
* Description   : This function initializes the uC/OS-II's tick source.
*
* Argument    	: None
*
* Returns       : None
*********************************************************************************************************
*/


static void  BSP_OSTickInit(void)
{
    WDTCTL  = WDT_MDLY_32;                                      /* This configuration produces interrupts every 32  ... */
                                                                /* ... ms for SMCLK = 1 MHz.                            */
                                                                /* BE SURE TO SET TICK RATE IN OS_CFG.H ...             */
                                                                /* ... TO CORRESPOND WITH THIS FREQUENCY!               */

    SFRIE1 |= 1;                                                /* Enable WDT interrupts.                               */
}

static void BSP_OSClockInit(void){
    // Use the REFO oscillator as the FLL reference, and also for ACLK
    UCSCTL3 = (UCSCTL3 & ~(SELREF_7)) | (SELREF__REFOCLK);   //璁剧疆FLL鍙傝�鏃堕挓涓篟EFOCLK(鍐呴儴32.768kHz鏅舵尟)锛涘綋鏈塜T2,閫夋嫨XT2鍚﹀垯閫塕EFOCLK
    UCSCTL4 = (UCSCTL4 & ~(SELA_7)) | (SELA__REFOCLK);       //ACLK鍙傝�鏃堕挓婧愰�鎷╂帶鍒讹紱褰撴湁XT2,閫夋嫨XT2鍚﹀垯閫塂COCLKDIV
    // Start the FLL, which will drive MCLK (not the crystal)
    Init_FLL(BSP_CPU_CLK_FREQ/1000, BSP_CPU_CLK_FREQ/32768);
}

static void BSP_OSCloseWatchDog(void)
{
	WDTCTL = WDTPW + WDTHOLD;       //CloseWatchDog
}





static void Init(void)
{
    BSP_Init();
}

SystemStruct OSBsp =
{
    Init,
};

