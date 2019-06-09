
#ifndef UART1_H_
#define UART1_H_

/**
 * \file       UART1.h
 * \author     Nicoara Ioan
 * \brief      Short description for this header file
 */


/*----------------------------------------------------------------------------*/
/*                                  Includes                                  */
/*----------------------------------------------------------------------------*/

#include "../../03_Common/types.h"
#include "UartTypes.h"
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
 * \brief     This function initialize the UART Module;
 * \param     None
 * \return    None 
 */
void UART1_vInit(void);

/**
 * \brief     This function is used for send values on UART Asynchronous Transmitter
 * \param     valSend - represents value wich to be send on TX
 * \return    None 
 */
void UART1_uiTransmitter(uint8_t valSend);

/**
 * \brief     This function is used for receive values on UART Asynchronous Receiver
 * \param    
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

/**
 * \brief     This function checks if the UART1 receiver ready for reading
 * \param     None
 * \return    Status of UART1 receiver
                TRUE: UART1 receiver is ready for reading
                FALSE: UART1 receiver is not ready for reading
 */
bool UART1_bRX_Ready();

/**
 * \brief     This function checks if the UART1 transmitter is ready to transmit data
 * \param     None
 * \return    Status of UART1 transmitter
                TRUE: UART1 transmitter is ready
                FALSE: UART1 transmitter is not ready
 */
bool UART1_bTX_Ready();

#endif /* HEAD_H_ */
