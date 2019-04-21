/**
 * \file       CAN.c
 * \author     Iosub Mihai Alexandru
 * \brief      Short description for this source file
 */

/*----------------------------------------------------------------------------*/
/*                                 Includes                                   */
/*----------------------------------------------------------------------------*/
#include "CAN.h"
/*----------------------------------------------------------------------------*/
/*                               Local defines                                */

/*----------------------------------------------------------------------------*/
#define CAN_REQOP_POSTION                       (5u)
#define CAN_ERROR_RECOGNITION_MODE_VALUE        (0x60)
#define CAN_OPMODE_POSTION                      (5u) 
#define CAN_MDSEL_POSITION                      (6u)
#define CAN_TXREQ_POSITION                      (3u)
#define CAN_SID0_POSITION                       (5u)
#define CAN_SID3_POSITION                       (3u)
#define CAN_EIDH_EXTENDED_POSITION              (8u)
#define CAN_SIDL_EXTENDED_POSITION              (17u)
#define CAN_SIDL_EXTENDED_POSITION2             (14u)
#define CAN_SIDH_EXTENDED_POSITION              (21u)
#define CAN_EXIDE_POSITION                      (3u)
#define CAN_BxDLC_RTR_POSITION                  (6u)
#define CAN_BxCON_RTR_POSITION                  (5u)
#define CAN_FIFO_EMPTY_POSITION                 (7u)
#define CAN_RXFUL_POSITION                      (7u)

#define CAN_EXTENDED_SIDL_3MSB_BITS             (3u)
#define CAN_EXTENDED_SIDL_2MSB_BITS             (2u)

#define CAN_SIDL_EXTENDED_MASK1                 (0x03)
#define CAN_SIDL_EXTENDED_MASK2                 (0xE0)
#define CAN_REQOP_CLEAR_MASK                    (0x1F)
#define CAN_MASK_GET_FIFO_POINTER               (0x0F)               

#define CAN_TRANSMITTER_BUS_MODE_MASK           (0x34)
#define CAN_TRANSMITTER_BUS_ACTIVE_VALUE        (0x00)
#define CAN_TRANSMITTER_BUS_WARNING_VALUE       (0x04)
#define CAN_TRANSMITTER_BUS_PASSIVE_VALUE       (0x14)
#define CAN_TRANSMITTER_BUS_OFF_VALUE           (0x34)

#define CAN_RECEIVER_BUS_MODE_MASK           (0x2A)
#define CAN_RECEIVER_BUS_ACTIVE_VALUE        (0x00)
#define CAN_RECEIVER_BUS_WARNING_VALUE       (0x02)
#define CAN_RECEIVER_BUS_PASSIVE_VALUE       (0x08)

#define CAN_MAX_PROGRAMMABLE_BUFFERS         (6u)

#define CAN_PROGRAMMABLE_BUFFER_FLAG_VALUE   (0x10)
#define CAN_TXPRI_MASK                       (0x03)
#define CAN_GET_SIDL_VALUE_STANDARD(x)       (x << CAN_SID0_POSITION)
#define CAN_GET_SIDH_VALUE_STANDARD(x)       (x >> CAN_SID3_POSITION)
#define CAN_GET_EIDH_VALUE_EXTENDED(x)       (x >> CAN_EIDH_EXTENDED_POSITION)
#define CAN_GET_SIDL_VALUE_EXTENDED(x)       (x >> CAN_SIDL_EXTENDED_POSITION)
#define CAN_GET_SIDH_VALUE_EXTENDED(x)       (x >> CAN_SIDH_EXTENDED_POSITION)

#define CAN_TRANSCEIVER_NSIL_PORT_PIN        (0xA3)
#define CAN_TRANSCEIVER_STBY_PORT_PIN        (0xA4)
#define CAN_TRANSCEIVER_TX_PORT_PIN          (0xB4)
/*----------------------------------------------------------------------------*/
/*                              Local data types                              */

/*----------------------------------------------------------------------------*/
typedef enum
{
    CAN_ERROR_ACTIVE_MODE = 0u,
    CAN_ERROR_PASIVE_MODE,
    CAN_BUSS_OFF_MODE,
} CAN_ErrorMode;

