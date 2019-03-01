/*
 * HEAD.H
 *
 *  Created on: Mmm DD, YYYY
 *      Author: Ioan Nicoara
 */

#ifndef HEAD_H_
#define HEAD_H_

/**
 * \file       Head.h
 * \author     Nicoara Ioan
 * \brief      Short description for this header file
 */


/*----------------------------------------------------------------------------*/
/*                                  Includes                                  */
/*----------------------------------------------------------------------------*/
#include <xc.h>
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
 * \brief     This function initialize the UART Module;
 * \param     None
 * \return    None 
 */
void UART1_vInit(void);

/**
 * \brief     This function is used for send values on UART Asynchronous Transmitter
 * \param     valSend - represents value wich to be send on RX
 * \return    None 
 */
void UART1_uiTransmitter(uint8_t valSend);

/**
 * \brief     This function is used for receive values on UART Asynchronous Receiver
 * \param     * var
 * \return    None 
 */
uint8_t UART1_uiReception(/*uint8_t * var*/);

/**
 * \brief     This function [...];
 * \param     None
 * \return    None 
 */
void UART_vOperationMode(uint8_t reg, uint8_t mode);

#endif /* HEAD_H_ */
