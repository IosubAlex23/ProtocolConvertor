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

void I2C_vMasterRead(uint8_t targetAdress, uint8_t targetRegister, uint8_t numberOfBytes, uint8_t * storingLocation);

uint8_t I2C_vSlaveRead(void);
#endif	/* I2C_H */