/*----------------------------------------------------------------------------*/
/*                             Global data at RAM                             */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*                             Global data at ROM                             */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*                             Local data at RAM                              */
/*----------------------------------------------------------------------------*/
static CAN_Configuration CAN_ModuleConfiguration;
static CAN_Buffer * CAN_ReceiveBuffers[CAN_NUMBER_OF_RXBF];
static CAN_Buffer * CAN_TransmitBuffers[CAN_NUMBER_OF_TXBF];
static CAN_Buffer * CAN_ProgrammableBuffers[CAN_NUMBER_OF_PROGRAMMABLE_BUFFERS];
static uint8_t CAN_NumberOfReceiveBuffers;
/*----------------------------------------------------------------------------*/
/*                             Local data at ROM                              */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*                       Declaration of local functions                       */
/*----------------------------------------------------------------------------*/
/**
 * \brief     This function returns the operation mode;
 * \param     None
 * \return    None 
 */
CAN_OperationMode CAN_uiGetOperationMode(void);
/**
 * \brief     This function requests an operation mode and waits for it to be set;
 * \param     None
 * \return    None 
 */
void CAN_vRequestOperationMode(CAN_OperationMode mode);
/**
 * \brief     This function sets the functional mode of the CAN module [0-2]
 * \param     None
 * \return    None 
 */
void CAN_vSetFunctionalMode(CAN_FunctionalMode mode);
/**
 * \brief     This function returns one of the three error states; ?error-active?, ?error-passive? or 
 * ?bus-off?, according to the value of the internal error counters.
 * \param     None
 * \return    CAN_ErrorMode; 
 */
CAN_ErrorMode CAN_uiGetTransmitterErrorMode(void);

/**
 * \brief     This function returns one of the three error states; ?error-active?, ?error-passive? or 
 * ?bus-off?, according to the value of the internal error counters.
 * \param     None
 * \return    CAN_ErrorMode; 
 */
CAN_ErrorMode CAN_uiGetReceiverErrorMode(void);

/**
 * \brief     This function sets the identifier registers according to the standard / extended mode
 * \param     None
 * \return    CAN_ErrorMode; 
 */
void CAN_vSetIdentifier(CAN_Frame * frame, CAN_Buffer * buffer);

/**
 * \brief     This function sets the number of Receive buffers to number_of_rx_buffers;
 *                               the number of Transmit buffers will be 6-number_of_rx_buffers + 3 default buffers;
 *              Be advised, to the number of buffers selected are added 2 more buffers that are available by default: RXB0 and RXB1
 * \param     None
 * \return    None; 
 */
void CAN_vSetRxProgrammableBuffers(uint8_t number_of_rx_buffers);

/**
 * \brief     This function says if a buffer is available for transfer
 * \param     None
 * \return    None; 
 */
uint8_t CAN_vFindBufferReadyForTransfer(void);
/**
 * \brief     This function sets the baud rate on the CAN BUS
 * \param     baudRate - desired baud
 * \return    None; 
 */
void CAN_vSetBaudRate(CAN_BaudRate * baudRate);
/**
 * \brief     This function requests a transmission on CAN BUS
 * \param     None
 * \return    None; 
 */
void CAN_vRequestTransmissionFromBuffer(CAN_Buffer * target);
/**
 * \brief     This function creates the necessary conditions for the CAN transceiver to aquire Normal Mode
 * \param     None
 * \return    None; 
 */
void CAN_vRequestTransceiverNormalMode(void);

/*----------------------------------------------------------------------------*/
/*                     Implementation of global functions                     */

