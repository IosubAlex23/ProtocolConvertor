/*
 * HEAD.H
 *
 *  Created on: Mmm DD, YYYY
 *      Author: Iosub Mihai Alexandru
 */

#ifndef DATALOGGER_H_
#define DATALOGGER_H_

/**
 * \file       Head.h
 * \author     Iosub Mihai Alexandru
 * \brief      Short description for this header file
 */


/*----------------------------------------------------------------------------*/
/*                                  Includes                                  */
/*----------------------------------------------------------------------------*/

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
 * \brief     This function is used to send log of the requested data via SPI
 * \param     None
 * \return    None 
 */
void DataLogger_vSendDataRequested(uint8_t sourceAdrress, uint8_t sourceProtocol, uint8_t DestinationAdrress, uint8_t destinationProtocol);

/**
 * \brief     This function sends log when a destination was not found
 * \param     None
 * \return    None 
 */
void DataLogger_vSendDestinationNotFound(uint8_t sourceAdrress, uint8_t sourceProtocol, uint8_t * dataBytes, uint8_t noOfBytes);

/**
 * \brief     This function send data when a conversion happened
 * \param     None
 * \return    None 
 */
void DataLogger_vSendDataConverted(uint8_t sourceAdrress, uint8_t sourceProtocol, uint8_t * dataBytes, uint8_t noOfBytes);

#endif /* DATALOGGER_H_ */
