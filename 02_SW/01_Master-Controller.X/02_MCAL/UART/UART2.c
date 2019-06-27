/**
 * \file       UART2.c
 * \author     Ioan Nicoara
 * \brief      Short description for this source file
 */

/*----------------------------------------------------------------------------*/
/*                                 Includes                                   */
/*----------------------------------------------------------------------------*/
#include "UART2.h"

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
void UART2_vInit(void)
{
    GPIO_vSetPinDirection(0xB2, GPIO_OUTPUT_PIN); // PIN B2 set as output
    GPIO_vSetPinDirection(0xB1, GPIO_INPUT_PIN); // PIN B1 set as input
    RB2PPS = 0x16;              //  UART2 => set TX on PORTB pin 2
    U2RXPPS = 0x09;              //  UART2 => set RX on PORTB pin 1 
    U2CON0 = 0xB0;              //  BRGS:1(HighSpeed) / TXEN:1(Transmie Enable) / RXEN:1(Receive Enable) 
    U2CON1 = 0x80;              //  ON:1(Serial Port Enable))
    U2CON2 = 0x00;       //  HW flow control RTS/CTS 
    U2ERRIR = RESET_VALUE;
    //U2ERRIE = RESET_VALUE;
    U2UIR = RESET_VALUE;
    U2FIFO = RESET_VALUE;
    U2BRG = DEFAULT_BAUDRATE;
//    U2P1L = RESET_VALUE;
//    U2P2 = RESET_VALUE;
//    U2P3 = RESET_VALUE;
    U2TXCHK  = RESET_VALUE;
}

void UART2_vTransmitter(uint8_t valSend)
{   
    while(STD_HIGH == TX2_BUFFER_FULL_STATUS) // TX complete
    {
    }
    U2TXB = valSend; 
    
}

uint8_t UART2_uiReception(uint8_t *received_data)
{
    if(STD_HIGH == RX2_INTERRUPT_FLAG)
    {
        *received_data = U2RXB;
        return 1;
    } 
   return 0;
}

/*----------------------------------------------------------------------------*/
/*                     Implementation of local functions                      */
/*----------------------------------------------------------------------------*/
void UART2_vBaudCalculator( BaudGeneratorSpeed GeneratorSpeed, uint32_t DesiredBaud)
{
    if(((GeneratorSpeed > HIGH_SPEED) && (GeneratorSpeed < NORMAL_SPEED)) || ((DesiredBaud < MIN_BAUD_VAL) && (DesiredBaud > MAX_BAUD_VAL)))
    {
        U2BRG = DEFAULT_BAUDRATE; //default BaudRate = 9600
        MASK_8BIT_SET_BIT(U2CON0, 7u); // U2CON0:BRGS = "HIGH" => BaudRate generator is HighSpeed MODE
    }
    else if(HIGH_SPEED == GeneratorSpeed)
    {
        MASK_8BIT_SET_BIT(U2CON0, 7u); // U2CON0:BRGS = "STD_HIGH" => BaudRate generator is HighSpeed MODE
        U2BRG = (_XTAL_FREQ / (4 * DesiredBaud)) - 1; // BaudRate formula for HighSpeed MODE
    }
    else if(NORMAL_SPEED == GeneratorSpeed)
    {
        MASK_8BIT_CLEAR_BIT(U2CON0, 7u); // U2CON0:BRGS = "STD_LOW" => BaudRate generator is NormalSpeed MODE
        U2BRG = (_XTAL_FREQ / (16 * DesiredBaud)) - 1; // BaudRate formula for NormalSpeed MODE
    }
    else
    {
        
    }
}

void UART2_vUARTMode(UartMode Mode)
{
    MASK_8BIT_CLEAR_BIT(U2CON1, 7u); // serial port disabled
    U2CON0 = U2CON0 & LSB_MASK;     // clear 4LSB from U2CON0
    U2CON0 |= Mode;                 // select mode
    MASK_8BIT_SET_BIT(U2CON1, 7u);   // serial port enabled   
}

void UART2_vStopBitMode(StopBitMode NoStopBits)
{
    if((NoStopBits < ONE_STOP_BIT) && (NoStopBits > TWO_STOP_BITS_FIRST_VERIF))
    {
        U2CON2 = (U2CON2 & STOP_BITS_MASK) | (ONE_STOP_BIT << 4u); // clear bits 5-4:U2CON2 AND set NoStopBit at this position (DEFAULT_VALUE: ONE_STOP_BIT)
    }
    else
    {
        U2CON2 = (U2CON2 & STOP_BITS_MASK) | (NoStopBits << 4u); // clear bits 5-4:U2CON2 AND set NoStopBit at this position
    }
}

void UART2_vTransmitPolarityControl(TransmitPolarity Polarity)
{
    if((Polarity < NON_INVERTED) && (Polarity > INVERTED))
    {
        U2CON2 = (U2CON2 & POLARITY_BIT_MASK) | (NON_INVERTED << 2u); // clear bits TXPOL:U2CON2 AND set Polarity at this position (DEFAULT_VALUE: NON_POLARITY))
    }
    else
    {
        U2CON2 = (U2CON2 & POLARITY_BIT_MASK) | (Polarity << 2u); // clear bits TXPOL:U2CON2 AND set Polarity at this position
    }
}

void UART2_vHandshakeFlowControl(HandshakeFlowControl FlowControl)
{
    if((RTS_CTS_HW_LOW_CONTROL == FlowControl) || (FLOW_CONTROL_OFF == FlowControl))
    {
        U2CON2 = (U2CON2 & FLOW_CONTROL_BITS_MASK) | (FlowControl); // clear bits FLO<1:0> : U2CON2 AND set FlowControl at this position 
    }
    else
    {
        U2CON2 = (U2CON2 & FLOW_CONTROL_BITS_MASK) | (FLOW_CONTROL_OFF); // clear bits FLO<1:0> : U2CON2 AND set Handshake Flow Control 
                                                                         //at this position (DEFAULT_VALUE: FLOW_CONTROL_OFF))
    }
}

bool UART2_bRX_Ready()
{
    return (bool)RX2_BUFFER_FULL_STATUS;
}
        
bool UART2_bTX_Ready()
{
    return (bool)(TX2_BUFFER_FULL_STATUS && TX2_IS_ENABLE);
}

        
