/**
 * \file       I2C.c
 * \author     Iosub Mihai Alexandru
 * \brief      This module controls the I2C modules.
 */

/*----------------------------------------------------------------------------*/
/*                                 Includes                                   */
/*----------------------------------------------------------------------------*/
#include "I2C.h"
/*----------------------------------------------------------------------------*/
/*                               Local defines                                */
/*----------------------------------------------------------------------------*/
#define CNT_VALUE_SEND_DATA                 (2u)
#define I2C2_SET_CNT_VALUE(x)               (I2C2CNT = x)
#define I2C2_GET_CNT_VALUE()                (I2C2CNT)
#define I2C2_SET_TARGER_ADR(adr, rw)        (I2C2ADB1 = ((adr << 1) | rw))
#define I2C2_WRITE_TXB(x)                   (I2C2TXB = x)
#define I2C_TXIF_POSITION                   (1u)
#define I2C_CLRBF_POSITION                  (2u)
#define I2C_TXBE_POSITION                   (5u)
#define I2C_RXBF_POSITION                   (0u)
#define I2C_MDR_POSITION                    (3u)
#define I2C_S_POSITION                      (5u)
#define I2C_ACKCNT_POSITION                 (7u)
#define I2C_ADRIE_POSITION                  (3u)
#define I2C_ACKTIE_POSITION                 (6u)
#define I2C_ACKDT_POSITION                  (6u)
#define I2C_SMA_POSITION                    (6u)
#define I2C_R_POSITION                      (4u)
#define I2C_D_POSITION                      (3u)
#define I2C_CSTR_POSTION                    (4u)
#define I2C2_GET_TXIF()                     (MASK_8BIT_GET_BIT(PIR7, I2C_TXIF_POSITION))
#define I2C2_IS_TXB_EMPTY()                 (MASK_8BIT_GET_BIT(I2C2STAT1, I2C_TXBE_POSITION))
#define I2C2_IS_RXB_FULL()                  (MASK_8BIT_GET_BIT(I2C2STAT1, I2C_RXBF_POSITION))
#define I2C2_IS_MASTER_PAUSED()             (MASK_8BIT_GET_BIT(I2C2CON0, I2C_MDR_POSITION))
#define I2C2_SET_START()                    (MASK_8BIT_SET_BIT(I2C2CON0, I2C_S_POSITION))
#define I2C2_READ_RXB()                     (I2C2RXB)
#define I2C_MASK_SLAVE_MODE                 (0xF8)
#define I2C2_SET_SLAVE_MODE()               (I2C2CON0 &= I2C_MASK_SLAVE_MODE);
#define I2C2_IS_SLAVE_ACTIVE()              (MASK_8BIT_GET_BIT(I2C2STAT0, I2C_SMA_POSITION))
#define I2C2_IS_READ_REQUEST()              (MASK_8BIT_GET_BIT(I2C2STAT0, I2C_R_POSITION))
#define I2C2_LAST_BYTE_IS_DATA()            (MASK_8BIT_GET_BIT(I2C2STAT0, I2C_D_POSITION))
#define I2C2_SET_ACKDT()                    (MASK_8BIT_CLEAR_BIT(I2C2CON1, I2C_ACKDT_POSITION))
#define I2C2_SET_NACKDT()                   (MASK_8BIT_SET_BIT(I2C2CON1, I2C_ACKDT_POSITION))
#define I2C2_RELEASE_CLOCK()                (MASK_8BIT_CLEAR_BIT(I2C2CON0, I2C_CSTR_POSTION))
#define I2C2_PORT_MASK                      (0x0C)
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
uint16_t I2C2_uiGetMatchedAdress(void);
/*----------------------------------------------------------------------------*/
/*                     Implementation of global functions                     */

/*----------------------------------------------------------------------------*/
void I2C_vInit(void)
{
    I2C2CON0 = 0x0C;
    I2C2CON2 = 0x20;
    I2C2CLK = 0x03;
    MASK_8BIT_SET_BIT(I2C1STAT1, I2C_CLRBF_POSITION);
    // de aici in jos is copy paste de pe proj vechi
    TRISC &= ~I2C2_PORT_MASK;
    ANSELC &= ~I2C2_PORT_MASK;
    WPUC |= I2C2_PORT_MASK;
    ODCONC |= I2C2_PORT_MASK;

    //    RC3I2C = 0x61; /* pULL uP, Slew Rate & Threshold */
    //    RC2I2C = 0x61; /* pULL uP, Slew Rate & Threshold */

    RC3PPS = 0x23; /* Selecting which module outputs on RC3 */
    RC2PPS = 0x24; /* Selecting which module outputs on RC2 */

    I2C2SDAPPS = 0x12; /* Feeding I2C1SDA from pin RC2 */
    I2C2SCLPPS = 0x13; /* Feeding I2C1SCL from pin RC3 */

}

void I2C_vMasterTransmit(uint8_t targetAdress, uint8_t targetRegister, uint8_t dataToBeSent)
{
    MASK_8BIT_SET_BIT(I2C2CON1, I2C_ACKCNT_POSITION);
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
    while (0u == I2C2_IS_TXB_EMPTY())
    {

    }
    I2C2_WRITE_TXB(dataToBeSent);
}

