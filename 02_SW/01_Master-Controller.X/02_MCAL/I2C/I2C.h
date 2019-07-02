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
#include "../TIMER2/Timer2.h"
#include "../01_Application/DataLogger/DataLogger.h"

/*----------------------------------------------------------------------------*/
/*                             Defines and macros                             */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*                                 Data types                                 */

/*----------------------------------------------------------------------------*/
typedef enum {
    I2C_NO_NEW_DATA = 0u,
    I2C_NEW_DATA_RECEIVED,
    I2C_DATA_REQUESTED,
    I2C_REQUEST_SERVED
} I2C_SlaveOperationType;
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
 * \brief     This function initializes the I2C module
 * \param     None
 * \return    None 
 */
void I2C_vInit(void);
/**
 * \brief     This function puts the module into the slave mode
 * \param     adresssAsSlave - the address of the slave device
 * \return    None 
 */
void I2C_vJoinAsSlave(uint8_t adresssAsSlave);
/**
 * \brief     With this function a master can send two bytes of data.
 * \param     targetAdress - represents the target address
 *            targetRegister - represents the first byte of data
 *            dataToBeSent - represents second byte of data
 * \return    None 
 */
void I2C_vMasterTransmit(uint8_t targetAdress, uint8_t targetRegister, uint8_t dataToBeSent);
/**
 * \brief     With this function a master can send a number of bytes
 * \param     targetAdress - represents the target address
 *            *arrayWithData - it's a pointer to the array that contains the data that need to be sent
 *            numberOfBytes - represents the number of bytes
 * \return    None 
 */
void I2C_vMasterTransmitBytes(uint8_t targetAdress, uint8_t * arrayWithData, uint8_t numberOfBytes);
/**
 * \brief     With this function a master can read a number of bytes
 * \param     targetAdress - represents the target address
 *            numberOfBytes - represents the number of bytes
 *            *storing  - it's a pointer to the array where the data will be saved
 * \return    None 
 */
void I2C_vMasterRead(uint8_t targetAdress, uint8_t numberOfBytes, uint8_t * storingLocation);

/**
 * \brief     This function is used to set the response for a read requested by a master node;
 * \param     None
 * \return    None
 */
void I2C_vSlaveSetResponse(uint8_t * DataSource_ptr, uint8_t NumberOfBytes);
/**
 * \brief     This function is used to read received data by the slave from the RX buffer;
 * \param     None
 * \return    bool: true if new data was received - meaning that in *receivedData is a new value; 
 *                  false if no new data was put at receivedData; 
 */
I2C_SlaveOperationType I2C_vSlaveMainFunction(uint8_t * receivedData, uint16_t * matchedAdress, bool * isAStop);
/**
 * \brief     This function is used to read received data by the slave from the RX buffer;
 * \param     None
 * \return    bool: true if new data was received - meaning that in *receivedData is a new value; 
 *                  false if no new data was put at receivedData; 
 */
bool I2C_bStopDetected(void);

bool I2C_bOperationWasARead(void);

bool I2C_bIsMasterModeActive(void);
/**
 * \brief     This function is used to set the clock frequency of the I2C module;
 * \param     clkID * 25 = desired baud rate in khz
 * \return    
 */
void I2C_vSetCLK(uint8_t clkID);

uint8_t I2C_uiGetNumberOfResponseBytes(void);

#endif	/* I2C_H */

