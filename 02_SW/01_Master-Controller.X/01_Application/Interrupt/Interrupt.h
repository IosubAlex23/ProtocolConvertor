/*
 * HEAD.H
 *
 *  Created on: Mmm DD, YYYY
 *      Author: Iosub Mihai Alexandru
 */

#ifndef INTERRUPT_H_
#define INTERRUPT_H_

/**
 * \file       Head.h
 * \author     Iosub Mihai Alexandru
 * \brief      Short description for this header file
 */


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
 * \brief     This function [...];
 * \param     None
 * \return    None 
 */
void INTERRUPT_Initialize(void);

void INTERRUPT_GlobalInterruptDisable(void);

void INTERRUPT_GlobalInterruptEnable(void);
#endif /* HEAD_H_ */
