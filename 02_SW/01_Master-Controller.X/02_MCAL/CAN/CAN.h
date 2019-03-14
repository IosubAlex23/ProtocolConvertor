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
    CAN_FrameType     Module_FrameType;
    uint8_t           Module_ReceiveFIFO_Size;
} CAN_Configuration;
/*----------------------------------------------------------------------------*/
/*                 External declaration of global RAM-Variables               */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*                 External declaration of global ROM-Variables               */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*                  External declaration of global functions                  */
/*----------------------------------------------------------------------------*/
void CAN_vInit();

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

#endif /* HEAD_H_ */
