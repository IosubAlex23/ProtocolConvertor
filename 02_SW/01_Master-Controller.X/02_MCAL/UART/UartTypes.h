/*
 * UartTypes.H
 *
 *  Created on: March 09, 2020
 *      Author: Ioan Nicoara
 */

#ifndef UARTTYPES_H_
#define UARTTYPES_H_

/**
 * \file       UartTypes.h
 * \author     Ioan Nicoara
 * \brief      Short description for this header file
 */


/*----------------------------------------------------------------------------*/
/*                                  Includes                                  */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*                             Defines and macros                             */
/*----------------------------------------------------------------------------*/
#define DEFAULT_BAUDRATE    (0x682u)    //default baudrate 9600
#define MIN_BAUD_VAL        (110u)      //min baudrate value
#define MAX_BAUD_VAL        (460800u)   //max baudrate value

#define RESET_VALUE             (0x00u)  //UART2 Module RESET Value MASK
#define LSB_MASK                (0xF0u)  //UART2 Module LSB Mask
#define STOP_BITS_MASK          (0xCFu)
#define POLARITY_BIT_MASK       (0xFBu) // 0x04
#define FLOW_CONTROL_BITS_MASK  (0xFCu)

/*----------------------------------------------------------------------------*/
/*                                 Data types                                 */
/*----------------------------------------------------------------------------*/
typedef enum
{
    NORMAL_SPEED = 0u,
    HIGH_SPEED,
}BaudGeneratorSpeed;

typedef enum
{
    BAUD_110 = 110u,
    BAUD_150 = 150u,
    BAUD_300 = 300u,
    BAUD_1200 = 1200u,
    BAUD_2400 = 2400u,
    BAUD_4800 = 4800u,
    BAUD_9600 = 9600u,
    BAUD_19200 = 19200u,
    BAUD_38400 = 38400u,
    BAUD_57600 = 57600u,
    BAUD_115200 = 115200u,
    BAUD_230400 = 230400u,
    BAUD_460800 = 460800u,
}BaudValue;

typedef enum
{
    ASYNC_8BIT = 0x0u,          // 8-bit
    ASYNC_7BIT,                 // 7-bit
    ASYNC_8BIT_ODD,             // 8-bit with odd parity in the 9th bit
    ASYNC_8BIT_EVEN,            // 8-bit with even parity in the 9th bit
    ASYNC_9BIT,                 // 8-bit with address indicator in the 9th bit
    LIN_SLAVE = 0xBu,           // LIN only slave mode
    LIN_MASTER,                 // LIN Master mode
}UartMode;

typedef enum
{
    ONE_STOP_BIT = 0x00,            // 1 transmit with receive verify on first
    ONE_FIVE_STOP_BITS,             // 1.5 transmit with receive verify on first
    TWO_STOP_BITS_BOTH_VERIF,       // 2 transmit with receive verify on both
    TWO_STOP_BITS_FIRST_VERIF,      // 2 transmit with receive verify on first only
}StopBitMode;

typedef enum
{
    NON_INVERTED = 0u,              // Output data is non-inverted => TX output is high in idle state 
    INVERTED,                       // Output data is inverted => TX output is low in idle state
}TransmitPolarity;

typedef enum
{
    FLOW_CONTROL_OFF = 0x0u,        // FlowControl is OFF
    RTS_CTS_HW_LOW_CONTROL = 0x2u,  // RTS/CTS and TXDE Hardware Flow Control
}HandshakeFlowControl;
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

#endif /* HEAD_H_ */
