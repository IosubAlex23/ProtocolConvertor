/**
 * \file       MainApplication.c
 * \author     Iosub Mihai Alexandru
 * \brief      Short description for this source file
 */

/*----------------------------------------------------------------------------*/
/*                                 Includes                                   */
/*----------------------------------------------------------------------------*/
#include "MainApplication.h"
/*----------------------------------------------------------------------------*/
/*                               Local defines                                */
/*----------------------------------------------------------------------------*/
#define DTBS_I2C_FIFO_SIZE              (32)
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
MainApplication_State Application_State;

MainApplication_DataToBeSent DataToBeSent_CAN[] = {0, 0, 0, 0, 0};
MainApplication_DataToBeSent DataToBeSent_I2C[DTBS_I2C_FIFO_SIZE];

/*  TargetProtocol, StateOfTheRequest, *Request_DTBS_ptr, TargetLocation, Receiver;*/
MainApplication_LookUpTable LookUpTable_I2C[] = {
    {APP_PROTOCOL_CAN, REQUEST_IDLE, DataToBeSent_CAN, 0x64, 0x08},
    {APP_PROTOCOL_CAN, REQUEST_IDLE, DataToBeSent_CAN, 0x6F, 0x09},
};

MainApplication_LookUpTable LookUpTable_CAN[] = {
    {APP_PROTOCOL_I2C, REQUEST_IDLE, DataToBeSent_I2C, 0x08, 0x64},
    {APP_PROTOCOL_I2C, REQUEST_IDLE, DataToBeSent_I2C, 0x09, 0x6F},
};

uint8_t DataToBeSent_I2C_StackPointer;
uint8_t DataToBeSent_I2C_ReadPointer;

uint8_t LookUpTable_I2C_Elements;
uint8_t LookUpTable_CAN_Elements;
/*----------------------------------------------------------------------------*/
/*                       Declaration of local functions                       */
/*----------------------------------------------------------------------------*/
/**
 * \brief     This function initializes the device
 * \param     None
 * \return    None 
 */
void MainApplication_vInit(void);
/**
 * \brief     This is the main function of the application, puts / reads data in / from rx / tx buffers;
 * \param     None
 * \return    None 
 */
void MainApplication_vConvert(void);
/**
 * \brief     This function searches within the targetTable for the targetReceiver;
 * \param     targetProtocol - the table where to search; ex: LookUpTable_I2C - if something was received via I2C and want to find on which 
 *              protocol to send the received data;
 *            targetReceiver - the value of the address / identifier that is searched 
 * \return    MainAplication_Protocol - returns the protocol where the received data should be sent. 
 */
MainApplication_LookUpTable * MainApplication_uiCheckLookUpTable(MainAplication_Protocol targetProtocol, uint32_t * targetReceiver);
/**
 * \brief     This function creates a slot in the DataToBeSent array according to the target protocol
 * \param     targetProtocol - the table where to create the slot; 
 * \return    MainApplication_DataToBeSent - The address of the element in the DataToBeSent array
 */
MainApplication_DataToBeSent * MainApplication_CreateDataToBeSentSlot(MainAplication_Protocol protocol, uint8_t requestedNumberOfBytes, uint32_t * targetAdress);
/**
 * \brief     This function searches within the targetTable for the targetReceiver;
 * \param     targetProtocol - the table where to search; ex: LookUpTable_I2C - if something was received via I2C and want to find on which 
 *              protocol to send the received data;
 *            targetReceiver - the value of the address / identifier that is searched 
 * \return    MainAplication_Protocol - returns the protocol where the received data should be sent. 
 */
void MainApplication_vAddDataToBeSent(MainAplication_Protocol protocol, uint8_t * dataToBeSend, uint32_t * targetAdress, bool lastByte);
/**
 * \brief     This function searches within the targetTable for the targetReceiver;
 * \param     targetProtocol - the table where to search; ex: LookUpTable_I2C - if something was received via I2C and want to find on which 
 *              protocol to send the received data;
 *            targetReceiver - the value of the address / identifier that is searched 
 * \return    MainAplication_Protocol - returns the protocol where the received data should be sent. 
 */
