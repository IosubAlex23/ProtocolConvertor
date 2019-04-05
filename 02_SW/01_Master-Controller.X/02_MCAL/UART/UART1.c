/**
 * \file       UART1.c
 * \author     Ioan Nicoara
 * \brief      Short description for this source file
 */

/*----------------------------------------------------------------------------*/
/*                                 Includes                                   */
/*----------------------------------------------------------------------------*/
#include "UART1.h"
#include "../GPIO/GPIO.h"
/*----------------------------------------------------------------------------*/
/*                               Local defines                                */
/*----------------------------------------------------------------------------*/
#define TX1_IS_ENABLE        (MASK_8BIT_GET_BIT(U1CON0,5))   // Transmit Enable bit
#define TX1_INTERRUPT_FLAG   (MASK_8BIT_GET_BIT(PIR3,4))     // UART1 Transmit Interrupt Flag bit   
#define RX1_INTERRUPT_FLAG   (MASK_8BIT_GET_BIT(PIR3,3))     // UART1 Receive Interrupt Flag bit
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

/*----------------------------------------------------------------------------*/
/*                     Implementation of global functions                     */
/*----------------------------------------------------------------------------*/
void UART1_vInit(void)
{
    GPIO_vSetPinDirection(0xC6, GPIO_OUTPUT_PIN); // PIN C6 set as output
    GPIO_vSetPinDirection(0xC7, GPIO_INPUT_PIN); // PIN C7 set as input
    RC6PPS = 0x13;              //  UART1 => set TX on PORTC pin 6
    U1RXPPS = 0x17;             //  UART1 => set RX on PORTC pin 7 
    U1CON0 = 0xB0;              //  BRGS:1(HighSpeed) / TXEN:1(Transmie Enable) / RXEN:1(Receive Enable) 
    U1CON1 = 0x80;              //  ON:1(Serial Port Enable))
    U1CON2 = RESET_VALUE;       //  HW flow control RTS/CTS 
    U1ERRIR = RESET_VALUE;
    U1ERRIR = RESET_VALUE;
    U1UIR = RESET_VALUE;
    U1FIFO = RESET_VALUE;
    U1BRG = DEFAULT_BAUDRATE;
    U1P1 = RESET_VALUE;
    U1P2 = RESET_VALUE;
    U1P3 = RESET_VALUE;
    U1TXCHK  = RESET_VALUE;
}

void UART1_uiTransmitter(uint8_t valSend)
{   
//    while(STD_LOW == TX1_INTERRUPT_FLAG) // TX complete
//    {
//    }
    U1TXB = valSend; 
}

uint8_t UART1_uiReception()
{
    while(STD_LOW == RX1_INTERRUPT_FLAG)
    {
        
    }
    
   return U1RXB;
}

/*----------------------------------------------------------------------------*/
/*                     Implementation of local functions                      */
/*----------------------------------------------------------------------------*/
void UART1_vBaudCalculator( BaudGeneratorSpeed GeneratorSpeed, uint32_t DesiredBaud)
{
    if(((GeneratorSpeed > HIGH_SPEED) && (GeneratorSpeed < NORMAL_SPEED)) || ((DesiredBaud < MIN_BAUD_VAL) && (DesiredBaud > MAX_BAUD_VAL)))
    {
        U1BRG = DEFAULT_BAUDRATE; //default BaudRate = 9600
        MASK_8BIT_SET_BIT(U1CON0, 7u); // U1CON0:BRGS = "HIGH" => BaudRate generator is HighSpeed MODE
    }
    else if(HIGH_SPEED == GeneratorSpeed)
    {
        MASK_8BIT_SET_BIT(U1CON0, 7u); // U1CON0:BRGS = "STD_HIGH" => BaudRate generator is HighSpeed MODE
        U1BRG = (_XTAL_FREQ / (4 * DesiredBaud)) - 1; // BaudRate formula for HighSpeed MODE
    }
    else if(NORMAL_SPEED == GeneratorSpeed)
    {
        MASK_8BIT_CLEAR_BIT(U1CON0, 7u); // U1CON0:BRGS = "STD_LOW" => BaudRate generator is NormalSpeed MODE
        U1BRG = (_XTAL_FREQ / (16 * DesiredBaud)) - 1; // BaudRate formula for NormalSpeed MODE
    }
    else
    {
        
    }
}

void UART1_vUARTMode(UartMode Mode)
{
    MASK_8BIT_CLEAR_BIT(U1CON1, 7u); // serial port disabled
    U1CON0 = U1CON0 & LSB_MASK;     // clear 4LSB from U1CON0
    U1CON0 |= Mode;                 // select mode
    MASK_8BIT_SET_BIT(U1CON1, 7u);   // serial port enabled   
}

void UART1_vStopBitMode(StopBitMode NoStopBits)
{
    if((NoStopBits < ONE_STOP_BIT) && (NoStopBits > TWO_STOP_BITS_FIRST_VERIF))
    {
        U1CON2 = (U1CON2 & STOP_BITS_MASK) | (ONE_STOP_BIT << 4u); // clear bits 5-4:U1CON2 AND set NoStopBit at this position (DEFAULT_VALUE: ONE_STOP_BIT)
    }
    else
    {
        U1CON2 = (U1CON2 & STOP_BITS_MASK) | (NoStopBits << 4u); // clear bits 5-4:U1CON2 AND set NoStopBit at this position
    }
}

void UART1_vTransmitPolarityControl(TransmitPolarity Polarity)
{
    if((Polarity < NON_INVERTED) && (Polarity > INVERTED))
    {
        U1CON2 = (U1CON2 & POLARITY_BIT_MASK) | (NON_INVERTED << 2u); // clear bits TXPOL:U1CON2 AND set Polarity at this position (DEFAULT_VALUE: NON_POLARITY))
    }
    else
    {
        U1CON2 = (U1CON2 & POLARITY_BIT_MASK) | (Polarity << 2u); // clear bits TXPOL:U1CON2 AND set Polarity at this position
    }
}

void UART1_vHandshakeFlowControl(HandshakeFlowControl FlowControl)
{
    if((RTS_CTS_HW_LOW_CONTROL == FlowControl) || (FLOW_CONTROL_OFF == FlowControl))
    {
        U1CON2 = (U1CON2 & FLOW_CONTROL_BITS_MASK) | (FlowControl); // clear bits FLO<1:0> : U1CON2 AND set FlowControl at this position 
    }
    else
    {
        U1CON2 = (U1CON2 & FLOW_CONTROL_BITS_MASK) | (FLOW_CONTROL_OFF); // clear bits FLO<1:0> : U1CON2 AND set Handshake Flow Control 
                                                                         //at this position (DEFAULT_VALUE: FLOW_CONTROL_OFF))
    }
}

bool UART1_bRX_Ready()
{
    return (bool)RX1_INTERRUPT_FLAG;
}
        
bool UART1_bTX_Ready()
{
    return (bool)(TX1_INTERRUPT_FLAG && TX1_IS_ENABLE);
}