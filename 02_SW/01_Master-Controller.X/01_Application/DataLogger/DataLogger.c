/**
 * \file       DataLogger.c
 * \author     Iosub Mihai Alexandru
 * \brief      Short description for this source file
 */

/*----------------------------------------------------------------------------*/
/*                                 Includes                                   */
/*----------------------------------------------------------------------------*/
#include "DataLogger.h"
/*----------------------------------------------------------------------------*/
/*                               Local defines                                */
/*----------------------------------------------------------------------------*/
#define DATA_REQUESTED_START_FLAG            (0xEA)
#define DESTINATION_NOT_FOUND_START_FLAG     (0xEB)
#define DATA_CONVERTED_START_FLAG            (0xEC)
#define DATA_RECEIVED_START_FLAG            (0xED)
/*----------------------------------------------------------------------------*/
/*                              Local data types                              */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*                             Global data at RAM                             */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*                             Global data at ROM                             */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*                             Local data at RAM                              */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*                             Local data at ROM                              */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*                       Declaration of local functions                       */
/*----------------------------------------------------------------------------*/
/**
 * \brief     This function [...];
 * \param     None
 * \return    None 
 */
/*----------------------------------------------------------------------------*/
/*                     Implementation of global functions                     */

/*----------------------------------------------------------------------------*/
void DataLogger_vSendDataRequested(uint16_t sourceAdrress, MainAplication_Protocol sourceProtocol, uint16_t destinationAdrress, MainAplication_Protocol destinationProtocol)
{
    SPI_uiMasterExchangeByte(DATA_REQUESTED_START_FLAG);
    SPI_uiMasterExchangeByte(sourceProtocol);
    SPI_uiMasterExchangeByte(MASK_16BIT_GET_MSB_HALF(sourceAdrress));
    SPI_uiMasterExchangeByte(MASK_16BIT_GET_LSB_HALF(sourceAdrress));

    SPI_uiMasterExchangeByte(destinationProtocol);
    SPI_uiMasterExchangeByte(MASK_16BIT_GET_MSB_HALF(destinationAdrress));
    SPI_uiMasterExchangeByte(MASK_16BIT_GET_LSB_HALF(destinationAdrress));

    SPI_uiMasterExchangeByte(DATA_REQUESTED_START_FLAG);
    SPI_uiMasterExchangeByte(~DATA_REQUESTED_START_FLAG);
}

/**
 * \brief     This function sends log when a destination was not found
 * \param     None
 * \return    None 
 */
void DataLogger_vSendDestinationNotFound(uint16_t sourceAdrress, MainAplication_Protocol sourceProtocol, uint8_t * dataBytes, uint8_t noOfBytes)
{
    SPI_uiMasterExchangeByte(DESTINATION_NOT_FOUND_START_FLAG);
    SPI_uiMasterExchangeByte(sourceProtocol);
    SPI_uiMasterExchangeByte(MASK_16BIT_GET_MSB_HALF(sourceAdrress));
    SPI_uiMasterExchangeByte(MASK_16BIT_GET_LSB_HALF(sourceAdrress));

    SPI_uiExchangeXBytes(dataBytes, noOfBytes);

    SPI_uiMasterExchangeByte(DESTINATION_NOT_FOUND_START_FLAG);
    SPI_uiMasterExchangeByte(~DESTINATION_NOT_FOUND_START_FLAG);
}

/**
 * \brief     This function send data when a conversion happened
 * \param     None
 * \return    None 
 */
void DataLogger_vSendDataConverted(uint16_t destinationAdrress, MainAplication_Protocol destinationProtocol, uint8_t * dataBytes, uint8_t noOfBytes)
{
    SPI_uiMasterExchangeByte(DATA_CONVERTED_START_FLAG);
    SPI_uiMasterExchangeByte(destinationProtocol);
    SPI_uiMasterExchangeByte(MASK_16BIT_GET_MSB_HALF(destinationAdrress));
    SPI_uiMasterExchangeByte(MASK_16BIT_GET_LSB_HALF(destinationAdrress));

    SPI_uiExchangeXBytes(dataBytes, noOfBytes);

    SPI_uiMasterExchangeByte(DATA_CONVERTED_START_FLAG);
    SPI_uiMasterExchangeByte(~DATA_CONVERTED_START_FLAG);
}

void DataLogger_vSendDataReceived(uint16_t sourceAdrress, MainAplication_Protocol sourceProtocol, uint8_t * dataBytes, uint8_t noOfBytes)
{
    SPI_uiMasterExchangeByte(DATA_RECEIVED_START_FLAG);
    SPI_uiMasterExchangeByte(sourceProtocol);
    SPI_uiMasterExchangeByte(MASK_16BIT_GET_MSB_HALF(sourceAdrress));
    SPI_uiMasterExchangeByte(MASK_16BIT_GET_LSB_HALF(sourceAdrress));

    SPI_uiExchangeXBytes(dataBytes, noOfBytes);

    SPI_uiMasterExchangeByte(DATA_RECEIVED_START_FLAG);
    SPI_uiMasterExchangeByte(~DATA_RECEIVED_START_FLAG);
}
/*----------------------------------------------------------------------------*/
/*                     Implementation of local functions                      */
/*----------------------------------------------------------------------------*/