void MainApplication_vAddDataToBuffers(void);
/**
 * \brief     This function searches within the targetTable for the targetReceiver;
 * \param     targetProtocol - the table where to search; ex: LookUpTable_I2C - if something was received via I2C and want to find on which 
 *              protocol to send the received data;
 *            targetReceiver - the value of the address / identifier that is searched 
 * \return    MainAplication_Protocol - returns the protocol where the received data should be sent. 
 */
void MainApplication_vAddDataToBuffers_CAN(void);
/**
 * \brief     This function verifies if the receive buffers contains new data.
 * \param     None
 * \return    None
 */
void MainApplication_vCheckReceiveBuffers(void);


uint8_t BasicOperations_BinarySearch(MainApplication_LookUpTable table[], uint8_t startIndex, uint8_t endIndex, uint8_t searchedValue);
/*----------------------------------------------------------------------------*/
/*                     Implementation of global functions                     */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*                     Implementation of local functions                      */

/*----------------------------------------------------------------------------*/
void MainApplication_vInit()
{
    Application_State = APP_RUNNING_STATE;
    LookUpTable_I2C_Elements = 2;
    LookUpTable_CAN_Elements = 2;
    DataToBeSent_I2C_StackPointer = 0;
    DataToBeSent_I2C_ReadPointer = 0;
}

void main(void)
{
    // Initialize the device
    SYSTEM_Initialize();
    TimeoutModule_vInit();
    I2C_vInit();
    //I2C_vJoinAsSlave(0x08);
    CAN_Configuration CAN_config;
    CAN_config.Module_FrameType = CAN_STANDARD_FRAME;
    CAN_config.Module_FunctionalMode = CAN_ENHANCED_FUNCTIONAL_MODE;
    CAN_config.Module_OperationMode = CAN_NORMAL_OPERATION_MODE;
    CAN_config.Module_ReceiveFIFO_Size = 0;
    CAN_config.Module_BaudRate = CAN_250KBITS;
    CAN_vInit(&CAN_config);
    MainApplication_vInit();
    uint8_t arr[] = {0x33, 0x12};
    uint8_t a = 1, b = 0;
    CAN_Buffer * bfrPtr = CAN_uiGetBufferAdrress(&a);

    while (1)
    {
        switch (Application_State)
        {
                /* The code under this case will execute while connected to the device that is configuring the protocol convertor */
            case APP_CONFIGURATION_STATE:

                break;
                /* The code under this case will realise the conversion */
            case APP_RUNNING_STATE:
                MainApplication_vConvert();
                MainApplication_vAddDataToBuffers();

                //                if((RXB1CON & 0x80) == 0x80)
                //                {
                //                    I2C_vMasterTransmit(0x08, 0x34, bfrPtr->BxData[0]);
                //                    RXB1CON &= 0x7F;
                //                }
                break;
        }
    }
}

