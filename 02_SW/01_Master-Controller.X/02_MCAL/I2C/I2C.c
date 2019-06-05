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
#define I2C_MASTER_MODE_BITS_VALUE          (0x03)
/*----------------------------------------------------------------------------*/
/*                              Local data types                              */

/*----------------------------------------------------------------------------*/
typedef enum
{
    I2C_OPERATION_WRITE = 0u,
    I2C_OPERATION_READ,
} I2C_OperationType;

typedef struct
{
    uint8_t DataForResponse[8];
    uint8_t NumberOfBytesToBeSent;
    uint8_t StackIndex;
    uint8_t DataPendingValue;
    bool FirstRequestedByteSent;
} I2C_SlaveResponse;
/*----------------------------------------------------------------------------*/
/*                             Global data at RAM                             */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*                             Global data at ROM                             */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*                             Local data at RAM                              */
/*----------------------------------------------------------------------------*/
static const uint8_t I2C_DefaultResponsePendingValue = 0x55;
/*----------------------------------------------------------------------------*/
/*                             Local data at ROM                              */
/*----------------------------------------------------------------------------*/
static I2C_SlaveResponse I2C_SlaveResponseData;
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
    I2C2CLK = 0x06;
    MASK_8BIT_SET_BIT(I2C1STAT1, I2C_CLRBF_POSITION);
    // de aici in jos is copy paste de pe proj vechi
    TRISC &= ~I2C2_PORT_MASK;
    ANSELC &= ~I2C2_PORT_MASK;
    //WPUC |= I2C2_PORT_MASK;
    ODCONC |= I2C2_PORT_MASK;

    //    RC3I2C = 0x61; /* pULL uP, Slew Rate & Threshold */
    //    RC2I2C = 0x61; /* pULL uP, Slew Rate & Threshold */

    RC3PPS = 0x23; /* Selecting which module outputs on RC3 */
    RC2PPS = 0x24; /* Selecting which module outputs on RC2 */

    I2C2SDAPPS = 0x12; /* Feeding I2C1SDA from pin RC2 */
    I2C2SCLPPS = 0x13; /* Feeding I2C1SCL from pin RC3 */
    MASK_8BIT_SET_BIT(I2C2STAT1, I2C_CLRBF_POSITION);
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
    uint8_t index;
    MASK_8BIT_SET_BIT(I2C2STAT1, I2C_CLRBF_POSITION);
    /* Setting Slave 7 bit address */
    I2C2_SET_SLAVE_MODE();
    /* I2C2ADR0<7:1> address bits */
    I2C2ADR0 = (adresssAsSlave << 1u);
    //    I2C1ADR1 = (adresssAsSlave << 1u); /* ADRx<7:1>:7-bit Slave Address => Address 9*/
    //    I2C1ADR2 = (adresssAsSlave << 1u); /* ADRx<7:1>:7-bit Slave Address => Address 9*/
    //    I2C1ADR3 = (adresssAsSlave << 1u); /* ADRx<7:1>:7-bit Slave Address => Address 9*/
    /* Clearing ADRIE */
    MASK_8BIT_CLEAR_BIT(I2C2PIE, I2C_ADRIE_POSITION);
    /* Clearing ACKTIE */
    MASK_8BIT_CLEAR_BIT(I2C2PIE, I2C_ACKTIE_POSITION);
    /* Setting ACKCNT */
    MASK_8BIT_SET_BIT(I2C2CON1, I2C_ACKCNT_POSITION);
    /* Clearing ACKDT */
    I2C2_SET_ACKDT();

    I2C_SlaveResponseData.DataPendingValue = I2C_DefaultResponsePendingValue;
    I2C_SlaveResponseData.NumberOfBytesToBeSent = 0;
    I2C_SlaveResponseData.StackIndex = 0;
    I2C_SlaveResponseData.FirstRequestedByteSent = false;
    for (index = 0; index < 8; index++)
    {
        I2C_SlaveResponseData.DataForResponse[index] = I2C_DefaultResponsePendingValue + 1 + index;
    }
    I2C2_vModuleEnable();
}

