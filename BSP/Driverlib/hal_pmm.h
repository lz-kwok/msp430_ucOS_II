//****************************************************************************//
// Function Library for setting the PMM
//    File: hal_pmm.h
//
//    Texas Instruments
//
//    Version 1.2
//    10/17/09
//
//    V1.0  Initial Version
//    V1.1  Adjustment to UG
//    V1.2  Added return values
//****************************************************************************////====================================================================


#ifndef __PMM
#define __PMM

#include <stdint.h>
#include "hal_macros.h"

/*****************************************************************************
* MACROS 
*****************************************************************************/
#define ENABLE_SVSL()        st(PMMCTL0_H = 0xA5; SVSMLCTL |= SVSLE; PMMCTL0_H = 0x00;)
#define DISABLE_SVSL()       st(PMMCTL0_H = 0xA5; SVSMLCTL &= ~SVSLE; PMMCTL0_H = 0x00;)
#define ENABLE_SVML()        st(PMMCTL0_H = 0xA5; SVSMLCTL |= SVMLE; PMMCTL0_H = 0x00;)
#define DISABLE_SVML()       st(PMMCTL0_H = 0xA5; SVSMLCTL &= ~SVMLE; PMMCTL0_H = 0x00;)
#define ENABLE_SVSH()        st(PMMCTL0_H = 0xA5; SVSMHCTL |= SVSHE; PMMCTL0_H = 0x00;)
#define DISABLE_SVSH()       st(PMMCTL0_H = 0xA5; SVSMHCTL &= ~SVSHE; PMMCTL0_H = 0x00;)
#define ENABLE_SVMH()        st(PMMCTL0_H = 0xA5; SVSMHCTL |= SVMHE; PMMCTL0_H = 0x00;)
#define DISABLE_SVMH()       st(PMMCTL0_H = 0xA5; SVSMHCTL &= ~SVMHE; PMMCTL0_H = 0x00;)
#define ENABLE_SVSL_SVML()   st(PMMCTL0_H = 0xA5; SVSMLCTL |= (SVSLE + SVMLE); PMMCTL0_H = 0x00;)
#define DISABLE_SVSL_SVML()  st(PMMCTL0_H = 0xA5; SVSMLCTL &= ~(SVSLE + SVMLE); PMMCTL0_H = 0x00;)
#define ENABLE_SVSH_SVMH()   st(PMMCTL0_H = 0xA5; SVSMHCTL |= (SVSHE + SVMHE); PMMCTL0_H = 0x00;)
#define DISABLE_SVSH_SVMH()  st(PMMCTL0_H = 0xA5; SVSMHCTL &= ~(SVSHE + SVMHE); PMMCTL0_H = 0x00;)

#define ENABLE_SVSL_RESET()       st(PMMCTL0_H = 0xA5; PMMRIE |= SVSLPE; PMMCTL0_H = 0x00;)
#define DISABLE_SVSL_RESET()      st(PMMCTL0_H = 0xA5; PMMRIE &= ~SVSLPE; PMMCTL0_H = 0x00;)
#define ENABLE_SVML_INTERRUPT()   st(PMMCTL0_H = 0xA5; PMMRIE |= SVMLIE; PMMCTL0_H = 0x00;)
#define DISABLE_SVML_INTERRUPT()  st(PMMCTL0_H = 0xA5; PMMRIE &= ~SVMLIE; PMMCTL0_H = 0x00;)
#define ENABLE_SVSH_RESET()       st(PMMCTL0_H = 0xA5; PMMRIE |= SVSHPE; PMMCTL0_H = 0x00;)
#define DISABLE_SVSH_RESET()      st(PMMCTL0_H = 0xA5; PMMRIE &= ~SVSHPE; PMMCTL0_H = 0x00;)
#define ENABLE_SVMH_INTERRUPT()   st(PMMCTL0_H = 0xA5; PMMRIE |= SVMHIE; PMMCTL0_H = 0x00;)
#define DISABLE_SVMH_INTERRUPT()  st(PMMCTL0_H = 0xA5; PMMRIE &= ~SVMHIE; PMMCTL0_H = 0x00;)
#define CLEAR_PMM_IFGS()          st(PMMCTL0_H = 0xA5; PMMIFG = 0; PMMCTL0_H = 0x00;)
//************************************************************************
// Defines
//************************************************************************

#define PMM_STATUS_OK     0
#define PMM_STATUS_ERROR  1

//====================================================================
/**
  * Set the VCore to a new level if it is possible and return a
  * error - value.
  *
  * \param      level       PMM level ID
  * \return	int	    1: error / 0: done
  */
extern uint16_t SetVCore (uint8_t level);

//====================================================================
/**
  * Set the VCore to a higher level, if it is possible.
  * Return a 1 if voltage at highside (Vcc) is to low
  * for the selected Level (level).
  *
  * \param      level       PMM level ID
  * \return	int	    1: error / 0: done
  */
static uint16_t SetVCoreUp (uint8_t level);

//====================================================================
/**
  * Set the VCore to a lower level.
  * Return a 1 if voltage at highside (Vcc) is still to low
  * for the selected Level (level).
  *
  * \param      level       PMM level ID
  * \return	int	    1: done with error / 0: done without error
  */
static uint16_t SetVCoreDown (uint8_t level);

#endif /* __PMM */
