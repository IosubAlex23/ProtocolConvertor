/*
 * CAN.H
 *
 *  Created on: 22 Feb, 2019
 *      Author: Iosub Mihai Alexandru
 */

#ifndef CAN_H_
#define CAN_H_

/**
 * \file       CAN.h
 * \author     Iosub Mihai Alexandru
 * \brief      Short description for this header file
 */


/*----------------------------------------------------------------------------*/
/*                                  Includes                                  */
/*----------------------------------------------------------------------------*/
#include "../../03_Common/types.h"
#include "../../02_MCAL/GPIO/GPIO.h"
/*----------------------------------------------------------------------------*/
/*                             Defines and macros                             */
/*----------------------------------------------------------------------------*/
#define CAN_MAX_DATA_BYTES                  (8u)
#define CAN_NUMBER_OF_RXBF                   (2u)
#define CAN_NUMBER_OF_TXBF                   (3u)
#define CAN_NUMBER_OF_PROGRAMMABLE_BUFFERS   (6u)
#define CAN_NO_DATA_RECEIVED_BUFFER          (8u)
/*----------------------------------------------------------------------------*/
/*                                 Data types                                 */

/*----------------------------------------------------------------------------*/
typedef enum {
    CAN_NORMAL_OPERATION_MODE = 0,
    CAN_SLEEP_OPERATION_MODE,
    CAN_LOOPBACK_OPERATION_MODE,
    CAN_LISTEN_ONLY_OPERATION_MODE,
    CAN_CONFIGURATION_OPERATION_MODE,
    CAN_ERROR_RECOGNITION_OPERATION_MODE,
} CAN_OperationMode;

typedef enum {
    CAN_LEGACY_FUNCTIONAL_MODE = 0u,
    CAN_ENHANCED_FUNCTIONAL_MODE,
    CAN_ENHANCED_FIFO_FUNCTIONAL_MODE,
} CAN_FunctionalMode;

typedef enum {
    CAN_STANDARD_FRAME = 0u,
    CAN_EXTENDED_FRAME
} CAN_FrameType;

typedef enum {
    CAN_20KBITS = 0u,
    CAN_50KBITS,
    CAN_80KBITS,
    CAN_100KBITS,
    CAN_125KBITS,
    CAN_200KBITS,
    CAN_250KBITS,
    CAN_500KBITS,
    CAN_800KBITS,
    CAN_1MBIT,
} CAN_BaudRate;

typedef struct {
    uint32_t Frame_Identifier;
    bool Frame_RTR;
    uint8_t Frame_DataLength;
    uint8_t Frame_DataBytes[CAN_MAX_DATA_BYTES];
    uint8_t Frame_Priority; /* This should be masked with logic AND and 0x03 */
} CAN_Frame;

typedef struct {
    CAN_FunctionalMode Module_FunctionalMode;
    CAN_OperationMode Module_OperationMode;
    CAN_FrameType Module_FrameType;
    CAN_BaudRate Module_BaudRate;
    uint8_t Module_ReceiveFIFO_Size;
} CAN_Configuration;

typedef struct
{
    uint8_t BxCON;
    uint8_t BxSIDH;
    uint8_t BxSIDL;
    uint8_t BxEIDH;
    uint8_t BxEIDL;
    uint8_t BxDLC;
    uint8_t BxData[8];
    uint8_t BxCANSTAT;
    uint8_t BxCANCON;
} CAN_Buffer;
/*----------------------------------------------------------------------------*/
/*                 External declaration of global RAM-Variables               */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*                 External declaration of global ROM-Variables               */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*                  External declaration of global functions                  */
/*----------------------------------------------------------------------------*/
void CAN_vInit(CAN_Configuration * config);

/**
 * \brief     This function writes data to a transmit buffer.
 * \param     None
 * \return    None 
 */
void CAN_vTransmitFrame(CAN_Frame frame);

/**
 * \brief     This function configures the module.
 * \param     None
 * \return    None 
 */
void CAN_vConfigureModule(CAN_Configuration configuration);

/**
 * \brief     This function is used to set data bytes.
 * \param     None
 * \return    None 
 */
void CAN_vFrameSetData(CAN_Frame * frame, uint8_t * dataBytes, uint8_t numberOfDataBytes);


/**
 * \brief     This function is used to set data bytes.
 * \param     None
 * \return    None 
 */
uint8_t CAN_uiGetFIFOReadPointer(void);

/**
 * \brief     This function is used to check if the Receive buffers has new data
 * \param     None
 * \return    true if FIFO is not empty; false if it is empty; 
 */
bool CAN_bFIFOContainsData(void);

/**
 * \brief     This function is used to get the identifier from a received frame
 * \param     Number of the buffer
 * \return    the identifier from a received frame;
 */
uint32_t CAN_uiGetRxBufferIdentifier(uint8_t * bufferID);

/**
 * \brief     This function is used to get the identifier from a received frame
 * \param     CAN_Buffer
 * \return    the identifier from a received frame;
 */
uint32_t CAN_uiGetIdentifier(CAN_Buffer * buffer);

/**
 * \brief     This function is used to get a buffer based on it's ID
 * \param     bufferId
 * \return    the buffer with id bufferID;
 */
CAN_Buffer * CAN_uiGetBufferAdrress(uint8_t * bufferID);

/**
 * \brief     This function sets a bit on a buffer register that acknowledges the hardware that the buffer was read.
 * \param     target - represents the target buffer
 * \return    None
 */
void CAN_vSetBufferAsFree(CAN_Buffer * target);

/**
 * \brief     This function iterates all the receive buffers and check if 
 * \param     bufferId
 * \return    the buffer with id bufferID;
 */
bool CAN_bBufferHasNewData(CAN_Buffer * target);

/**
 * \brief     This function returns the exact number of buffers configured as receive; CAN_NUMBER_OF_RXBF(2)RXB defaults + the number of configured buffers
 * \param     None
 * \return    the number of buffers configured as receive
 */
uint8_t CAN_uiGetNumberOfReceiveBuffers(void);
#endif /* HEAD_H_ */
