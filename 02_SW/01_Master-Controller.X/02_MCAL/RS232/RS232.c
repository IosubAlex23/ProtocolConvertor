/**
 * \file       RS232.c
 * \author     Ioan Nicoara
 * \brief      Short description for this source file
 */

/*----------------------------------------------------------------------------*/
/*                                 Includes                                   */
/*----------------------------------------------------------------------------*/
#include "RS232.h"
/*----------------------------------------------------------------------------*/
/*                               Local defines                                */
/*----------------------------------------------------------------------------*/

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
void RS232_vInit(RS232_Configuration * Config)
{
    UART1_vInit();
    UART1_vBaudCalculator(Config->communicationBaudGenSpeed, Config->communicationDesiredBaud);
    UART1_vUARTMode(Config->communicationUartMode);
    UART1_vStopBitMode(Config->communicationStopBitMode); 
    UART1_vTransmitPolarityControl(Config->communicationPolarity);
    UART1_vHandshakeFlowControl(Config->communicationHandshakeFlowControl);
}

void RS232_vSendData(uint8_t Data)
{
    UART1_uiTransmitter(Data);
}

uint8_t RS232_uiReceiveData()
{
    return UART1_uiReception();
}

bool RS232_bReceiveDataReady()
{
    return (bool)UART1_bRX_Ready();
}

bool RS232_bSendDataReady()
{
    return (bool)UART1_bTX_Ready();
}

/*----------------------------------------------------------------------------*/
/*                     Implementation of local functions                      */
/*----------------------------------------------------------------------------*/