/*----------------------------------------------------------------------------*/
void CAN_vInit(CAN_Configuration * config)
{
    CAN_vRequestTransceiverNormalMode();
    /* Preparing the TX, RX and programmable buffers
     * 
     * Cast to (CAN_Buffer *) is necessary in order to remove warning for incompatible conversion types*/
    CAN_ReceiveBuffers[0] = (CAN_Buffer *) & RXB0CONbits;
    CAN_ReceiveBuffers[1] = (CAN_Buffer *) & RXB1CONbits;
    CAN_TransmitBuffers[0] = (CAN_Buffer *) & TXB0CONbits;
    CAN_TransmitBuffers[1] = (CAN_Buffer *) & TXB1CONbits;
    CAN_TransmitBuffers[2] = (CAN_Buffer *) & TXB2CONbits;
    CAN_ProgrammableBuffers[0] = (CAN_Buffer *) & B0CONbits;
    CAN_ProgrammableBuffers[1] = (CAN_Buffer *) & B1CONbits;
    CAN_ProgrammableBuffers[2] = (CAN_Buffer *) & B2CONbits;
    CAN_ProgrammableBuffers[3] = (CAN_Buffer *) & B3CONbits;
    CAN_ProgrammableBuffers[4] = (CAN_Buffer *) & B4CONbits;
    CAN_ProgrammableBuffers[5] = (CAN_Buffer *) & B5CONbits;
    /* Configuring the pins as I/O*/



    /* B = 1101 => 1 = PORTB; 101 = PIN 5  */
    CANRXPPS = 0x0C;
    GPIO_vSetPinDirection(0xB4, GPIO_INPUT_PIN);
    /* Route CAN0 to RB4 & set as OUTPUT */
    /* 0x34 = CANTX1; 0x33 = CANTX0; */
    RB5PPS = 0x33;
    GPIO_vSetPinDirection(0xB5, GPIO_OUTPUT_PIN);

    CAN_vRequestOperationMode(CAN_CONFIGURATION_OPERATION_MODE);
    /* Poplaca style, should be removed */
    /**    
        Initialize Receive Masks
        If any mask bit is set to a zero, then that bit will automatically be accepted regardless of the filter bit
     */
    RXM0EIDH = 0x00;
    RXM0EIDL = 0x00;
    RXM0SIDH = 0x00;
    RXM0SIDL = 0x00;

    RXM1EIDH = 0x00;
    RXM1EIDL = 0x00;
    RXM1SIDH = 0x00;
    RXM1SIDL = 0x00;
    /* End of Poplaca */

    /* Configuring buffers based on Module_ReceiveFIFO_Size */
    CAN_vSetRxProgrammableBuffers(config->Module_ReceiveFIFO_Size);
    CAN_vSetFunctionalMode(config->Module_FunctionalMode);
    CAN_vSetBaudRate(&(config->Module_BaudRate));
    /* Filters & Masks*/
    CAN_vRequestOperationMode(config->Module_OperationMode);
}

void CAN_vSetBaudRate(CAN_BaudRate * baudRate)
{
    /* 
     * Setting up the baud rate register:
     * Clock taken from System Clock (16MHz)
     * Baud rate: 50kb/s
     * Sampling Point: 80%
     * 
     * BRGCON1 register: 
     * BRGCON1[7:6] - SJW<1:0>: Synchronized Jump Width bits 00 = Synchronization jump width time = 1 x TQ 
     * BRGCON1[5:0] - BRP<5:0>: Baud Rate Prescaler bits = 00_1111 = TQ = (2 x 15+1)/16
     */
    /*  BRGCON2 register:
     * bit 7 SEG2PHTS: Phase Segment 2 Time Select bit: 1 = Freely programmable
     * bit 6 SAM: Sample of the CAN bus Line bit: 0 = Bus line is sampled once at the sample point
     * bit 5-3 SEG1PH<2:0>: Phase Segment 1 bits: 101 = Phase Segment 1 time = 5 x TQ
     * bit 2-0 PRSEG<2:0>: Propagation Time Select bits: 000 = Propagation time = 1 x TQ
     */
    /* BRGCON3 register:
     * bit 7 WAKDIS: Wake-up Disable bit: 0 = Enable CAN bus activity wake-up feature
     * bit 6 WAKFIL: Selects CAN bus Line Filter for Wake-up bit: 0 = CAN bus line filter is not used for wake-up
     * bit 5-3 Unimplemented: Read as ?0?
     * bit 2-0 SEG2PH<2:0>: Phase Segment 2 Time Select bits: 001 = Phase Segment 2 time = 2 x TQ
     */

    CIOCON = 0x80;
    switch (*baudRate)
    {
        case CAN_20KBITS:
            BRGCON1 = 0x27;
            BRGCON2 = 0xA8;
            BRGCON3 = 0x01;
            break;
        case CAN_50KBITS:
            BRGCON1 = 0x0F;
            BRGCON2 = 0xA8;
            BRGCON3 = 0x01;
            break;
        case CAN_80KBITS:
            BRGCON1 = 0x09;
            BRGCON2 = 0xA8;
            BRGCON3 = 0x01;
            break;
        case CAN_100KBITS:
            BRGCON1 = 0x07;
            BRGCON2 = 0xA8;
            BRGCON3 = 0x01;
            break;
        case CAN_125KBITS:
            BRGCON1 = 0x07;
            BRGCON2 = 0x98;
            BRGCON3 = 0x01;
            break;
        case CAN_200KBITS:
            BRGCON1 = 0x04;
            BRGCON2 = 0x98;
            BRGCON3 = 0x01;
            break;
        case CAN_250KBITS:
            BRGCON1 = 0x03;
            BRGCON2 = 0x98;
            BRGCON3 = 0x01;
            break;
        case CAN_500KBITS:
            BRGCON1 = 0x01;
            BRGCON2 = 0x98;
            BRGCON3 = 0x01;
            break;
        case CAN_800KBITS:
            BRGCON1 = 0x00;
            BRGCON2 = 0xA8;
            BRGCON3 = 0x01;
            break;
        case CAN_1MBIT:
            BRGCON1 = 0x00;
            BRGCON2 = 0x98;
            BRGCON3 = 0x01;
            break;
    }

}