void MainApplication_vConvert()
{
    uint8_t index, bufferIndex;
    uint8_t receivedData_I2C = 0;
    uint8_t numberOfReceiveBuffers_CAN = CAN_uiGetNumberOfReceiveBuffers();
    uint32_t matchedAdrr;
    CAN_Buffer * target_CAN_buffer;
    CAN_Frame frameToBeSent;
    MainApplication_LookUpTable * targetTable;
    /* This is used to convert data received from I2C_Slave Receiver to the corespondent protocol according to the LookUpTable*/
    if (true == I2C_vSlaveMainFunction(&receivedData_I2C, (uint16_t*) & matchedAdrr))
    {
        targetTable = MainApplication_uiCheckLookUpTable(APP_PROTOCOL_I2C, &matchedAdrr);
        if (true == I2C_bOperationWasARead())
        {

        }
        else
        {
            if ((true == I2C_bStopDetected()))
            {
                MainApplication_vAddDataToBeSent(targetTable->TargetProtocol, &receivedData_I2C, &targetTable->TargetLocation, true);
            }
            else
            {
                MainApplication_vAddDataToBeSent(targetTable->TargetProtocol, &receivedData_I2C, &targetTable->TargetLocation, false);
            }
        }
    }
    for (bufferIndex = 0; bufferIndex < numberOfReceiveBuffers_CAN; bufferIndex++)
    {
        target_CAN_buffer = CAN_uiGetBufferAdrress(&bufferIndex);
        /* Checking for new data from CAN bus*/
        if (CAN_bBufferHasNewData(target_CAN_buffer) == 1)
        {
            matchedAdrr = CAN_uiGetRxBufferIdentifier(&bufferIndex);
            targetTable = MainApplication_uiCheckLookUpTable(APP_PROTOCOL_CAN, &matchedAdrr);
            /* Checking if there was a Remote Transmission Request */
            if (CAN_bBufferRequestsData(target_CAN_buffer) == 1)
            {
                switch (targetTable->StateOfTheRequest)
                {
                    case REQUEST_IDLE:
                        targetTable->Request_DTBS_ptr = MainApplication_CreateDataToBeSentSlot(targetTable->TargetProtocol, MASK_8BIT_GET_LSB_HALF(target_CAN_buffer->BxDLC), &targetTable->TargetLocation);
                        targetTable->StateOfTheRequest = REQUEST_DATA_PENDING;
                        switch (targetTable->TargetProtocol)
                        {
                            case APP_PROTOCOL_I2C:
                                I2C_vMasterRead(targetTable->TargetLocation, targetTable->Request_DTBS_ptr->NextIndex, targetTable->Request_DTBS_ptr->Data);
                                targetTable->StateOfTheRequest = REQUEST_DATA_READY;
                                /* This is set to pending so that the received data will not be sent back */
                                targetTable->Request_DTBS_ptr->DataState = DATA_PENDING;
                                frameToBeSent.Frame_DataLength = targetTable->Request_DTBS_ptr->NextIndex;
                                for (index = 0; index < frameToBeSent.Frame_DataLength; index++)
                                {
                                    frameToBeSent.Frame_DataBytes[index] = targetTable->Request_DTBS_ptr->Data[index];
                                }
                                frameToBeSent.Frame_Identifier = matchedAdrr;
                                frameToBeSent.Frame_RTR = 0;
                                CAN_vTransmitFrame(frameToBeSent);
                                targetTable->StateOfTheRequest = REQUEST_IDLE;
                                break;
                        }
                        break;
                    case REQUEST_DATA_READY:


                        break;
                }
                CAN_vSetBufferAsFree(target_CAN_buffer);
            }
                /* If it was not a RTR it means that new data was received */
            else
            {
                switch (targetTable->TargetProtocol)
                {
                    case APP_PROTOCOL_I2C:
                        for (index = 0; index < target_CAN_buffer->BxDLC; index++)
                        {
                            if (index == (target_CAN_buffer->BxDLC - 1))
                            {
                                MainApplication_vAddDataToBeSent(targetTable->TargetProtocol, &target_CAN_buffer->BxData[index], &targetTable->TargetLocation, true);
                            }
                            else
                            {
                                MainApplication_vAddDataToBeSent(targetTable->TargetProtocol, &target_CAN_buffer->BxData[index], &targetTable->TargetLocation, false);
                            }
                        }
                        break;
                }
                CAN_vSetBufferAsFree(target_CAN_buffer);
            }
        }
        /* This checks if data was requested from CAN */
        /*else */
    }
}

MainApplication_LookUpTable * MainApplication_uiCheckLookUpTable(MainAplication_Protocol targetProtocol, uint32_t * targetReceiver)
{
    MainApplication_LookUpTable * returnValue;
    uint8_t targetIndex = 0;
    switch (targetProtocol)
    {
        case APP_PROTOCOL_CAN:
            targetIndex = BasicOperations_BinarySearch(LookUpTable_CAN, 0, (LookUpTable_CAN_Elements - 1), *targetReceiver);
            returnValue = &LookUpTable_CAN[targetIndex];
            break;
        case APP_PROTOCOL_I2C:
            targetIndex = BasicOperations_BinarySearch(LookUpTable_I2C, 0, (LookUpTable_I2C_Elements - 1), *targetReceiver);
            returnValue = &LookUpTable_I2C[targetIndex];
            /*if ((0xFF) != targetIndex)
            {
                returnValue = LookUpTable_I2C[targetIndex].TargetProtocol;
            }
            else
            {*/
            /* Do something here, when the targetReceiver was not found */
            //}
            break;
        case APP_PROTOCOL_LIN:
            break;
        case APP_PROTOCOL_RS232:
            break;
        case APP_PROTOCOL_UNKNOWN:
            break;
    }
    return returnValue;
}

