/**
 * \file       I2C.c
 * \author     Iosub Mihai Alexandru
 * \brief      This module controls the I2C modules.
 */

/*----------------------------------------------------------------------------*/
/*                                 Includes                                   */
/*----------------------------------------------------------------------------*/
#include <pic18f26k83.h>

#include "I2C.h"
/*----------------------------------------------------------------------------*/
/*                               Local defines                                */
/*----------------------------------------------------------------------------*/
#define CNT_VALUE_SEND_DATA                 (2u)
#define I2C2_SET_CNT_VALUE(x)               (I2C2CNT = x)
#define I2C2_GET_CNT_VALUE()                  (I2C2CNT)
#define I2C2_SET_TARGER_ADR(adr, rw)        (I2C2ADB1 = ((adr << 1) | rw))
#define I2C2_WRITE_TXB(x)                   (I2C2TXB = x)
#define I2C2_TXIF_POSITION                  (1u)
#define I2C2_CLRBF_POSITION                 (2u)
#define I2C2_TXBE_POSITION                  (5u)
#define I2C2_RXBF_POSITION                  (0u)
#define I2C2_MDR_POSITION                   (3u)
#define I2C2_S_POSITION                     (5u)
#define I2C2_GET_TXIF()                     (MASK_8BIT_GET_BIT(PIR7, I2C2_TXIF_POSITION))
#define I2C2_IS_TXB_EMPTY()                 (MASK_8BIT_GET_BIT(I2C2STAT1, I2C2_TXBE_POSITION))
#define I2C2_IS_RXB_FULL()                  (MASK_8BIT_GET_BIT(I2C2STAT1, I2C2_RXBF_POSITION))
#define I2C2_IS_MASTER_PAUSED()             (MASK_8BIT_GET_BIT(I2C2CON0, I2C2_MDR_POSITION))
#define I2C2_SET_START()                    (MASK_8BIT_SET_BIT(I2C2CON0, I2C2_S_POSITION))
/*----------------------------------------------------------------------------*/
/*                              Local data types                              */

/*----------------------------------------------------------------------------*/
typedef enum
{
    I2C_OPERATION_WRITE = 0u,
    I2C_OPERATION_READ,
} I2C_OperationType;
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
 * \brief     This function waits for ACK to be received
 * \param     None
 * \return    None 
 */
void I2C2_vWaitACK(void);
void inline I2C2_vModuleDisable(void);
void inline I2C2_vModuleEnable(void);
bool I2C2_bStopDetected(void);
/*----------------------------------------------------------------------------*/
/*                     Implementation of global functions                     */

/*----------------------------------------------------------------------------*/
void I2C_vInit(void)
{
    I2C2CON0 = 0x0C;
    I2C2CON2 = 0x20;
    I2C2CLK = 0x03;
    MASK_8BIT_SET_BIT(I2C1STAT1, 2);
// de aici in jos is copy paste de pe proj vechi
    TRISC &= ~0x18;
    ANSELC &= ~0x18;
    WPUC |= 0x18;
    ODCONC |= 0x18;

    RC3I2C = 0x61; /* pULL uP, Slew Rate & Threshold */
    RC4I2C = 0x61; /* pULL uP, Slew Rate & Threshold */

    RC3PPS = 0x23; /* Selecting which module outputs on RC3 */
    RC4PPS = 0x24; /* Selecting which module outputs on RC4 */

    I2C2SDAPPS = 0x14; /* Feeding I2C1SDA from pin RC4 */
    I2C2SCLPPS = 0x13; /* Feeding I2C1SCL from pin RC3 */
}

void I2C_vMasterTransmit(uint8_t targetAdress, uint8_t targetRegister, uint8_t dataToBeSent)
{
    I2C2_vModuleEnable();
    I2C2_SET_CNT_VALUE(CNT_VALUE_SEND_DATA);
    //I2C2_SET_TARGER_ADR(targetAdress, I2C_OPERATION_WRITE);
    I2C2_SET_TARGER_ADR(targetAdress, I2C_OPERATION_WRITE);
    I2C2_WRITE_TXB(targetRegister);
    I2C2_SET_START();
    /*while ((false == I2C2_bStopDetected()) || (I2C2_GET_CNT_VALUE() != 0x00))
    {
        if (1u == I2C2_IS_MASTER_PAUSED())
        {
            if (1u == I2C2_IS_RXB_FULL())
            {

            }
            else if (1u == I2C2_IS_TXB_EMPTY())
            {
                I2C2_WRITE_TXB(dataToBeSent);
            }
        }
    }*/
    while(0u == I2C2_IS_TXB_EMPTY())
    {
        
    }
    I2C2_WRITE_TXB(dataToBeSent);
}
/*----------------------------------------------------------------------------*/
/*                     Implementation of local functions                      */

/*----------------------------------------------------------------------------*/
void I2C2_vWaitACK(void)
{
    //while()
}

bool I2C2_bStopDetected(void)
{
    bool returnValue = false;
    if (1u == I2C2PIRbits.PCIF)
    {
        returnValue = true;
        I2C2PIRbits.PCIF = 0;
        I2C2_vModuleDisable();
    }
    return returnValue;
}

void inline I2C2_vModuleEnable(void)
{
    I2C2CON0bits.EN = 1;
}

void inline I2C2_vModuleDisable(void)
{
    I2C2CON0bits.EN = 0;
}