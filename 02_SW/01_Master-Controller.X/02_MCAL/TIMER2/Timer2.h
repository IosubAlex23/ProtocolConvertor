/* 
 * File:   Timer0.h
 * Author: Alex
 *
 * Created on December 6, 2018, 5:17 PM
 */

#ifndef TIMER2_H
#define	TIMER2_H

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
    TMR_100kHz = 0,
    TMR_200kHz,
    TMR_300kHz,
    TMR_400kHz,
    TMR_500kHz,
    TMR_600kHz,
    TMR_700kHz,
    TMR_800kHz,
    TMR_900kHz,
    TMR_1000kHz,
    TMR_1100kHz,
    TMR_1200kHz,
    TMR_1300kHz,
    TMR_1400kHz,
    TMR_1500kHz,
    TMR_1600kHz
} Timer2_OutputFrequency;
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
void Timer2_vInit(Timer2_OutputFrequency targetFrequency);
/**
 * \brief     This function starts the module with a given configuration;
 * \param     config - The configuration of the timer;
 *            startValue - count start Value;
 *            endValue - count end Value (overflow value); if TIMER_16BIT_MODE == config.opMode this param is irrelevant;
 * \return    None 
 */
void Timer2_vStart();
/**
 * \brief     This function stops the module;
 * \param     None
 * \return    None 
 */
void Timer2_vStop();

/**
 * \brief     This function stops the module;
 * \param     None
 * \return    None 
 */
bool Timer2_bWasOverflow();
#endif	/* TIMER2_H */