MainApplication_DataToBeSent * MainApplication_CreateDataToBeSentSlot(MainAplication_Protocol protocol, uint8_t requestedNumberOfBytes, uint32_t * targetAdress)
{
    MainApplication_DataToBeSent * returnValue;
    switch (protocol)
    {
        case APP_PROTOCOL_CAN:
            DataToBeSent_CAN[0].NextIndex = requestedNumberOfBytes;
            DataToBeSent_CAN[0].TargetLocation = *targetAdress;
            DataToBeSent_CAN[0].DataState = DATA_PENDING;
            returnValue = &DataToBeSent_CAN[0];
            break;
        case APP_PROTOCOL_I2C:
            DataToBeSent_I2C[DataToBeSent_I2C_StackPointer].NextIndex = requestedNumberOfBytes;
            DataToBeSent_I2C[DataToBeSent_I2C_StackPointer].TargetLocation = *targetAdress;
            DataToBeSent_I2C[DataToBeSent_I2C_StackPointer].DataState = DATA_PENDING;
            returnValue = &DataToBeSent_I2C[DataToBeSent_I2C_StackPointer];
            DataToBeSent_I2C_StackPointer++;
            if (DataToBeSent_I2C_StackPointer >= DTBS_I2C_FIFO_SIZE)
            {
                DataToBeSent_I2C_StackPointer = 0;
            }
            break;
        case APP_PROTOCOL_LIN:
            break;
        case APP_PROTOCOL_RS232:
            break;
        case APP_PROTOCOL_UNKNOWN:
            break;
    }
    return returnValue;
}

void MainApplication_vAddDataToBeSent(MainAplication_Protocol protocol, uint8_t * dataToBeSend, uint32_t * targetAdress, bool lastByte)
{
    uint8_t nextIndex;
    switch (protocol)
    {
        case APP_PROTOCOL_CAN:
            nextIndex = DataToBeSent_CAN[0].NextIndex;
            DataToBeSent_CAN[0].Data[nextIndex] = *dataToBeSend;
            DataToBeSent_CAN[0].TargetLocation = *targetAdress;
            if (true == lastByte)
            {
                //                DataToBeSent_CAN[0].ReadyForSending = true;
                //                DataToBeSent_CAN[0].DataWasSent = false;

                DataToBeSent_CAN[0].DataState = DATA_READY;
            }
            else
            {
                DataToBeSent_CAN[0].NextIndex++;
                DataToBeSent_CAN[0].DataState = DATA_PENDING;
                //                DataToBeSent_CAN[0].ReadyForSending = false;
                //                DataToBeSent_CAN[0].DataWasSent = false;
            }
            break;
        case APP_PROTOCOL_I2C:
            nextIndex = DataToBeSent_I2C[DataToBeSent_I2C_StackPointer].NextIndex;
            DataToBeSent_I2C[DataToBeSent_I2C_StackPointer].Data[nextIndex] = *dataToBeSend;
            DataToBeSent_I2C[DataToBeSent_I2C_StackPointer].TargetLocation = *targetAdress;
            if (true == lastByte)
            {
                DataToBeSent_I2C[DataToBeSent_I2C_StackPointer].DataState = DATA_READY;
                //                DataToBeSent_I2C[DataToBeSent_I2C_StackPointer].ReadyForSending = true;
                //                DataToBeSent_I2C[DataToBeSent_I2C_StackPointer].DataWasSent = false;

                DataToBeSent_I2C_StackPointer++;
                if (DataToBeSent_I2C_StackPointer >= DTBS_I2C_FIFO_SIZE)
                {
                    DataToBeSent_I2C_StackPointer = 0;
                }
            }
            else
            {
                DataToBeSent_I2C[DataToBeSent_I2C_StackPointer].NextIndex++;
                DataToBeSent_I2C[DataToBeSent_I2C_StackPointer].DataState = DATA_PENDING;
                //                DataToBeSent_I2C[DataToBeSent_I2C_StackPointer].ReadyForSending = false;
                //                DataToBeSent_I2C[DataToBeSent_I2C_StackPointer].DataWasSent = false;
            }
            break;
        case APP_PROTOCOL_LIN:
            break;
        case APP_PROTOCOL_RS232:
            break;
        case APP_PROTOCOL_UNKNOWN:
            break;
    }

}

