/* 
 * File:   I2C.h
 * Author: Alex
 *
 * Created on February 17, 2019, 6:42 PM
 */

#ifndef I2C_H
#define	I2C_H

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
void I2C_vInit(void);

void I2C_vJoinAsSlave(uint8_t adresssAsSlave);

void I2C_vMasterTransmit(uint8_t targetAdress, uint8_t targetRegister, uint8_t dataToBeSent);

void I2C_vMasterTransmitBytes(uint8_t targetAdress, uint8_t * arrayWithData, uint8_t numberOfBytes);

void I2C_vMasterRead(uint8_t targetAdress, uint8_t numberOfBytes, uint8_t * storingLocation);
/**
 * \brief     This function is used to read received data by the slave from the RX buffer;
 * \param     None
 * \return    bool: true if new data was received - meaning that in *receivedData is a new value; 
 *                  false if no new data was put at receivedData; 
 */
bool I2C_vSlaveMainFunction(uint8_t * receivedData, uint16_t * matchedAdress);

bool I2C_bStopDetected(void);

bool I2C_bOperationWasARead(void);

#endif	/* I2C_H */