void CAN_vTransmitFrame(CAN_Frame frame)
{
    CAN_Buffer * selectedBuffer;
    uint8_t targetBuffer = CAN_vFindBufferReadyForTransfer();
    uint8_t index = 0;
    /* Selecting the buffer */
    if (0x00 == MASK_8BIT_GET_MSB_HALF(targetBuffer))
    {
        /* Add code for CAN_TRansmitBuffer[target]*/
        selectedBuffer = CAN_TransmitBuffers[MASK_8BIT_GET_LSB_HALF(targetBuffer)];
    }
    else if (CAN_PROGRAMMABLE_BUFFER_FLAG_VALUE == MASK_8BIT_GET_MSB_HALF(targetBuffer))
    {
        /* Add code for CAN_ProgrammableBuffer[target]*/
        selectedBuffer = CAN_ProgrammableBuffers[MASK_8BIT_GET_LSB_HALF(targetBuffer)];
    }
    /* Writing to the buffer according to the frame */
    /* Setting the priority of the buffer */
    selectedBuffer->BxCON |= (CAN_TXPRI_MASK & frame.Frame_Priority);
    /* Setting the identifier */
    CAN_vSetIdentifier(&frame, selectedBuffer);
    /* Setting the number of bytes to sent */
    selectedBuffer->BxDLC = (frame.Frame_DataLength & 0x0F);
    /* Setting RTR */
    if (true == frame.Frame_RTR)
    {
        MASK_8BIT_SET_BIT(selectedBuffer->BxDLC, CAN_BxDLC_RTR_POSITION);
    }
    else
    {
        MASK_8BIT_CLEAR_BIT(selectedBuffer->BxDLC, CAN_BxDLC_RTR_POSITION);
    }
    /* Putting data onto the buffer */
    for (index = 0; index < frame.Frame_DataLength; index++)
    {

        selectedBuffer->BxData[index] = frame.Frame_DataBytes[index];
    }
    /* Requesting the transmission */
    CAN_vRequestTransmissionFromBuffer(selectedBuffer);
}

uint8_t CAN_uiGetFIFOReadPointer()
{
    /*uint8_t bufferIndex;
    uint8_t returnValue = CAN_NO_DATA_RECEIVED_BUFFER;
    for (bufferIndex = 0; bufferIndex < CAN_NumberOfReceiveBuffers; bufferIndex++)
    {
        if (bufferIndex < CAN_NUMBER_OF_RXBF)
        {
            if ((MASK_8BIT_GET_BIT(CAN_TransmitBuffers[bufferIndex]->BxCON, CAN_RXFUL_POSITION)) == 1)
            {
                returnValue = bufferIndex;
                break;
            }
        }
        else
        {
            if ((MASK_8BIT_GET_BIT(CAN_ProgrammableBuffers[(bufferIndex - CAN_NUMBER_OF_RXBF)]->BxCON, CAN_RXFUL_POSITION)) == 1)
            {
                returnValue = bufferIndex;
                break;
            }
        }
    }
    return returnValue;*/
    return (CANCON & CAN_MASK_GET_FIFO_POINTER);
}

uint32_t CAN_uiGetRxBufferIdentifier(uint8_t * bufferID)
{
    uint32_t returnValue;
    if (*bufferID < CAN_NUMBER_OF_RXBF)
    {
        returnValue = CAN_uiGetIdentifier(CAN_ReceiveBuffers[*bufferID]);
    }
    else
    {
        /* RX Buffers are the first of the programmable buffers, after them the TX buffers comes */
        returnValue = CAN_uiGetIdentifier(CAN_ProgrammableBuffers[(*bufferID) - CAN_NUMBER_OF_RXBF]);
    }
    return returnValue;
}

