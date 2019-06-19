/**
 * \file       LIN.c
 * \author     Ioan Nicoara 
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
#define BREAK_STATUS                MASK_8BIT_GET_BIT(U2ERRIR, 2u)
#define NO_OF_PIDS                  (10u)
#define RECEIVE_FIFO_SIZE           (32u)
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
bool DataCanBeSent = true;
volatile uint8_t rcv[12] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
volatile uint8_t ircv = 0u;
volatile uint8_t noOfBytes = 2u;
uint8_t DataBytesToReceive = 0xFF;
uint8_t PID_wasFound = false;

LIN_packet LIN_ReceiveFIFO[32u];
uint8_t LIN_FIFOStackPointer = 0u;
uint8_t LIN_FIFOReadPointer = 0u;

volatile LIN_packet LIN_FramePacket;
PID_Description PID_noOfBytes[] = {
    {49, 1},
    {0x05, 2}
};

PID_Description * Actual_PID;
uint8_t PID_noOfBytesIndex = 2u;
/*----------------------------------------------------------------------------*/
/*                             Local data at ROM                              */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*                       Declaration of local functions                       */
/*----------------------------------------------------------------------------*/
/**
 * \brief     This function is used for sets the Checksum Mode Select bit 
 * \param     CkSUM - represents Checksum Mode: LEGACY - not include PID in Checksum
 *                                              ENHANCED - include PID in CheckSum
 * \return    None 
 */
void LIN_vCheckSUMMode(CheckSUM_Mode CkSUM);

/**
 * \brief     This function checks if the LIN transmitter is ready to transmit data
 * \param     None
 * \return    Status of LIN transmitter
                TRUE: LIN transmitter is ready
                FALSE: LIN transmitter is not ready
 */
bool LIN_vTransmitReady();

/**
 * \brief     This function checks if the LIN receiver is ready for reading
 * \param     None
 * \return    Status of LIN receiver
                TRUE: LIN receiver is ready for reading
                FALSE: LIN receiver is not ready for reading
 */
bool LIN_vReceiveReady();

/**
 * \brief     This function is used for receive values on UART Asynchronous Receiver
 * \param    
 * \return    returns the value received in the U2RXB register 
 */
uint8_t LIN_uiReceive();
/*----------------------------------------------------------------------------*/
/*                     Implementation of global functions                     */
/*----------------------------------------------------------------------------*/
void LIN_vInit(LIN_Configuration * Config)
{
    UART2_vInit();
    RB2PPS = 0x16; //  UART1 => set TX on PORTB pin 2
    U2RXPPS = 0x09; //  UART1 => set RX on PORTB pin 1 
    GPIO_vSetPinDirection(0xB2, GPIO_OUTPUT_PIN); // PIN B2 set as output
    GPIO_vSetPinDirection(0xB1, GPIO_INPUT_PIN); // PIN B1 set as input
    MASK_8BIT_CLEAR_BIT(U2CON1, UART2_ENABLE); //UART2 Disabled
    MASK_8BIT_SET_BIT(U2CON0, UART2_TX_EN); //Enable TX on UART2
    MASK_8BIT_SET_BIT(U2CON0, UART2_RX_EN); //Enable RX on UART2
    UART2_vUARTMode(Config->configUartMode); //set UART2 Mode as LIN MASTER
    UART2_vBaudCalculator(Config->configBaudGeneratorSpeed, Config->configBaudValue);
    UART2_vTransmitPolarityControl(Config->configTransmitPolarity);
    UART2_vStopBitMode(Config->configStopBitMode);
    U2CON2 |= 0x80;
    LIN_vCheckSUMMode(Config->config_ChecksumMode);
    U2FIFO |= 0x20;
    PIE7 |= 0x10;
    MASK_8BIT_SET_BIT(U2CON1, UART2_ENABLE); //UART2 ENABLED
    LIN_FramePacket.noOfBytes = 0u;
}

void SetDataForResponse(uint8_t pid, uint8_t *data, uint8_t noOfDataBytes)
{
    uint8_t index;
    uint8_t bytePosition;

    for (index = 0u; index < NO_OF_PIDS; index++) //index represents pid
    {
        if (pid == PID_noOfBytes[index].pid)
        {
            for (bytePosition = 0u; bytePosition < noOfDataBytes; bytePosition++)
            {
                PID_noOfBytes[index].dataForResponse[bytePosition] = data[bytePosition];
            }
        }
        else
        {
            
        }
    }
}


void LIN_vAddNewPID(uint8_t pid, uint8_t noOfDataBytes)
{
    PID_noOfBytes[PID_noOfBytesIndex].pid = pid;
    PID_noOfBytes[PID_noOfBytesIndex].noOfBytes = noOfDataBytes;
    PID_noOfBytesIndex++;
}


