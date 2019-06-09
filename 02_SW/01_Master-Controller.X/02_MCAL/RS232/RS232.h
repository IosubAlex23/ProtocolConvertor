#ifndef RS232_H_
#define RS232_H_

/**
 * \file       RS232.h
 * \author     Ioan Nicoara
 * \brief      
 */


/*----------------------------------------------------------------------------*/
/*                                  Includes                                  */
/*----------------------------------------------------------------------------*/
#include "../../03_Common/types.h"
#include "../../02_MCAL/UART/UART1.h"
/*----------------------------------------------------------------------------*/
/*                             Defines and macros                             */
/*----------------------------------------------------------------------------*/
typedef struct
{
    BaudGeneratorSpeed communicationBaudGenSpeed;
    BaudValue communicationDesiredBaud;
    UartMode communicationUartMode;
    TransmitPolarity communicationPolarity;
    StopBitMode communicationStopBitMode;
    HandshakeFlowControl communicationHandshakeFlowControl;
}RS232_Configuration;
/*----------------------------------------------------------------------------*/
/*                                 Data types                                 */
/*----------------------------------------------------------------------------*/
RS232_Configuration RS232_actualConfig;
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
 * \brief     This function initialize the RS232 Module;
 * \param     * Config
 * \return    None 
 */
void RS232_vInit(RS232_Configuration * Config);

/**
 * \brief     This function is used for send data on RS232
 * \param     Data - represents value wich to be send on TX
 * \return    None 
 */
void RS232_vSendData(uint8_t Data);

/**
 * \brief     This function is used for receive values on RS232
 * \param     -
 * \return    None 
 */
uint8_t RS232_uiReceiveData();

/**
 * \brief     This function checks if the RS232 receiver ready for reading
 * \param     None
 * \return    Status of RS232 receiver
                TRUE: RS232 receiver is ready for reading
                FALSE: RS232 receiver is not ready for reading
 */
bool RS232_bReceiveDataReady();

/**
 * \brief     This function checks if the RS232 transmitter is ready to transmit data
 * \param     None
 * \return    Status of RS232 transmitter
                TRUE: RS232 transmitter is ready
                FALSE: RS232 transmitter is not ready
 */
bool RS232_bSendDataReady();


#endif /* HEAD_H_ */