void I2C_vMasterRead(uint8_t targetAdress, uint8_t numberOfBytes, uint8_t * storingLocation)
{
    /* When the I2CxCNT = 0 the master will send NACK & STOP Condition */
    MASK_8BIT_SET_BIT(I2C2CON1, I2C_ACKCNT_POSITION);
    I2C2_vModuleEnable();

    /* When I2C2CNT == 0 master wil send NACK & Stop Bit*/
    I2C2_SET_CNT_VALUE(numberOfBytes);
    I2C2_SET_TARGER_ADR(targetAdress, I2C_OPERATION_READ);
    I2C2_SET_START();
    while (I2C2_GET_CNT_VALUE() != 0x00)
    {
        //        while (false == I2C2_bStopDetected())
        //        {
        /* Wait for the RXbuffer to receive the byte from SDA */
        while (0u == I2C2_IS_RXB_FULL())
        {

        }
        storingLocation[numberOfBytes - I2C2_GET_CNT_VALUE() - 1] = I2C2_READ_RXB();
        //        }
    }
}

void I2C_vJoinAsSlave(uint8_t adresssAsSlave)
{
    MASK_8BIT_SET_BIT(I2C2STAT1, I2C_CLRBF_POSITION);
    /* Setting Slave 7 bit address */
    I2C2_SET_SLAVE_MODE();
    /* I2C2ADR0<7:1> address bits */
    I2C2ADR0 = (adresssAsSlave << 1u);
    //    I2C1ADR1 = (adresssAsSlave << 1u); /* ADRx<7:1>:7-bit Slave Address => Address 9*/
    //    I2C1ADR2 = (adresssAsSlave << 1u); /* ADRx<7:1>:7-bit Slave Address => Address 9*/
    //    I2C1ADR3 = (adresssAsSlave << 1u); /* ADRx<7:1>:7-bit Slave Address => Address 9*/
    /* Setting ADRIE */
    MASK_8BIT_SET_BIT(I2C2PIE, I2C_ADRIE_POSITION);
    /* Clearing ACKTIE */
    MASK_8BIT_CLEAR_BIT(I2C2PIE, I2C_ACKTIE_POSITION);
    /* Setting ACKCNT */
    MASK_8BIT_SET_BIT(I2C2CON1, I2C_ACKCNT_POSITION);
    /* Clearing ACKDT */
    I2C2_SET_ACKDT();
    I2C2_vModuleEnable();
}

bool I2C_vSlaveMainFunction(uint8_t * receivedData, uint16_t * matchedAdress)
{
    bool returnValue = false;
    if (1u == I2C2_IS_SLAVE_ACTIVE())
    {
        I2C2_SET_CNT_VALUE(0xFF);
        /* if == 0 it means the last byte was an address*/
        if (0u == I2C2_LAST_BYTE_IS_DATA())
        {
            if (I2C2ADR0 == (I2C2ADB0 & 0xFE))
            {
                I2C2_SET_ACKDT();
                /* If master wants to read from slave put data in TXB*/
                if (1u == I2C2_IS_READ_REQUEST())
                {
                    while (0u == I2C2_IS_TXB_EMPTY())
                    {
                        I2C2_RELEASE_CLOCK();
                    }
                    I2C2_WRITE_TXB(0x8A);
                }
                    /* If master wants to write to slave read data from RXB */
                else if (0u == I2C2_IS_READ_REQUEST())
                {
                    while (0u == I2C2_IS_RXB_FULL())
                    {
                        I2C2_RELEASE_CLOCK();
                    }
                    returnValue = true;
                    *receivedData = I2C2RXB;
                    *matchedAdress = I2C2_uiGetMatchedAdress();
                }
            }
            else
            {
                I2C2_SET_NACKDT();
            }
            I2C2_RELEASE_CLOCK();
        }
            /* Last byte was data */
        else
        {
            /* If master wants to read from slave put data in TXB*/
            if (1u == I2C2_IS_READ_REQUEST())
            {
                if (1u == I2C2_IS_TXB_EMPTY())
                {
                    I2C2_WRITE_TXB(0x8A); /* Writing to this will let stop clock stretching */
                }
            }
                /* If master wants to write to slave read data from RXB */
            else if (0u == I2C2_IS_READ_REQUEST())
            {
                if (1u == I2C2_IS_RXB_FULL())
                {
                    returnValue = true;
                    *receivedData = I2C2RXB;
                    *matchedAdress = I2C2_uiGetMatchedAdress();
                }
            }
        }

    }

    return returnValue;
}

void I2C_vMasterTransmitBytes(uint8_t targetAdress, uint8_t * arrayWithData, uint8_t numberOfBytes)
{
    uint8_t index = 0;
    I2C2_vModuleEnable();
    I2C2_SET_CNT_VALUE(numberOfBytes);
    I2C2_SET_TARGER_ADR(targetAdress, I2C_OPERATION_WRITE);

    for (index = 0; index < numberOfBytes; index++)
    {
        while (0u == I2C2_IS_TXB_EMPTY())
        {

        }
        I2C2_WRITE_TXB(arrayWithData[index]);
        I2C2_SET_START();
    }
}

bool I2C_bOperationWasARead(void)
{
    return I2C2_IS_READ_REQUEST();
}
/*----------------------------------------------------------------------------*/
/*                     Implementation of local functions                      */

/*----------------------------------------------------------------------------*/
void I2C2_vWaitACK(void)
{
    //while()
}

bool I2C_bStopDetected(void)
{
    bool returnValue = false;
    if (1u == I2C2PIRbits.PCIF)
    {
        returnValue = true;
        I2C2PIRbits.PCIF = 0;
        //I2C2_vModuleDisable();
    }
    return returnValue;
}

void inline I2C2_vModuleEnable(void)
{
    while (I2C2CON0bits.EN != 1u)
    {
        I2C2CON0bits.EN = 1;
    }
}

void inline I2C2_vModuleDisable(void)
{
    I2C2CON0bits.EN = 0;
}

uint16_t I2C2_uiGetMatchedAdress()
{
    return (I2C2ADB0 >> 1);
}