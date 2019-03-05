
#ifndef HEAD_H_
#define HEAD_H_

/**
 * \file       Head.h
 * \author     Nicoara Ioan
 * \brief      Short description for this header file
 */


/*----------------------------------------------------------------------------*/
/*                                  Includes                                  */
/*----------------------------------------------------------------------------*/
#include <xc.h>

#include "../../03_Common/types.h"
/*----------------------------------------------------------------------------*/
/*                             Defines and macros                             */
/*----------------------------------------------------------------------------*/
#define DEFAULT_BAUDRATE    (0x682u) //default baudrate 9600
#define MIN_BAUD_VAL        (110u)   //min baudrate value
#define MAX_BAUD_VAL        (460800u)//max baudrate value

typedef enum
{
    NORMAL_SPEED = 0u,
    HIGH_SPEED,
}BaudGeneratorSpeed;

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
 * \brief     This function initialize the UART Module;
 * \param     None
 * \return    None 
 */
void UART1_vInit(void);

/**
 * \brief     This function is used for send values on UART Asynchronous Transmitter
 * \param     valSend - represents value wich to be send on RX
 * \return    None 
 */
void UART1_uiTransmitter(uint8_t valSend);

/**
 * \brief     This function is used for receive values on UART Asynchronous Receiver
 * \param     * var
 * \return    None 
 */
uint8_t UART1_uiReception();

/**
 * \brief     This function automatically calculates Baud Rate and sets the operating mode (NORMAL_SPEED or HIGH_SPEED);
 * \param     GeneratorSpeed - sets the operating mode (NORMAL_SPEED:HIGH_SPEED) 
 *            DesiredBaud - desired BaudRate (110u - 460800u)
 * \return    None 
 */
void UART1_vBaudCalculator( BaudGeneratorSpeed GeneratorSpeed, uint32_t DesiredBaud);

/**
 * \brief     This function selects the UART mode
 * \param     Mode - operating mode (8-bit / 7-bit / 8-bit with odd parity in the 9th bit / 8-bit with even parity in the 9th bit
              8-bit with address indicator in the 9th bit / LIN only slave mode / LIN Master mode)
 * \return    None 
 */
void UART1_vUARTMode(UartMode Mode);

/**
 * \brief     This function selects number of stop bits from UART frame
 * \param     NoStopBits - 1 transmit with receive verify on first / 1.5 transmit with receive verify on first
                           2 transmit with receive verify on both / 2 transmit with receive verify on first only
 * \return    None 
 */
void UART1_vStopBitMode(StopBitMode NoStopBits);

/**
 * \brief     This function selects polarity bits from UART frame
 * \param     Polarity - '1' Output data is inverted => TX output is low in idle state
 *                       '0' Output data is non-inverted => TX output is high in idle state
 * \return    None 
 */
void UART1_vTransmitPolarityControl(TransmitPolarity Polarity);

/**
 * \brief     This function sets HW Hlow Control from UART
 * \param     FlowControl - FLOW_CONTROL_OFF => FlowControl is OFF
 *                          RTS_CTS_HW_LOW_CONTROL => RTS/CTS and TXDE Hardware Flow Control
 * \return    None 
 */
void UART1_vHandshakeFlowControl(HandshakeFlowControl FlowControl);

#endif /* HEAD_H_ */
