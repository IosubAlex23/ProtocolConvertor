/**
 * \file       LIN.c
 * \author     
 * \brief      Short description for this source file
 */

/*----------------------------------------------------------------------------*/
/*                                 Includes                                   */
/*----------------------------------------------------------------------------*/
#include <pic18f26k83.h>

#include "../UART/UART2.h"
#include "../UART/UartTypes.h"
#include "LIN.h"
/*----------------------------------------------------------------------------*/
/*                               Local defines                                */
/*----------------------------------------------------------------------------*/
#define DEFAULT_CHECKSUM_MODE       (LEGACY)
#define BREAK_STATUS                MASK_8BIT_GET_BIT(U2ERRIR, 2)
/*----------------------------------------------------------------------------*/
/*                              Local data types                              */
/*----------------------------------------------------------------------------*/
uint8_t lin_state = LIN_RX_BREAK;
LIN_packet LIN_Frame;
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
void LIN_vCheckSUMMode(CheckSUM_Mode CkSUM);
/*----------------------------------------------------------------------------*/
/*                     Implementation of global functions                     */
/*----------------------------------------------------------------------------*/
void LIN_vInit(uint8_t mode)
{
    UART2_vInit();
    RB2PPS = 0x16;              //  UART1 => set TX on PORTB pin 2
    U2RXPPS = 0x09;              //  UART1 => set RX on PORTB pin 1 
    GPIO_vSetPinDirection(0xB2, GPIO_OUTPUT_PIN); // PIN B2 set as output
    GPIO_vSetPinDirection(0xB1, GPIO_INPUT_PIN); // PIN B1 set as input
    
    MASK_8BIT_CLEAR_BIT(U2CON1, UART2_ENABLE);  //UART2 Disabled
    MASK_8BIT_SET_BIT(U2CON0, UART2_TX_EN); //Enable TX on UART2
    MASK_8BIT_SET_BIT(U2CON0, UART2_RX_EN); //Enable RX on UART2
    UART2_vUARTMode(mode);    //set UART2 Mode as LIN MASTER
    UART2_vBaudCalculator(HIGH_SPEED,9600);
    UART2_vTransmitPolarityControl(NON_INVERTED);
    UART2_vStopBitMode(ONE_STOP_BIT);
    LIN_vCheckSUMMode(ENHANCED);  
    
    MASK_8BIT_SET_BIT(U2CON1, UART2_ENABLE);  //UART2 ENABLED
}


/*----------------------------------------------------------------------------*/
/*                     Implementation of local functions                      */
/*----------------------------------------------------------------------------*/

void LIN_vCheckSUMMode(CheckSUM_Mode CkSUM)
{
    if(CkSUM < LEGACY || CkSUM > ENHANCED)
    {
        MASK_8BIT_CLEAR_BIT(U2CON2, CHECKSUM_BIT);  // sets CHECKSUM Mode as LEGACY (if CkSUM parameter is not in interval)[DEFAULT]
    }
    else if(CkSUM == LEGACY)
    {
        MASK_8BIT_CLEAR_BIT(U2CON2, CHECKSUM_BIT);   // sets CHECKSUM Mode as LEGACY
    }
    else if(CkSUM == ENHANCED)
    {
        MASK_8BIT_SET_BIT(U2CON2, CHECKSUM_BIT);    // sets CHECKSUM Mode as ENHANCED
    }
    else
    {
        
    }
}

void LIN_vTransmit(uint8_t identifier, uint8_t NoOfBytes, uint8_t *data)
{
    uint8_t byte_count = 0;
    U2P2L = RESET_VALUE;
    if((U2P2L == STD_LOW) && (TX2_INTERRUPT_FLAG == STD_LOW))
    {
        U2P2L = NoOfBytes+1;
        U2P3L = 0;
        if(MASK_8BIT_GET_LSB_HALF(U2CON0) == LIN_MASTER)
        {
            U2P1L = identifier;
            __delay_ms(2);
        }
        if(TX2_INTERRUPT_FLAG == 1)
        {
            for(byte_count = 0; byte_count<=NoOfBytes; byte_count++)
            {
                    UART2_vTransmitter(data[byte_count]);
            }
        }
    }
}

bool LIN_vTransmitReady()
{
    return (bool)UART2_bTX_Ready();
}

bool LIN_vReceiveReady()
{
    return (bool)UART2_bRX_Ready();
}





void LIN_stateCheck(void)
{    
    U2P3L = 2;
    uint8_t state;
    switch(lin_state)
        {
            case LIN_RX_BREAK:
            {
                state = BREAK_STATUS;
//                if(state == 1)
//                {
                    lin_state = LIN_RX_SYNC;
//                }
//                else
//                {
//                    lin_state = LIN_RX_BREAK;
//                }
                break;
            }
            
            case LIN_RX_SYNC:
            {
                state = UART2_uiReception();
                if(state == 0x55)
                {
                    lin_state = LIN_RX_PID;
                }
                else
                {
                   // lin_state = LIN_RX_BREAK;
                    lin_state = LIN_RX_PID;
                }                
                break;
            }
            
            case LIN_RX_PID:
            {
                LIN_Frame.pid = UART2_uiReception();
                lin_state = LIN_RX_DATA;
                break;
            }
            case LIN_RX_DATA:
            {
                    LIN_Frame.data[0] = UART2_uiReception();
                    LIN_Frame.data[1] = UART2_uiReception();
 
               
              
                lin_state = LIN_RX_CHECKSUM;
                break;
            }
            case LIN_RX_CHECKSUM:
            {
                LIN_Frame.checksum = UART2_uiReception();
                lin_state = LIN_RX_BREAK;
                break;
            }
        }    
}