CAN_Buffer * CAN_uiGetBufferAdrress(uint8_t * bufferID)
{
    CAN_Buffer * returnValue;
    if (*bufferID < CAN_NUMBER_OF_RXBF)
    {
        returnValue = CAN_ReceiveBuffers[*bufferID];
    }
    else
    {
        /* RX Buffers are the first of the programmable buffers, after them the TX buffers comes */
        returnValue = CAN_ProgrammableBuffers[(*bufferID) - CAN_NUMBER_OF_RXBF];
    }
    return returnValue;
}

uint32_t CAN_uiGetIdentifier(CAN_Buffer * buffer)
{
    uint32_t returnValue = 0;
    if (CAN_ModuleConfiguration.Module_FrameType == CAN_EXTENDED_FRAME)
    {
        /* Adding the most significant 8 bits */
        returnValue |= buffer->BxSIDH;
        returnValue = returnValue << CAN_EXTENDED_SIDL_3MSB_BITS;
        /* Adding bits 20-18 of the identifier */
        returnValue |= (buffer->BxSIDL >> CAN_SID0_POSITION);
        returnValue = returnValue << CAN_EXTENDED_SIDL_2MSB_BITS;
        /* Adding bits 17-16 of the identifier */
        returnValue |= (buffer->BxSIDL & CAN_SIDL_EXTENDED_MASK1);
        returnValue = returnValue << REGISTER_NUMBER_OF_BITS;
        /* Adding next 8 bits */
        returnValue |= buffer->BxEIDH;
        returnValue = returnValue << REGISTER_NUMBER_OF_BITS;
        /* Adding last 8 bits */
        returnValue |= buffer->BxEIDL;
    }
    else if (CAN_ModuleConfiguration.Module_FrameType == CAN_STANDARD_FRAME)
    {

        /* Adding the most significant 8 bits */
        returnValue |= buffer->BxSIDH;
        returnValue = returnValue << CAN_EXTENDED_SIDL_3MSB_BITS;
        /* Adding bits 2-0 of the identifier */
        returnValue |= (buffer->BxSIDL >> CAN_SID0_POSITION);
    }
    return returnValue;
}

void CAN_vSetBufferAsFree(CAN_Buffer * target)
{
    MASK_8BIT_CLEAR_BIT(target->BxCON, CAN_RXFUL_POSITION);
}

bool CAN_bBufferHasNewData(CAN_Buffer * target)
{
    bool returnValue = false;
    returnValue = MASK_8BIT_GET_BIT(target->BxCON, CAN_RXFUL_POSITION);
    return returnValue;
}

bool CAN_bBufferRequestsData(CAN_Buffer * target)
{
    bool returnValue = false;
    returnValue = MASK_8BIT_GET_BIT(target->BxDLC, CAN_BxDLC_RTR_POSITION);
    return returnValue;
}

uint8_t CAN_uiGetNumberOfReceiveBuffers(void)
{
    return (CAN_NUMBER_OF_RXBF + CAN_NumberOfReceiveBuffers);
}
/*----------------------------------------------------------------------------*/
/*                     Implementation of local functions                      */

/*----------------------------------------------------------------------------*/
void CAN_vRequestTransmissionFromBuffer(CAN_Buffer * target)
{

    MASK_8BIT_SET_BIT(target->BxCON, CAN_TXREQ_POSITION);
}

void CAN_vSetRxProgrammableBuffers(uint8_t number_of_rx_buffers)
{
    /* !(((2^number) - 1) << 2) Shift with 2 because BSEL0[0:1] are read only*/
    uint8_t BSEL_value;
    if (CAN_MAX_PROGRAMMABLE_BUFFERS < number_of_rx_buffers)
    {
        CAN_NumberOfReceiveBuffers = CAN_MAX_PROGRAMMABLE_BUFFERS;
    }
    else
    {
        CAN_NumberOfReceiveBuffers = number_of_rx_buffers;
    }
    BSEL_value = (!(((1 << CAN_NumberOfReceiveBuffers) - 1) << 2u));

    BSEL0 = BSEL_value;
}

