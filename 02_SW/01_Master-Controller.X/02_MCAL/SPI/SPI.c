/**
 * \file       spi.c
 * \author     Ioan Nicoara
 * \brief      Short description for this source file
 */

/*----------------------------------------------------------------------------*/
/*                                 Includes                                   */
/*----------------------------------------------------------------------------*/
#include <pic18f26k83.h>
#include "SPI.h"
/*----------------------------------------------------------------------------*/
/*                               Local defines                                */
/*----------------------------------------------------------------------------*/
#define SPI_TX_COMPLETE             MASK_8BIT_GET_BIT(PIR2, 5)  //  1 = Interrupt has occurred    
                                                                //  0 = Interrupt event has not occurred 
#define SPI_RX_COMPLETE             MASK_8BIT_GET_BIT(PIR2, 4)
#define TRANSMIT_BUFFER_EMPTY       MASK_8BIT_GET_BIT(SPI1STATUS, 5)
#define SPI_SCK                     (0xB3)
#define SPI_MOSI                    (0xC1)
#define SPI_MISO                    (0xC0)
#define SPI_SLAVE_EN                (0xA5)

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
 * \brief     This function is used for send values on SPI Master Transmitter;
 * \param     data - represents value wich to be send on MOSI
 * \return    None 
 */
void SPI_vTransmit(uint8_t data);

/**
 * \brief     This function is used for receive values on SPI Master Receiver
 * \param     None
 * \return    returns the value received in the SPI1RXB register  
 */
uint8_t SPI_uiReceive();

/**
 * \brief     This function automatically calculates BaudRate for the SPI Module
 * \param     DesiredBaud - desired BaudRate 
 * \return    None 
 */
void SPI_vBaudRateCalculator(uint32_t DesiredBaud);
/*----------------------------------------------------------------------------*/
/*                     Implementation of global functions                     */
/*----------------------------------------------------------------------------*/
void SPI_vInit(uint8_t OperationMode)
{
    SPI1CON0 = RESET_VALUE;
    SPI1INTF = RESET_VALUE;
    SPI1INTE = RESET_VALUE;
    SPI1TCNTL = RESET_VALUE;
    SPI1TCNTH = RESET_VALUE;
    SPI1TWIDTH = RESET_VALUE;
    SPI1CLK = RESET_VALUE;      //CLKSEL FOSC; 
        
    if(OperationMode == MASTER_MODE)
    {
        RB3PPS = 0x1E;          //SPI_SCK_SLAVE_INPUT:RB3
        RC1PPS = 0x1F;          //SPI_MOSI_INPUT:RC1
        SPI1SDIPPS = 0x10;      //SPI_MISO_OUTPUT:RC0
        RA5PPS = 0x20;          //SPI_SS_INPUT:RA5
        SPI1BAUD = 0x1F;        //1MHz
        SPI1CON1 = 0x40;
        SPI1CON2 = 0X07;
        SPI1CON0 = 0x83;        //BMODE last byte; LSBF MSb first; EN enabled; MST bus master; 
        GPIO_vSetPinDirection(SPI_SCK, GPIO_OUTPUT_PIN);
        GPIO_vSetPinDirection(SPI_MOSI, GPIO_OUTPUT_PIN);
        GPIO_vSetPinDirection(SPI_MISO, GPIO_INPUT_PIN);
        GPIO_vSetPinDirection(SPI_SLAVE_EN,GPIO_OUTPUT_PIN);
    }
    else if(OperationMode == SLAVE_MODE)
    {
        SPI1SCKPPS = 0x0B;      //SPI_SCK_SLAVE_INPUT
        SPI1SDIPPS = 0x11;      //SPI_MOSI_INPUT
        RC0PPS = 0x1F;          //SPI_MISO_OUTPUT
        SPI1SSPPS = 0x05;       //SPI_SS_INPUT
        SPI1BAUD = RESET_VALUE;
        SPI1CON1 = 0x40;
        SPI1CON2 = 0x07;
        SPI1CON0 = 0x80;
        GPIO_vSetPinDirection(SPI_SCK, GPIO_INPUT_PIN);
        GPIO_vSetPinDirection(SPI_MOSI, GPIO_INPUT_PIN);
        GPIO_vSetPinDirection(SPI_MISO, GPIO_OUTPUT_PIN);
        GPIO_vSetPinDirection(SPI_SLAVE_EN,GPIO_INPUT_PIN);
    }
    else
    {
        
    }
}

uint8_t SPI_uiExchangeByte(uint8_t data)
{
    uint8_t ReceivedData;   //Represents the received data
    
    SPI1TCNTL = 1u;  //One byte transfer count
    SPI_vTransmit(data);
    ReceivedData = SPI_uiReceive();
    
    return ReceivedData;
}

uint8_t SPI_uiExchangeXBytes(uint8_t *data, uint8_t NoOfBytes)
{
    uint8_t byte_count = 0u;
    uint8_t ReceivedData;   //Represents the received data
    SPI1TCNTL = NoOfBytes;
    
    for(byte_count=0u; byte_count < NoOfBytes; byte_count++)
    {
        if(TRANSMIT_BUFFER_EMPTY == 1u)
        {
            SPI_vTransmit(data[byte_count]);
            ReceivedData = SPI_uiReceive();
        }
        else
        {
            byte_count--;
        }
    }
    return ReceivedData;
}

void SPI_vSlaveState(uint8_t state)
{
    if(state == SS_ENABLED)
    {
        GPIO_vSetPinLevel(SPI_SLAVE_EN, STD_HIGH);
    }
    else if(state == SS_DISABLED)
    {
        GPIO_vSetPinLevel(SPI_SLAVE_EN, STD_LOW);
    }
    else
    {
        
    }
}

/*----------------------------------------------------------------------------*/
/*                     Implementation of local functions                      */
/*----------------------------------------------------------------------------*/
void SPI_vBaudRateCalculator(uint32_t DesiredBaud)
{
    SPI1BAUD = (_XTAL_FREQ/(2u*DesiredBaud))-1u;
}

void SPI_vTransmit(uint8_t data)
{
    while(STD_LOW == SPI_TX_COMPLETE)
    {
    }
    
    SPI1TXB = data;
}

uint8_t SPI_uiReceive()
{
    while(STD_LOW == SPI_RX_COMPLETE)
    {
    }
    
    return SPI1RXB;
}


