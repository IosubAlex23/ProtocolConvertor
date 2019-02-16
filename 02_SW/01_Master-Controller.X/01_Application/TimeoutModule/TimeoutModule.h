/* 
 * File:   TimeoutModule.h
 * Author: Alex
 *
 * Created on February 16, 2019, 2:41 PM
 */

#ifndef TIMEOUTMODULE_H
#define	TIMEOUTMODULE_H

/*----------------------------------------------------------------------------*/
/*                                  Includes                                  */
/*----------------------------------------------------------------------------*/
#include "../../03_Common/types.h"
#include "../../02_MCAL/TIMER0/Timer0.h"
/*----------------------------------------------------------------------------*/
/*                             Defines and macros                             */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*                                 Data types                                 */
/*----------------------------------------------------------------------------*/
typedef enum
{
    TIMEOUT_NOT_REACHED=0u,
    TIMEOUT_REACHED,
    TIMEOUT_IDLE,
    TIMEOUT_ERROR,
}TimeoutModule_State;

/* The unit for the timeout: microseconds, milliseconds, seconds */
typedef enum
{
    TIMEOUT_uS=0u, 
    TIMEOUT_mS,
    TIMEOUT_S,
}TimeoutModule_MeasurementUnit;

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
 * \brief     This function initializes the TimeoutModule;
 * \param     None
 * \return    None 
 */
void TimeoutModule_vInit();
/**
 * \brief     This function is used when a timeout is needed; Can set timeout from 0 to 65535 microseconds or milliseconds
 * \param     timeAmount = the maximum number of 'unit' of timeout
 * \return    TimeoutModule_State - it says if timeout was or not reached or if there was an error 
 */
TimeoutModule_State TimeoutModule_uiSetTimeout(TimeoutModule_MeasurementUnit unit, uint16_t timeAmount);

TimeoutModule_State TimeoutModule_uiCancelTimeout(void);
#endif	/* TIMEOUTMODULE_H */

