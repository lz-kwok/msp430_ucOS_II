#ifndef __HAL_LAYER_API_H
#define __HAL_LAYER_API_H

#include  <bsp.h>

/*************************************************************************
* MACROS
**************************************************************************/
#define hal_Reboot()                  PMMCTL0 = PMMPW + PMMSWBOR + (PMMCTL0 & 0x0003)

#define null                          ((void *) 0)   //空指针
#define invalid                       0xFF

/*Bit Operation Function*/
#define hal_SetBit(data, offset)      data |= 1 << offset      //置位某位为1
#define hal_ResetBit(data, offset)    data &= ~(1 << offset)   //复位某位为0
#define hal_GetBit(data, offset)      ((data >> offset) &0x01) //获取某位

#define hal_Delay_us(x)                __delay_cycles((long)(BSP_CPU_CLK_FREQ*(long)x/1000000.0))
#define hal_Delay_ms(x)                __delay_cycles((long)(BSP_CPU_CLK_FREQ*(long)x/1000.0))
#define hal_Delay_sec(x)               __delay_cycles((long)(BSP_CPU_CLK_FREQ*(long)x))

#endif /* __hal_UCS */
