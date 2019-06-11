/* 
 * File:   MainApplication.h
 * Author: Alex
 *
 * Created on March 19, 2019, 1:35 PM
 */

#ifndef MAINAPPLICATION_H
#define	MAINAPPLICATION_H

/*----------------------------------------------------------------------------*/
/*                                  Includes                                  */
/*----------------------------------------------------------------------------*/
#include "../../02_MCAL/GPIO/GPIO.h"
#include "../../02_MCAL/TIMER0/Timer0.h"
#include "../../02_MCAL/TIMER2/Timer2.h"
#include "../TimeoutModule/TimeoutModule.h"
#include "../../02_MCAL/I2C/I2C.h"
#include "../../02_MCAL/CAN/CAN.h"
#include "../../02_MCAL/SPI/SPI.h"
#include "../Interrupt/Interrupt.h"
#include "../02_MCAL/RS232/RS232.h"
/*----------------------------------------------------------------------------*/
/*                             Defines and macros                             */
/*----------------------------------------------------------------------------*/
#define MAX_BYTES_ON_TRANSACTION    (8u)
#define DTBS_CAN_FIFO_SIZE                  (32u)
#define DTBS_I2C_FIFO_SIZE                  (32u)
#define DTBS_RS232_FIFO_SIZE                (32u)
#define LKT_I2C_SIZE                        (16u)
#define LKT_CAN_SIZE                        (16u)
/* This is is when asking the configurator for data */
#define SPI_REQUEST_DATA_VALUE              (0x21)
#define APP_START_BYTE_MASK                 (0xF8)
#define APP_START_BYTE                      (0xF8)
#define NUMBER_OF_PROTOCOLS                 (4u)
#define CONFIGURATOR_GET_PROTOCOL_ID(x)     ((x >> 1) & (0x03))
#define CONFIGURATOR_GET_PROTOCOL_STATE(x)  (MASK_8BIT_GET_BIT(x,0))
#define BYTES_PER_LKT                       (5u)
#define MASTER_SLAVE_MODE_POSITION          (7u)
#define RETRANSMISION_MASK                  (0xFE)
#define RETRANSMISION_VALUE                 (0xA8)
#define END_OF_CONFIG_MASK                  (0xF8)
#define END_OF_CONFIG_VALUE                 (0x58)
#define START_CONVERSION_FLAG_MASK          (0xFE)
#define START_CONVERSION_FLAG_VALUE         (0x64)
#define PROTOCOL_CONFIG_NEEDS_RETRANSMISION(x)  ((x & RETRANSMISION_MASK) == RETRANSMISION_VALUE)
#define END_OF_CONFIGURATION_BYTE(x)            ((x & END_OF_CONFIG_MASK) == END_OF_CONFIG_VALUE)
#define GET_PROTOCOL_FROM_LKT_LAST_BYTE(x)      (x & 0x03)
/*----------------------------------------------------------------------------*/
/*                                 Data types                                 */

/*----------------------------------------------------------------------------*/
typedef enum {
    APP_CONFIGURATION_STATE = 0u,
    APP_RUNNING_STATE,
} MainApplication_State;

typedef enum {
    DATA_PENDING = 0u,
    DATA_READY,
    DATA_WAS_SENT,
} MainApplication_DataState;

typedef enum {
    REQUEST_IDLE = 0u,
    REQUEST_DATA_PENDING,
    REQUEST_DATA_READY,

} MainApplication_RequestState;

typedef struct {
    MainApplication_DataState DataState;
    uint32_t TargetLocation;
    uint8_t Data[MAX_BYTES_ON_TRANSACTION];
    uint8_t NextIndex;
} MainApplication_DataToBeSent;

typedef struct {
    /* This is the matched slave address or the identifier in the case of CAN / LIN protocols from where data was received*/
    MainAplication_Protocol TargetProtocol;
    MainApplication_RequestState StateOfTheRequest;
    /* This points to the DataToBeSent element that contains the data that needs to be sent in order to respond to the request*/
    MainApplication_DataToBeSent * Request_DTBS_ptr;
    uint32_t TargetLocation;
    uint16_t Receiver;
} MainApplication_LookUpTable;


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

#endif	/* MAINAPPLICATION_H */