CAN_OperationMode CAN_uiGetOperationMode()
{

    return (CANSTAT >> CAN_OPMODE_POSTION);
}

void CAN_vRequestOperationMode(CAN_OperationMode mode)
{
    if (CAN_ERROR_RECOGNITION_OPERATION_MODE == mode)
    {
        RXB0CON |= CAN_ERROR_RECOGNITION_MODE_VALUE;
    }
    else
    {
        CANCON = (mode << CAN_REQOP_POSTION);
    }
    /*  When changing modes, the mode will not actually
        change until all pending message transmissions are
        complete. Because of this, the user must verify that the
        device has actually changed into the requested mode
        before further operations are executed.
        Wait for the module to aquire target mode */
    while (CAN_uiGetOperationMode() != mode)
    {

    }
}

void CAN_vSetFunctionalMode(CAN_FunctionalMode mode)
{
    uint8_t ecancon_desiredValue = (mode << CAN_MDSEL_POSITION);
    do
    {
        ECANCON = ecancon_desiredValue;
    }
    while (ECANCON != ecancon_desiredValue);
}

CAN_ErrorMode CAN_uiGetTransmitterErrorMode()
{
    CAN_ErrorMode returnValue = CAN_ERROR_ACTIVE_MODE;
    uint8_t errorFlags = (COMSTAT & CAN_TRANSMITTER_BUS_MODE_MASK);
    switch (errorFlags)
    {
            /* This to cases does the same thing, that's why the first one does not have a break and body */
        case CAN_TRANSMITTER_BUS_WARNING_VALUE:
        case CAN_TRANSMITTER_BUS_ACTIVE_VALUE:
            returnValue = CAN_ERROR_ACTIVE_MODE;
            break;
        case CAN_TRANSMITTER_BUS_PASSIVE_VALUE:
            returnValue = CAN_ERROR_PASIVE_MODE;
            break;
        case CAN_TRANSMITTER_BUS_OFF_VALUE:
            returnValue = CAN_BUSS_OFF_MODE;

            break;
    }
    return returnValue;
}

CAN_ErrorMode CAN_uiGetReceiverErrorMode()
{
    CAN_ErrorMode returnValue = CAN_ERROR_ACTIVE_MODE;
    uint8_t errorFlags = (COMSTAT & CAN_RECEIVER_BUS_MODE_MASK);
    switch (errorFlags)
    {
            /* This to cases does the same thing, that's why the first one does not have a break and body */
        case CAN_RECEIVER_BUS_WARNING_VALUE:
        case CAN_RECEIVER_BUS_ACTIVE_VALUE:
            returnValue = CAN_ERROR_ACTIVE_MODE;
            break;
        case CAN_RECEIVER_BUS_PASSIVE_VALUE:
            returnValue = CAN_ERROR_PASIVE_MODE;
            break;
            /* The buss off condition appears only when Transmit error counter > 255 */
        case CAN_TRANSMITTER_BUS_OFF_VALUE:
            returnValue = CAN_BUSS_OFF_MODE;

            break;
    }
    return returnValue;
}

void CAN_vSetIdentifier(CAN_Frame * frame, CAN_Buffer * buffer)
{
    if (CAN_ModuleConfiguration.Module_FrameType == CAN_EXTENDED_FRAME)
    {
        /* 8 LSB will be added to EIDL */
        buffer->BxEIDL = frame->Frame_Identifier;
        /* 15 */
        buffer->BxEIDH = CAN_GET_EIDH_VALUE_EXTENDED(frame->Frame_Identifier);

        /* 16,17,18,19,20*/
        buffer->BxSIDL |= ((frame->Frame_Identifier >> CAN_SIDL_EXTENDED_POSITION) & CAN_SIDL_EXTENDED_MASK1);
        buffer->BxSIDL |= ((frame->Frame_Identifier >> CAN_SIDL_EXTENDED_POSITION2) & CAN_SIDL_EXTENDED_MASK2);
        /* EXIDE = 1 = Message will transmit extended ID, SID<10:0> become EID<28:18> */
        MASK_8BIT_SET_BIT(buffer->BxSIDL, CAN_EXIDE_POSITION);
        /* 21,22,23,24,25,26,27,28 */
        buffer->BxSIDH = CAN_GET_SIDH_VALUE_EXTENDED(frame->Frame_Identifier);
    }
    else if (CAN_ModuleConfiguration.Module_FrameType == CAN_STANDARD_FRAME)
    {

        buffer->BxSIDL = CAN_GET_SIDL_VALUE_STANDARD(frame->Frame_Identifier);
        buffer->BxSIDH = CAN_GET_SIDH_VALUE_STANDARD(frame->Frame_Identifier);
        buffer->BxEIDL = 0x00;
        buffer->BxEIDH = 0x00;
        /*EXIDE = 0 = Message will transmit standard ID, EID<17:0> are ignored */
        MASK_8BIT_CLEAR_BIT(buffer->BxSIDL, CAN_EXIDE_POSITION);
    }
}

