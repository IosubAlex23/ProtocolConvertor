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
#include "../TimeoutModule/TimeoutModule.h"
#include "../../02_MCAL/I2C/I2C.h"
#include "../../02_MCAL/CAN/CAN.h"
/*----------------------------------------------------------------------------*/
/*                             Defines and macros                             */
/*----------------------------------------------------------------------------*/
#define MAX_BYTES_ON_TRANSACTION    (8u)
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

typedef enum {
    APP_PROTOCOL_CAN = 0u,
    APP_PROTOCOL_I2C,
    APP_PROTOCOL_LIN,
    APP_PROTOCOL_RS232,
    APP_PROTOCOL_UNKNOWN,
} MainAplication_Protocol;

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