void LIN_vTransmit(uint8_t identifier, uint8_t NoOfBytes, uint8_t *data)
{
    if (true == DataCanBeSent)
    {
        uint8_t byte_count = 0u;
        U2P2L = RESET_VALUE;
        if ((STD_LOW == U2P2L) && (STD_LOW == TX2_INTERRUPT_FLAG))
        {
            U2P2L = NoOfBytes;
            //U2P3L = NoOfBytes + 1;
            if (LIN_MASTER == MASK_8BIT_GET_LSB_HALF(U2CON0))
            {
                U2P1L = identifier;
                __delay_us(500u);
            }
            if (STD_HIGH == TX2_SHIFTREG_EMPTY)
            {
                for (byte_count = 0u; byte_count < NoOfBytes; byte_count++)
                {
                    UART2_vTransmitter(data[byte_count]);
                }
            }
            else
            {
                
            }
        }
    }
    U2ERRIR &= 0x10u;
}


LIN_packet * LIN_GetPacket()
{
    LIN_packet * returnValue = &LIN_ReceiveFIFO[LIN_FIFOReadPointer];
    if (LIN_FIFOReadPointer < (LIN_FIFOStackPointer - 1u))
    {
        LIN_FIFOReadPointer++;
        if (LIN_FIFOReadPointer >= RECEIVE_FIFO_SIZE)
        {
            LIN_FIFOReadPointer = 0u;
        }
        else
        {
            
        }
    }
    else
    {
        
    }
    return returnValue;
}


/*----------------------------------------------------------------------------*/
/*                     Implementation of local functions                      */

/*----------------------------------------------------------------------------*/

void LIN_vCheckSUMMode(CheckSUM_Mode CkSUM)
{
    if (CkSUM < LEGACY || CkSUM > ENHANCED)
    {
        MASK_8BIT_CLEAR_BIT(U2CON2, CHECKSUM_BIT); // sets CHECKSUM Mode as LEGACY (if CkSUM parameter is not in interval)[DEFAULT]
    }
    else if (CkSUM == LEGACY)
    {
        MASK_8BIT_CLEAR_BIT(U2CON2, CHECKSUM_BIT); // sets CHECKSUM Mode as LEGACY
    }
    else if (CkSUM == ENHANCED)
    {
        MASK_8BIT_SET_BIT(U2CON2, CHECKSUM_BIT); // sets CHECKSUM Mode as ENHANCED
    }
    else
    {

    }
}

bool LIN_vTransmitReady()
{
    return (bool) UART2_bTX_Ready();
}

bool LIN_vReceiveReady()
{
    return (bool) UART2_bRX_Ready();
}



uint8_t LIN_uiReceive()
{
    uint8_t rec;
    UART2_uiReception(&rec);
    return rec;
}

void __interrupt(irq(60)) LIN_ReceiveInterrupt(void)
{
    uint8_t index;
    rcv[ircv] = LIN_uiReceive();
    if (false == PID_wasFound)
    {
        for (index = 0u; index < PID_noOfBytesIndex; index++)
        {
            if ((rcv[ircv] & PID_MASK) == PID_noOfBytes[index].pid)
            {
                DataBytesToReceive = PID_noOfBytes[index].noOfBytes + 1u;
                LIN_ReceiveFIFO[LIN_FIFOStackPointer].pid = rcv[ircv] & PID_MASK;
                DataBytesToReceive--;
                U2P3L = PID_noOfBytes[index].noOfBytes;
                PID_wasFound = true;
                Actual_PID = &PID_noOfBytes[index];
            }
        }
    }
    else if (true == PID_wasFound)
    {
        if (Actual_PID->type == Subscriber)
        {
            LIN_vTransmit(Actual_PID->pid, Actual_PID->noOfBytes, Actual_PID->dataForResponse);
        }
        else
        {
            if (0u == DataBytesToReceive)
            {
                LIN_ReceiveFIFO[LIN_FIFOStackPointer].checksum = rcv[LIN_ReceiveFIFO[LIN_FIFOStackPointer].noOfBytes + 1u];
                LIN_FIFOStackPointer++;
                if (LIN_FIFOStackPointer >= RECEIVE_FIFO_SIZE)
                {
                    LIN_FIFOStackPointer = 0u;
                }
                DataBytesToReceive = PID_noOfBytes[index].noOfBytes;
                ircv = 255u;
                PID_wasFound = false;
            }
            else
            {
                LIN_ReceiveFIFO[LIN_FIFOStackPointer].data[LIN_ReceiveFIFO[LIN_FIFOStackPointer].noOfBytes] = rcv[LIN_ReceiveFIFO[LIN_FIFOStackPointer].noOfBytes + 1u];
                LIN_ReceiveFIFO[LIN_FIFOStackPointer].noOfBytes++;
                DataBytesToReceive--;
            }
        }

    }
    else
    {
        
    }
    if (ircv >= noOfBytes)
    {
        DataCanBeSent = true;
        //U2ERRIR &= 0x10;
    }
    ircv++;
    U2ERRIR &= 0x10;
}