void MainApplication_vAddDataToBuffers()
{
    uint8_t index;
    /* This is for sending via CAN */
    if (DataToBeSent_CAN[0].DataState == DATA_READY)
    {
        MainApplication_vAddDataToBuffers_CAN();
        //        DataToBeSent_CAN[0].DataWasSent = true;
        DataToBeSent_CAN[0].DataState = DATA_WAS_SENT;
    }
    /* This is for sending via I2C */
    //    for (index = 0; index < DataToBeSent_I2C_StackPointer; index++)
    //    {
    if (DataToBeSent_I2C[DataToBeSent_I2C_ReadPointer].DataState == DATA_READY)
    {

        I2C_vMasterTransmitBytes(DataToBeSent_I2C[DataToBeSent_I2C_ReadPointer].TargetLocation, DataToBeSent_I2C[DataToBeSent_I2C_ReadPointer].Data, ((DataToBeSent_I2C[DataToBeSent_I2C_ReadPointer].NextIndex) + 1));
        //I2C_vMasterTransmit(DataToBeSent_I2C[DataToBeSent_I2C_ReadPointer].TargetLocation, DataToBeSent_I2C[DataToBeSent_I2C_ReadPointer].Data[0],DataToBeSent_I2C[DataToBeSent_I2C_ReadPointer].Data[1]);
        //I2C_vMasterTransmit(DataToBeSent_I2C[DataToBeSent_I2C_ReadPointer].TargetLocation, 0x55, 0x33);
        //        DataToBeSent_I2C[DataToBeSent_I2C_ReadPointer].DataWasSent = true;
        //        DataToBeSent_I2C[DataToBeSent_I2C_ReadPointer].ReadyForSending = false;
        DataToBeSent_I2C[DataToBeSent_I2C_ReadPointer].DataState = DATA_WAS_SENT;
        DataToBeSent_I2C[DataToBeSent_I2C_ReadPointer].NextIndex = 0;
        DataToBeSent_I2C_ReadPointer++;
        if (DataToBeSent_I2C_ReadPointer == DTBS_I2C_FIFO_SIZE)
        {
            DataToBeSent_I2C_ReadPointer = 0;
        }
    }

    //    }
}

void MainApplication_vAddDataToBuffers_CAN()
{

    CAN_Frame frame;
    CAN_vFrameSetData(&frame, DataToBeSent_CAN[0].Data, (DataToBeSent_CAN[0].NextIndex + 1));
    DataToBeSent_CAN[0].NextIndex = 0;
    frame.Frame_Identifier = DataToBeSent_CAN[0].TargetLocation;
    frame.Frame_RTR = false;
    CAN_vTransmitFrame(frame);
}

void MainApplication_vAddDataToBuffers_I2C()
{

    CAN_Frame frame;
    CAN_vFrameSetData(&frame, DataToBeSent_CAN[0].Data, (DataToBeSent_CAN[0].NextIndex + 1));
    DataToBeSent_CAN[0].NextIndex = 0;
    frame.Frame_Identifier = DataToBeSent_CAN[0].TargetLocation;
    frame.Frame_RTR = false;
    CAN_vTransmitFrame(frame);
}

uint8_t BasicOperations_BinarySearch(MainApplication_LookUpTable table[], uint8_t startIndex, uint8_t endIndex, uint8_t searchedValue)
{
    while (startIndex <= endIndex)
    {
        uint8_t middle = startIndex + ((endIndex - startIndex) / 2);
        if (table[middle].Receiver == searchedValue)
            return middle;

        if (table[middle].Receiver < searchedValue)
            startIndex = middle + 1;
        else
            endIndex = middle - 1;
    }

    /* return -1 if the element is not found */
    return 0xFF;
}

/**
 End of File
 */