/* In the programmable buffers first are the receive buffers and after that comes transmit buffers */
uint8_t CAN_vFindBufferReadyForTransfer()
{
    uint8_t returnValue = 0xFF;
    uint8_t index = 0;
    /* Parsing the default buffers*/
    for (index = 0; index < CAN_NUMBER_OF_TXBF; index++)
    {
        /* TXREQ: Transmit Request Status bit(2,4)
        1 = Requests sending a message; clears the TXABT, TXLARB and TXERR bits
        0 = Automatically cleared when the message is successfully sent  
         */
        if (MASK_8BIT_GET_BIT(CAN_TransmitBuffers[index]->BxCON, CAN_TXREQ_POSITION) == 0)
        {
            returnValue = index;
            break;
        }
    }
    if (returnValue == 0xFF)
    {
        for (index = CAN_NumberOfReceiveBuffers; index < CAN_NUMBER_OF_PROGRAMMABLE_BUFFERS; index++)
        {
            /* TXREQ: Transmit Request Status bit(2,4)
            1 = Requests sending a message; clears the TXABT, TXLARB and TXERR bits
            0 = Automatically cleared when the message is successfully sent  
             */
            if (MASK_8BIT_GET_BIT(CAN_ProgrammableBuffers[index]->BxCON, CAN_TXREQ_POSITION) == 0)
            {
                /* If the free buffer that was found is from the programmable ones,
                 *  add CAN_PROGRAMMABLE_BUFFER_FLAG_VALUE to the value that will be returned*/
                returnValue = (CAN_PROGRAMMABLE_BUFFER_FLAG_VALUE | index);
                break;
            }
        }
    }
    return returnValue;

}

void CAN_vRequestTransceiverNormalMode()
{
    /* Setting the normal mode for the TRANSCEIVER;
     * Setting NSIL - LOGIC HIGH
     * Setting STBY - LOGIC LOW
     *  */

    /* Setting the pins as output drain because the transceiver has internal pull up resistors*/

    GPIO_vSetPinOpenDrain(CAN_TRANSCEIVER_NSIL_PORT_PIN, true);
    GPIO_vSetPinOpenDrain(CAN_TRANSCEIVER_STBY_PORT_PIN, true);
    //GPIO_vSetPinOpenDrain(CAN_TRANSCEIVER_TX_PORT_PIN, true);
    GPIO_vSetPinDirection(CAN_TRANSCEIVER_STBY_PORT_PIN, GPIO_OUTPUT_PIN);
    GPIO_vSetPinDirection(CAN_TRANSCEIVER_NSIL_PORT_PIN, GPIO_OUTPUT_PIN);

    /* Setting this to 0 logic will make the transceiver to enter Normal state */
    GPIO_vSetPinLevel(CAN_TRANSCEIVER_STBY_PORT_PIN, STD_LOW);
    GPIO_vSetPinLevel(CAN_TRANSCEIVER_NSIL_PORT_PIN, STD_HIGH);
}

void CAN_vFrameSetData(CAN_Frame * frame, uint8_t * dataBytes, uint8_t numberOfDataBytes)
{
    uint8_t loop_index = 0;
    for (loop_index = 0; loop_index < numberOfDataBytes; loop_index++)
    {
        frame->Frame_DataLength = numberOfDataBytes;
        frame->Frame_DataBytes[loop_index] = dataBytes[loop_index];
    }
}

bool CAN_bFIFOContainsData()
{
    uint8_t FIFOcontainsData = MASK_8BIT_GET_BIT(COMSTAT, CAN_FIFO_EMPTY_POSITION);
    if (FIFOcontainsData == 1)
    {
        MASK_8BIT_CLEAR_BIT(COMSTAT, CAN_FIFO_EMPTY_POSITION);
    }
    return FIFOcontainsData;
}