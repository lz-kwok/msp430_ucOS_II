// (c)2009 by Texas Instruments Incorporated, All Rights Reserved.
/*----------------------------------------------------------------------------+
|                                                                             |
|                              Texas Instruments                              |
|                                                                             |
|                          MSP430 USB-Example (CDC/HID Driver)                |
|                                                                             |
+-----------------------------------------------------------------------------+
|  Source: device.h, File Version 1.00 2009/12/03                             |
|  Author: RSTO                                                               |
|                                                                             |
|  Description:                                                               |
|  This file is included in other source code files                           |
|  and this only one place to change the included device header               |
+----------------------------------------------------------------------------*/
 

#include <msp430.h>


#if defined (__MSP430F6638) || defined (__MSP430F6637) || defined (__MSP430F6636) || \
	defined (__MSP430F6635) || defined (__MSP430F6634) || defined (__MSP430F6633) || \
	defined (__MSP430F6632) || defined (__MSP430F6631) || defined (__MSP430F6630) || \
	defined (__MSP430F5638) || defined (__MSP430F5637) || defined (__MSP430F5636) || \
	defined (__MSP430F5635) || defined (__MSP430F5634) || defined (__MSP430F5633) || \
	defined (__MSP430F5632) || defined (__MSP430F5631) || defined (__MSP430F5630) 
	#define __MSP430F563x_F663x
#elif defined (__MSP430F5510) || defined (__MSP430F5509) || defined (__MSP430F5508) || \
	defined (__MSP430F5507) || defined (__MSP430F5506) || defined (__MSP430F5505) || \
	defined (__MSP430F5504) || defined (__MSP430F5503) || defined (__MSP430F5502) || \
	defined (__MSP430F5501) || defined (__MSP430F5500)
    #define __MSP430F550x
#elif defined (__MSP430F5529) || defined (__MSP430F5528) || defined (__MSP430F5527) || \
	defined (__MSP430F5526) || defined (__MSP430F5525) || defined (__MSP430F5524) || \
	defined (__MSP430F5522) || defined (__MSP430F5521) || defined (__MSP430F5519) || \
	defined (__MSP430F5517) || defined (__MSP430F5515) || defined (__MSP430F5514) || \
	defined (__MSP430F5513)
    #define __MSP430F552x
#elif defined (__MSP430F5259__)
    #define __MSP430F525x
#else
    #error Define a constant of format __MSP430Fxxxx__ within the projects preprocessor settings,
    according to the device being used.
#endif
/*------------------------ Nothing Below This Line --------------------------*/
