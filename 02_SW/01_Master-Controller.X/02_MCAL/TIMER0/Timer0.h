/* 
 * File:   Timer0.h
 * Author: Alex
 *
 * Created on December 6, 2018, 5:17 PM
 */

#ifndef TIMER_H
#define	TIMER_H

/*----------------------------------------------------------------------------*/
/*                                  Includes                                  */
/*----------------------------------------------------------------------------*/
#include "../../03_Common/types.h"
/*----------------------------------------------------------------------------*/
/*                             Defines and macros                             */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*                                 Data types                                 */

/*----------------------------------------------------------------------------*/
typedef enum {
    TIMER_8BIT_MODE = 0u,
    TIMER_16BIT_MODE,
}Timer_OperatingMode;

typedef struct {
    Timer_OperatingMode opMode;
    uint16_t startValue;
    uint16_t endValue;
    uint8_t clockSourceValue;
    uint8_t postscalerValue; /* 1:postscalerValue; ex: 1:14 =>  postscalerValue = 14, not the value 0b1101*/
    uint8_t prescalerPower; /* if desired prescaler value is 16384 the value for prescalerPower is 0b1110 (14) */
} Timer_Configuration;
/*----------------------------------------------------------------------------*/
/*                 External declaration of global RAM-Variables               */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*                 External declaration of global ROM-Variables               */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*                  External declaration of global functions                  */
/*----------------------------------------------------------------------------*/
/**
 * \brief     This function initializes the module;
 * \param     None
 * \return    None 
 */
void Timer0_vInit(void);
/**
 * \brief     This function starts the module with a given configuration;
 * \param     config - The configuration of the timer;
 *            startValue - count start Value;
 *            endValue - count end Value (overflow value); if TIMER_16BIT_MODE == config.opMode this param is irrelevant;
 * \return    None 
 */
void Timer0_vStart(Timer_Configuration config);
/**
 * \brief     This function stops the module;
 * \param     None
 * \return    None 
 */
void Timer0_vStop();

/**
 * \brief     This function stops the module;
 * \param     None
 * \return    None 
 */
bool Timer0_bWasOverflow();
#endif	/* TIMER_H */