I2C_SlaveOperationType I2C_vSlaveMainFunction(uint8_t * receivedData, uint16_t * matchedAdress)
{
    I2C_SlaveOperationType returnValue = I2C_NO_NEW_DATA;
    bool StackIncremented = false;
    if (I2C2PIRbits.SCIF == 1)
    {
        I2C_SlaveResponseData.StackIndex = 0;
        I2C2STAT1bits.CLRBF = 1;
    }
    if (I2C2STAT0bits.SMA == 1)
    {
        //        if (PIR7bits.I2C2IF == 1)
        //        {
        if (I2C2STAT0bits.R == 1)
        {
            if (I2C2STAT1bits.TXBE == 1)
            {

                if ((I2C_SlaveResponseData.NumberOfBytesToBeSent > 0) && (StackIncremented == false) && (I2C_SlaveResponseData.FirstRequestedByteSent == true) && (I2C_SlaveResponseData.StackIndex > 0))
                {
                    I2C2_WRITE_TXB(I2C_SlaveResponseData.DataForResponse[I2C_SlaveResponseData.StackIndex]);
                    I2C_SlaveResponseData.StackIndex++;
                    if (I2C_SlaveResponseData.StackIndex >= I2C_SlaveResponseData.NumberOfBytesToBeSent)
                    {
                        I2C_SlaveResponseData.StackIndex = 0;
                        I2C_SlaveResponseData.NumberOfBytesToBeSent = 0;
                        I2C_SlaveResponseData.FirstRequestedByteSent = false;
                    }
                    StackIncremented = true;
                    returnValue = I2C_REQUEST_SERVED;
                }
                else
                {
                    I2C2_WRITE_TXB(I2C_SlaveResponseData.DataPendingValue);
                    returnValue = I2C_DATA_REQUESTED;
                }
            }
            else
            {
                returnValue = I2C_NO_NEW_DATA;
            }
            if (I2C2PIRbits.ADRIF == 1)
            {
                I2C2STAT1bits.CLRBF = 1;
                I2C2PIRbits.ADRIF = 0;
                if (I2C2STAT1bits.TXBE == 1)
                {

                    if ((I2C_SlaveResponseData.NumberOfBytesToBeSent > 0) && (StackIncremented == false) && (I2C_SlaveResponseData.FirstRequestedByteSent == false) && (I2C_SlaveResponseData.StackIndex == 0))
                    {
                        I2C_SlaveResponseData.FirstRequestedByteSent = true;
                        I2C2_WRITE_TXB(I2C_SlaveResponseData.DataForResponse[I2C_SlaveResponseData.StackIndex]);
                        I2C_SlaveResponseData.StackIndex++;
                        if (I2C_SlaveResponseData.StackIndex >= I2C_SlaveResponseData.NumberOfBytesToBeSent)
                        {
                            I2C_SlaveResponseData.StackIndex = 0;
                            I2C_SlaveResponseData.NumberOfBytesToBeSent = 0;
                            I2C_SlaveResponseData.FirstRequestedByteSent = false;
                        }
                        StackIncremented = true;
                        returnValue = I2C_REQUEST_SERVED;
                    }

                    else
                    {
                        I2C2_WRITE_TXB(I2C_SlaveResponseData.DataPendingValue);
                        returnValue = I2C_DATA_REQUESTED;
                    }
                }
                else
                {
                    returnValue = I2C_NO_NEW_DATA;
                }
                I2C2_RELEASE_CLOCK();
            }
        }
        else
        {
            // WRITE
        }

        if (I2C2PIRbits.PCIF == 1)
        {
            I2C2PIRbits.PCIF = 0;
            I2C2STAT1bits.CLRBF = 1;
        }
        else if (I2C2PIRbits.ADRIF == 1)
        {
            I2C2PIRbits.ADRIF = 0;
        }

        I2C2_RELEASE_CLOCK();
        //        }
    }
    if (I2C2PIRbits.SCIF == 1)
    {
        I2C2_SET_CNT_VALUE(0xff);
        I2C2CON1bits.ACKCNT = 0;
        I2C2PIRbits.SCIF = 0;
    }


    *matchedAdress = I2C2_uiGetMatchedAdress();

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
            I2C2_SET_START();
        }
        I2C2_WRITE_TXB(arrayWithData[index]);
        I2C2_SET_START();
    }
}

bool I2C_bOperationWasARead(void)
{
    return I2C2_IS_READ_REQUEST();
}

void I2C_vSlaveSetResponse(uint8_t * DataSource_ptr, uint8_t NumberOfBytes)
{
    uint8_t index;
    if (0 == I2C_SlaveResponseData.NumberOfBytesToBeSent)
    {
        for (index = 0; index < NumberOfBytes; index++)
        {
            I2C_SlaveResponseData.DataForResponse[index] = DataSource_ptr[index];
        }
        I2C_SlaveResponseData.NumberOfBytesToBeSent = NumberOfBytes;
        I2C_SlaveResponseData.StackIndex = 0;
    }
}

bool I2C_bIsMasterModeActive(void)
{
    return (I2C2CON0 > I2C_MASTER_MODE_BITS_VALUE);
}

void I2C_vSetCLK(uint8_t clkID)
{
    Timer2_vInit(clkID);
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
        I2C_SlaveResponseData.StackIndex = 0;
        I2C_SlaveResponseData.NumberOfBytesToBeSent = 0;
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