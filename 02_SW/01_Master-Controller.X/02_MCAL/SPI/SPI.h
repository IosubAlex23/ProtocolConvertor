/*
 * SPI.h
 *
 *  Created on: March 17, 2019
 *      Author: Ioan Nicoara
 */

#ifndef SPI_H_
#define SPI_H_

/*----------------------------------------------------------------------------*/
/*                                  Includes                                  */
/*----------------------------------------------------------------------------*/
#include "../../03_Common/types.h"
#include "../GPIO/GPIO.h"

/*----------------------------------------------------------------------------*/
/*                             Defines and macros                             */
/*----------------------------------------------------------------------------*/

#define SLAVE_MODE      (0u)
#define MASTER_MODE     (1u)
#define SS_ENABLED      (STD_HIGH)
#define SS_DISABLED     (STD_LOW)

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
 * \brief     This function initializes the SPI Master and Slave Module;
 * \param     None
 * \return    None 
 */
void SPI_vInit(uint8_t OperationMode);

/**
 * \brief     This function returns if the transfer is complete or not complete;
 * \param     None
 * \return    0 - Either no data transfers have occurred or a data transfer is in progress
 *            1 - The data transfer is complete
 */
bool SPI_bMasterCompleteTransfer();

/**
 * \brief     This function selects the slave to transfer
 * \param     state - represents state of the slave (ENABLED or DISABLED)
 * \return    none
 */
void SPI_vSlaveState(uint8_t state);


/**
 * \brief     This function is used for send and receive values on SPI Master/Slave Module (Exchanges a data byte over SPI);
 * \param     data - data byte to be transmitted over SPI bus
 * \return    ReceivedData - The received byte over SPI bus 
 */
uint8_t SPI_uiExchangeByte(uint8_t data);

/**
 * \brief     This function is used for send and receive values on SPI Master/Slave Module (Exchanges X data bytes over SPI);
 * \param     *data - data byte to be transmitted over SPI bus
 *            NoOfBytes - Represents the number of the bytes transmitted/received over SPI
 * \return    ReceivedData - The received byte over SPI bus 
 */
uint8_t SPI_uiExchangeXBytes(uint8_t *data, uint8_t NoOfBytes);

#endif /* HEAD_H_ */
