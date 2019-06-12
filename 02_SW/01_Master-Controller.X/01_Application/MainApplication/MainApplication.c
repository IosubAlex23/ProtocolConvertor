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
#define DTBS_CAN_FIFO_SIZE                  (32u)
#define DTBS_I2C_FIFO_SIZE                  (32u)
#define DTBS_RS232_FIFO_SIZE                (32u)
#define LKT_I2C_SIZE                        (16u)
#define LKT_CAN_SIZE                        (16u)
/* This is is when asking the configurator for data */
#define SPI_REQUEST_DATA_VALUE              (0x21)
#define APP_START_BYTE_MASK                 (0xF8)
#define APP_START_BYTE                      (0xF8)
#define NUMBER_OF_PROTOCOLS                 (4u)
#define CONFIGURATOR_GET_PROTOCOL_ID(x)     ((x >> 1) & (0x03))
#define CONFIGURATOR_GET_PROTOCOL_STATE(x)  (MASK_8BIT_GET_BIT(x,0))
#define BYTES_PER_LKT                       (5u)
#define MASTER_SLAVE_MODE_POSITION          (7u)
#define RETRANSMISION_MASK                  (0xFE)
#define RETRANSMISION_VALUE                 (0xA8)
#define END_OF_CONFIG_MASK                  (0xF8)
#define END_OF_CONFIG_VALUE                 (0x58)
#define START_CONVERSION_FLAG_MASK          (0xFE)
#define START_CONVERSION_FLAG_VALUE         (0x64)
#define PROTOCOL_CONFIG_NEEDS_RETRANSMISION(x)  ((x & RETRANSMISION_MASK) == RETRANSMISION_VALUE)
#define END_OF_CONFIGURATION_BYTE(x)            ((x & END_OF_CONFIG_MASK) == END_OF_CONFIG_VALUE)
#define GET_PROTOCOL_FROM_LKT_LAST_BYTE(x)      (x & 0x03)
#define DEBUG_ACTIVE                        (0)
#define UINT32_MAX_VALUE                    (0xFFFFFFFF)
/*----------------------------------------------------------------------------*/
/*                              Local data types                              */

/*----------------------------------------------------------------------------*/
typedef enum
{
    CONFIGURATOR_IDLE = 0u,
    CONFIGURATOR_STARTED,
    CONFIGURATOR_LOOKUPS,
} Configurator_States;
/*----------------------------------------------------------------------------*/
/*                             Global data at RAM                             */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*                             Global data at ROM                             */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*                             Local data at RAM                              */
/*----------------------------------------------------------------------------*/
MainApplication_State Application_State;

MainApplication_DataToBeSent DataToBeSent_CAN[DTBS_CAN_FIFO_SIZE];
MainApplication_DataToBeSent DataToBeSent_I2C[DTBS_I2C_FIFO_SIZE];
MainApplication_DataToBeSent DataToBeSent_RS232[DTBS_RS232_FIFO_SIZE];

/*  TargetProtocol, StateOfTheRequest, *Request_DTBS_ptr, TargetLocation, Receiver;*/
MainApplication_LookUpTable LookUpTable_I2C[LKT_I2C_SIZE] = {
    {APP_PROTOCOL_CAN, REQUEST_IDLE, DataToBeSent_CAN, 0x64, 0x08},
    {APP_PROTOCOL_CAN, REQUEST_IDLE, DataToBeSent_CAN, 0x6F, 0x09},
};

MainApplication_LookUpTable LookUpTable_CAN[LKT_CAN_SIZE] = {
    {APP_PROTOCOL_I2C, REQUEST_IDLE, DataToBeSent_I2C, 0x09, 0x64},
    {APP_PROTOCOL_I2C, REQUEST_IDLE, DataToBeSent_I2C, 0x0a, 0x6F},
};

MainApplication_LookUpTable LookUpTable_RS232;

uint8_t DataToBeSent_CAN_StackPointer;
uint8_t DataToBeSent_CAN_ReadPointer;
uint32_t CAN_DataRequestedFromIdentifier;

uint8_t DataToBeSent_I2C_StackPointer;
uint8_t DataToBeSent_I2C_ReadPointer;

uint8_t DataToBeSent_RS232_StackPointer;
uint8_t DataToBeSent_RS232_ReadPointer;

uint8_t LookUpTable_I2C_Elements;
uint8_t LookUpTable_CAN_Elements;

uint8_t App_SPIReceivedData = 0;
Configurator_States ConfiguratorStates = CONFIGURATOR_IDLE;
bool App_ProtocolsState[NUMBER_OF_PROTOCOLS] = {false, false, false, false};

uint8_t App_NumberOfBytesToReceive;


uint8_t TestArray[32];
uint8_t TestArray_Size = 23;
/*----------------------------------------------------------------------------*/
/*                             Local data at ROM                              */
/*----------------------------------------------------------------------------*/
static const uint8_t App_Protocol_NumberOfBytesToReceive[NUMBER_OF_PROTOCOLS] = {2, 2, 0, 0};
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
 * \brief     This function initializes the look up tables
 * \param     None
 * \return    None 
 */
void MainApplication_vLookUpTableInit(void);
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
 * \brief     This function sets the DATA_READY flag fors conversions
 * \param     protocol - the table where to search; ex: LookUpTable_I2C - if something was received via I2C and want to find on which 
 *              protocol to send the received data;
 * \return    None
 */
void MainApplication_vSetLastByte(MainAplication_Protocol protocol);

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

/**
 * \brief     This function does a binary search on an array based on the searchBy parameter.
 * \param     None
 * \return    None
 */
uint8_t BasicOperations_BinarySearch(MainApplication_LookUpTable * table, uint8_t startIndex, uint8_t endIndex, uint8_t searchedValue, bool searchBy);
/*----------------------------------------------------------------------------*/
/*                     Implementation of global functions                     */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*                     Implementation of local functions                      */

/*----------------------------------------------------------------------------*/
void MainApplication_vInit()
{

    SPI_vInit(SLAVE_MODE);
    App_SPIReceivedData = 0;
    App_NumberOfBytesToReceive = 0;
    ConfiguratorStates = CONFIGURATOR_IDLE;
    //    Application_State = APP_CONFIGURATION_STATE;
    Application_State = APP_RUNNING_STATE;
    MainApplication_vLookUpTableInit();

}

void MainApplication_vLookUpTableInit(void)
{
    uint8_t index;
    /* I2C */
    for (index = 0; index < LKT_I2C_SIZE; index++)
    {
        LookUpTable_I2C[index].Receiver = 0xFF;
        LookUpTable_I2C[index].Request_DTBS_ptr = DataToBeSent_I2C;
        LookUpTable_I2C[index].StateOfTheRequest = REQUEST_IDLE;
        LookUpTable_I2C[index].TargetLocation = 0xFFFF;
        LookUpTable_I2C[index].TargetProtocol = APP_PROTOCOL_UNKNOWN;
    }

    LookUpTable_I2C_Elements = 0;
    DataToBeSent_I2C_StackPointer = 0;
    DataToBeSent_I2C_ReadPointer = 0;

    /* CAN */

    for (index = 0; index < LKT_CAN_SIZE; index++)
    {
        LookUpTable_CAN[index].Receiver = 0xFF;
        LookUpTable_CAN[index].Request_DTBS_ptr = DataToBeSent_CAN;
        LookUpTable_CAN[index].StateOfTheRequest = REQUEST_IDLE;
        LookUpTable_CAN[index].TargetLocation = 0xFFFF;
        LookUpTable_CAN[index].TargetProtocol = APP_PROTOCOL_UNKNOWN;
    }
    LookUpTable_CAN_Elements = 0;
    DataToBeSent_CAN_StackPointer = 0;
    DataToBeSent_CAN_ReadPointer = 0;
    CAN_DataRequestedFromIdentifier = UINT32_MAX_VALUE;

    /* RS-232 */
    LookUpTable_RS232.Receiver = 0xFF;
    LookUpTable_RS232.Request_DTBS_ptr = DataToBeSent_RS232;
    LookUpTable_RS232.StateOfTheRequest = REQUEST_IDLE;
    LookUpTable_RS232.TargetLocation = 0xFFFF;
    LookUpTable_RS232.TargetProtocol = APP_PROTOCOL_UNKNOWN;
    DataToBeSent_RS232_StackPointer = 0;
    DataToBeSent_RS232_ReadPointer = 0;
}

void TestArray_Add(uint8_t a)
{
    TestArray[TestArray_Size] = a;
    TestArray_Size++;
}

void main(void)
{
    // Initialize the device
    SYSTEM_Initialize();
    Timer2_vInit(TMR_400kHz);

    TimeoutModule_vInit();
    I2C_vInit();
    I2C_vJoinAsSlave(0x08);
    CAN_Configuration CAN_config;
    CAN_config.Module_FrameType = CAN_STANDARD_FRAME;
    CAN_config.Module_FunctionalMode = CAN_ENHANCED_FUNCTIONAL_MODE;
    CAN_config.Module_OperationMode = CAN_NORMAL_OPERATION_MODE;
    CAN_config.Module_ReceiveFIFO_Size = 2;
    CAN_config.Module_BaudRate = CAN_250KBITS;
    CAN_vInit(&CAN_config);

    RS232_actualConfig.communicationBaudGenSpeed = HIGH_SPEED;
    RS232_actualConfig.communicationDesiredBaud = BAUD_9600;
    RS232_actualConfig.communicationUartMode = ASYNC_8BIT;
    RS232_actualConfig.communicationPolarity = NON_INVERTED;
    RS232_actualConfig.communicationStopBitMode = ONE_STOP_BIT;
    RS232_vInit(&RS232_actualConfig);

    //    LIN_vInit(LIN_SLAVE);

    MainApplication_vInit();

    INTERRUPT_Initialize();
    MainAplication_Protocol Configurator_targetProtocol;
    MainApplication_LookUpTable Configurator_tempTable;
    uint8_t App_NumberOfConfigBytesReceived;
    uint8_t App_NumberOfLKTSToReceive;
    uint8_t Configurator_LKT_element_index;
    uint8_t Configurator_CurrentLKT;
    uint8_t Configurator_Response_Value = 1;
    uint8_t a = 1, b = 0;

    //I2C_vMasterTransmit(0x08, 0x33, 0x33);
    a = 0;
    TestArray[0] = 0x06;
    TestArray[1] = 0x64;
    TestArray[2] = 0x70;
    while (DEBUG_ACTIVE)
    {
        a++;
        //        LIN_vMasterReceive(0x06, 2, TestArray);
        //        LIN_vTransmit(0x06, 2, TestArray);

        //__delay_ms(100);
    }
    while (!DEBUG_ACTIVE)
    {

        switch (Application_State)
        {
                /* The code under this case will execute while connected to the device that is configuring the protocol convertor */
            case APP_CONFIGURATION_STATE:
                switch (ConfiguratorStates)
                {
                    case CONFIGURATOR_IDLE:

                        App_SPIReceivedData = SPI_uiExchangeByte(App_SPIReceivedData);
                        if ((App_SPIReceivedData & APP_START_BYTE_MASK) == APP_START_BYTE)
                        {
                            Configurator_targetProtocol = CONFIGURATOR_GET_PROTOCOL_ID(App_SPIReceivedData);
                            App_ProtocolsState[Configurator_targetProtocol] = CONFIGURATOR_GET_PROTOCOL_STATE(App_SPIReceivedData);
                            App_NumberOfConfigBytesReceived = 0;
                            ConfiguratorStates = CONFIGURATOR_STARTED;
                            Configurator_Response_Value++;
                            TestArray_Add(App_SPIReceivedData);
                        }
                        break;
                    case CONFIGURATOR_STARTED:
                        switch (Configurator_targetProtocol)
                        {
                            case APP_PROTOCOL_CAN:
                                for (App_NumberOfConfigBytesReceived = 0; App_NumberOfConfigBytesReceived < App_Protocol_NumberOfBytesToReceive[Configurator_targetProtocol]; App_NumberOfConfigBytesReceived++)
                                {
                                    App_SPIReceivedData = SPI_uiExchangeByte(App_SPIReceivedData);
                                    if (App_NumberOfConfigBytesReceived == 0u)
                                    {
                                        App_NumberOfLKTSToReceive = App_SPIReceivedData;
                                        LookUpTable_CAN_Elements = App_NumberOfLKTSToReceive;

                                    }
                                    else if (App_NumberOfConfigBytesReceived == 1u)
                                    {
                                        CAN_config.Module_BaudRate = App_SPIReceivedData;
                                    }
                                    Configurator_Response_Value++;
                                    if (App_NumberOfConfigBytesReceived == (App_Protocol_NumberOfBytesToReceive[Configurator_targetProtocol]) - 1)
                                    {
                                        Configurator_CurrentLKT = 0;
                                        ConfiguratorStates = CONFIGURATOR_LOOKUPS;
                                        Configurator_Response_Value = 3;
                                    }
                                    TestArray_Add(App_SPIReceivedData);
                                }
                                break;
                            case APP_PROTOCOL_I2C:
                                /* continue here*/
                                for (App_NumberOfConfigBytesReceived = 0; App_NumberOfConfigBytesReceived < App_Protocol_NumberOfBytesToReceive[Configurator_targetProtocol]; App_NumberOfConfigBytesReceived++)
                                {
                                    App_SPIReceivedData = SPI_uiExchangeByte(App_SPIReceivedData);
                                    if (App_NumberOfConfigBytesReceived == 0u)
                                    {
                                        App_NumberOfLKTSToReceive = App_SPIReceivedData;
                                        LookUpTable_I2C_Elements = App_NumberOfLKTSToReceive;
                                    }
                                    else if (App_NumberOfConfigBytesReceived == 1u)
                                    {
                                        /* Get MS Bit - that indicates the mode Master(1) / Slave(0) */
                                        if (0 == MASK_8BIT_GET_BIT(App_SPIReceivedData, MASTER_SLAVE_MODE_POSITION))
                                        {
                                            MASK_8BIT_CLEAR_BIT(App_SPIReceivedData, MASTER_SLAVE_MODE_POSITION);
                                            I2C_vJoinAsSlave(App_SPIReceivedData);
                                        }
                                        else if (1 == MASK_8BIT_GET_BIT(App_SPIReceivedData, MASTER_SLAVE_MODE_POSITION))
                                        {
                                            MASK_8BIT_CLEAR_BIT(App_SPIReceivedData, MASTER_SLAVE_MODE_POSITION);
                                            I2C_vSetCLK(App_SPIReceivedData);
                                        }
                                        /* The rest of the bits indicates the clock if Master mode is selected
                                            or the slave address if Slave mode is selected */
                                    }
                                    Configurator_Response_Value++;
                                    if (App_NumberOfConfigBytesReceived == (App_Protocol_NumberOfBytesToReceive[Configurator_targetProtocol]) - 1)
                                    {
                                        Configurator_CurrentLKT = 0;
                                        ConfiguratorStates = CONFIGURATOR_LOOKUPS;
                                        Configurator_Response_Value = 3;
                                    }
                                    TestArray_Add(App_SPIReceivedData);
                                }
                                break;
                        }
                        break;
                    case CONFIGURATOR_LOOKUPS:
                        for (Configurator_LKT_element_index = 0; Configurator_LKT_element_index < BYTES_PER_LKT; Configurator_LKT_element_index++)
                        {
                            switch (Configurator_LKT_element_index)
                            {
                                case 0:
                                    App_SPIReceivedData = SPI_uiExchangeByte(App_SPIReceivedData);
                                    Configurator_tempTable.Receiver = App_SPIReceivedData;
                                    break;
                                case 1:
                                    App_SPIReceivedData = SPI_uiExchangeByte(App_SPIReceivedData);
                                    MASK_16BIT_SET_MSB_HALF(Configurator_tempTable.Receiver, App_SPIReceivedData);
                                    break;
                                case 2:
                                    App_SPIReceivedData = SPI_uiExchangeByte(App_SPIReceivedData);
                                    Configurator_tempTable.TargetLocation = App_SPIReceivedData;
                                    break;
                                case 3:
                                    App_SPIReceivedData = SPI_uiExchangeByte(App_SPIReceivedData);
                                    MASK_16BIT_SET_MSB_HALF(Configurator_tempTable.TargetLocation, App_SPIReceivedData);
                                    break;
                                case 4:
                                    App_SPIReceivedData = SPI_uiExchangeByte(App_SPIReceivedData);
                                    if (1 == PROTOCOL_CONFIG_NEEDS_RETRANSMISION(App_SPIReceivedData))
                                    {
                                        ConfiguratorStates = CONFIGURATOR_IDLE;
                                        break;
                                    }
                                    Configurator_tempTable.TargetProtocol = GET_PROTOCOL_FROM_LKT_LAST_BYTE(App_SPIReceivedData);
                                    switch (Configurator_targetProtocol)
                                    {
                                        case APP_PROTOCOL_CAN:
                                            LookUpTable_CAN[Configurator_CurrentLKT].Request_DTBS_ptr = DataToBeSent_CAN;
                                            Configurator_tempTable.StateOfTheRequest = REQUEST_IDLE;
                                            LookUpTable_CAN[Configurator_CurrentLKT].StateOfTheRequest = REQUEST_IDLE;
                                            LookUpTable_CAN[Configurator_CurrentLKT].Receiver = Configurator_tempTable.Receiver;
                                            LookUpTable_CAN[Configurator_CurrentLKT].TargetLocation = Configurator_tempTable.TargetLocation;
                                            LookUpTable_CAN[Configurator_CurrentLKT].TargetProtocol = Configurator_tempTable.TargetProtocol;
                                            Configurator_CurrentLKT++;
                                            if (Configurator_CurrentLKT >= App_NumberOfLKTSToReceive)
                                            {
                                                ConfiguratorStates = CONFIGURATOR_IDLE;
                                                Application_State = APP_RUNNING_STATE;
                                                CAN_vInit(&CAN_config);
                                            }
                                            break;
                                        case APP_PROTOCOL_I2C:
                                            LookUpTable_I2C[Configurator_CurrentLKT].Request_DTBS_ptr = DataToBeSent_I2C;
                                            LookUpTable_I2C[Configurator_CurrentLKT].StateOfTheRequest = REQUEST_IDLE;
                                            LookUpTable_I2C[Configurator_CurrentLKT].Receiver = Configurator_tempTable.Receiver;
                                            LookUpTable_I2C[Configurator_CurrentLKT].TargetLocation = Configurator_tempTable.TargetLocation;
                                            LookUpTable_I2C[Configurator_CurrentLKT].TargetProtocol = Configurator_tempTable.TargetProtocol;
                                            Configurator_CurrentLKT++;
                                            if (Configurator_CurrentLKT >= App_NumberOfLKTSToReceive)
                                            {
                                                ConfiguratorStates = CONFIGURATOR_IDLE;
                                                Application_State = APP_RUNNING_STATE;
                                            }
                                            break;
                                        default:
                                            Configurator_CurrentLKT++;
                                            if (Configurator_CurrentLKT >= App_NumberOfLKTSToReceive)
                                            {
                                                ConfiguratorStates = CONFIGURATOR_IDLE;
                                                Application_State = APP_RUNNING_STATE;
                                            }
                                            break;

                                    }
                                    if (1 == END_OF_CONFIGURATION_BYTE(App_SPIReceivedData))
                                    {
                                        Application_State = APP_RUNNING_STATE;
                                    }
                                    break;
                            }
                            TestArray_Add(App_SPIReceivedData);
                        }
                        break;
                }
                break;
                /* The code under this case will realize the conversion */
            case APP_RUNNING_STATE:
                //                if (T2CON != 0x80)
                //                {
                //                    Timer2_vStart();
                //                }
                MainApplication_vConvert();
                MainApplication_vAddDataToBuffers();
                //                if (TestArray_Size != 0)
                //                {
                //                    //                    LIN_vTransmit(0x3a, 2, TestArray);
                //                                        TestArray_Size = 0;
                //                    //                    __delay_ms(1);
                //                    RS232_vSendData(0x66);
                //                }
                break;
        }
    }
}

void MainApplication_vConvert()
{
    bool I2C_IsAStop = false;
    uint8_t index, bufferIndex;
    uint8_t receivedData = 0;
    uint8_t numberOfReceiveBuffers_CAN = CAN_uiGetNumberOfReceiveBuffers();
    uint32_t matchedAdrr;
    CAN_Buffer * target_CAN_buffer;
    CAN_Frame frameToBeSent;
    MainApplication_LookUpTable * targetTable;
    I2C_SlaveOperationType slaveResponseStatus = I2C_vSlaveMainFunction(&receivedData, (uint16_t*) & matchedAdrr, &I2C_IsAStop);
    /* This is used to convert data received from I2C_Slave Receiver to the corespondent protocol according to the LookUpTable*/
    switch (slaveResponseStatus)
    {
        case I2C_NEW_DATA_RECEIVED:
            targetTable = MainApplication_uiCheckLookUpTable(APP_PROTOCOL_I2C, &matchedAdrr);
            if ((true == I2C_IsAStop))
            {
                MainApplication_vAddDataToBeSent(targetTable->TargetProtocol, &receivedData, &targetTable->TargetLocation, true);
            }
            else
            {
                MainApplication_vAddDataToBeSent(targetTable->TargetProtocol, &receivedData, &targetTable->TargetLocation, false);
            }
            break;
        case I2C_DATA_REQUESTED:
            targetTable = MainApplication_uiCheckLookUpTable(APP_PROTOCOL_I2C, &matchedAdrr);

            switch (targetTable->StateOfTheRequest)
            {
                case REQUEST_IDLE:
                    targetTable->Request_DTBS_ptr = MainApplication_CreateDataToBeSentSlot(targetTable->TargetProtocol, 0, &targetTable->TargetLocation);
                    targetTable->StateOfTheRequest = REQUEST_DATA_PENDING;
                    switch (targetTable->TargetProtocol)
                    {
                        case APP_PROTOCOL_CAN:
                            /* if (targetTable->StateOfTheRequest == REQUEST_IDLE)
                             {*/
                            frameToBeSent.Frame_Identifier = targetTable->TargetLocation;
                            frameToBeSent.Frame_DataLength = CAN_MAX_DATA_BYTES;
                            frameToBeSent.Frame_RTR = true;
                            CAN_vTransmitFrame(frameToBeSent);
                            targetTable->StateOfTheRequest = REQUEST_DATA_PENDING;
                            //}
                            CAN_DataRequestedFromIdentifier = targetTable->TargetLocation;


                            /* Tre sa citesc din RX buffers si apoi sa pun DATA READY */


                            break;
                    }
                    break;
                case REQUEST_DATA_PENDING:
                    /* Here can be implemented the response based on "cycles" so that the code executes the instructions
                     on a number o cycles
                     */
                    /*else if (targetTable->StateOfTheRequest == REQUEST_DATA_PENDING)
                    {*/
                    switch (targetTable->TargetProtocol)
                    {
                        case APP_PROTOCOL_CAN:
                            target_CAN_buffer = CAN_uiGetResponseBufferByIdentifier(targetTable->TargetLocation);
                            if (target_CAN_buffer != NULL)
                            {
                                I2C_vSlaveSetResponse(target_CAN_buffer->BxData, target_CAN_buffer->BxDLC);
                                targetTable->StateOfTheRequest = REQUEST_IDLE;
                                CAN_DataRequestedFromIdentifier = UINT32_MAX_VALUE;
                                CAN_vSetBufferAsFree(target_CAN_buffer);
                            }
                            /* Copiere date in DTBS*/
                            //                                targetTable->Request_DTBS_ptr->DataState = DATA_READY;
                            //                                targetTable->Request_DTBS_ptr->Data[index] = target_CAN_buffer->BxDLC[index];
                            break;
                    }
                    //}
                    break;
            }

            break;
        case I2C_NO_NEW_DATA:
            break;
    }

    /* This is the CAN part */
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
                        /* Here can be implemented the response based on "cycles" so that the code executes the instructions
                         on a number o cycles
                         */

                        break;
                }
                CAN_vSetBufferAsFree(target_CAN_buffer);
            }
                /* If it was not a RTR it means that new data was received */
            else
            {
                //if(DataToBeSent_CAN[0].)
                //                switch (targetTable->TargetProtocol)
                //                {
                //                    case APP_PROTOCOL_I2C:
                //                        for (index = 0; index < target_CAN_buffer->BxDLC; index++)
                //                        {
                //                            if (index == (target_CAN_buffer->BxDLC - 1))
                //                            {
                //                                MainApplication_vAddDataToBeSent(targetTable->TargetProtocol, &target_CAN_buffer->BxData[index], &targetTable->TargetLocation, true);
                //                            }
                //                            else
                //                            {
                //                                MainApplication_vAddDataToBeSent(targetTable->TargetProtocol, &target_CAN_buffer->BxData[index], &targetTable->TargetLocation, false);
                //                            }
                //                        }
                //                        break;
                //                    case APP_PROTOCOL_RS232:
                //
                //                        break;
                //                }
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
                if (CAN_uiGetIdentifier(target_CAN_buffer) != CAN_DataRequestedFromIdentifier)
                {
                    CAN_vSetBufferAsFree(target_CAN_buffer);
                }
            }
        }
        /* This checks if data was requested from CAN */
        /*else */
    }

    /* This is the RS-232 part */
    if (RS232_bReceiveDataReady() == 1)
    {
        receivedData = RS232_uiReceiveData();
        targetTable = &LookUpTable_RS232;

        MainApplication_vAddDataToBeSent(targetTable->TargetProtocol, &receivedData, &targetTable->TargetLocation, true);
    }
}

MainApplication_LookUpTable * MainApplication_uiCheckLookUpTable(MainAplication_Protocol targetProtocol, uint32_t * targetReceiver)
{
    MainApplication_LookUpTable * returnValue = NULL;
    uint8_t targetIndex = 0;
    switch (targetProtocol)
    {
        case APP_PROTOCOL_CAN:
            targetIndex = BasicOperations_BinarySearch(LookUpTable_CAN, 0, (LookUpTable_CAN_Elements - 1), *targetReceiver, 0);
            returnValue = &LookUpTable_CAN[targetIndex];
            break;
        case APP_PROTOCOL_I2C:
            targetIndex = BasicOperations_BinarySearch(LookUpTable_I2C, 0, (LookUpTable_I2C_Elements - 1), *targetReceiver, 0);
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
            DataToBeSent_CAN[DataToBeSent_CAN_StackPointer].NextIndex = requestedNumberOfBytes;
            DataToBeSent_CAN[DataToBeSent_CAN_StackPointer].TargetLocation = *targetAdress;
            DataToBeSent_CAN[DataToBeSent_CAN_StackPointer].DataState = DATA_PENDING;
            returnValue = &DataToBeSent_CAN[DataToBeSent_CAN_StackPointer];
            DataToBeSent_CAN_StackPointer++;
            if (DataToBeSent_CAN_StackPointer >= DTBS_CAN_FIFO_SIZE)
            {
                DataToBeSent_CAN_StackPointer = 0;
            }
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

void MainApplication_vSetLastByte(MainAplication_Protocol protocol)
{
    switch (protocol)
    {
        case APP_PROTOCOL_CAN:
            DataToBeSent_CAN[DataToBeSent_CAN_StackPointer].DataState = DATA_READY;
            DataToBeSent_CAN_StackPointer++;
            if (DataToBeSent_CAN_StackPointer >= DTBS_I2C_FIFO_SIZE)
            {
                DataToBeSent_CAN_StackPointer = 0;
            }

            break;
        case APP_PROTOCOL_I2C:

            DataToBeSent_I2C[DataToBeSent_I2C_StackPointer].DataState = DATA_READY;
            //                DataToBeSent_I2C[DataToBeSent_I2C_StackPointer].ReadyForSending = true;
            //                DataToBeSent_I2C[DataToBeSent_I2C_StackPointer].DataWasSent = false;

            DataToBeSent_I2C_StackPointer++;
            if (DataToBeSent_I2C_StackPointer >= DTBS_I2C_FIFO_SIZE)
            {
                DataToBeSent_I2C_StackPointer = 0;
            }

            break;
        case APP_PROTOCOL_LIN:
            break;
        case APP_PROTOCOL_RS232:

            DataToBeSent_RS232[DataToBeSent_RS232_StackPointer].DataState = DATA_READY;
            //                DataToBeSent_RS232[DataToBeSent_RS232_StackPointer].ReadyForSending = true;
            //                DataToBeSent_RS232[DataToBeSent_RS232_StackPointer].DataWasSent = false;

            DataToBeSent_RS232_StackPointer++;
            if (DataToBeSent_RS232_StackPointer >= DTBS_RS232_FIFO_SIZE)
            {
                DataToBeSent_RS232_StackPointer = 0;
            }

            break;
        case APP_PROTOCOL_UNKNOWN:
            break;
    }
}

void MainApplication_vAddDataToBeSent(MainAplication_Protocol protocol, uint8_t * dataToBeSend, uint32_t * targetAdress, bool lastByte)
{
    uint8_t nextIndex;
    switch (protocol)
    {
        case APP_PROTOCOL_CAN:
            nextIndex = DataToBeSent_CAN[DataToBeSent_CAN_StackPointer].NextIndex;
            DataToBeSent_CAN[DataToBeSent_CAN_StackPointer].Data[nextIndex] = *dataToBeSend;
            DataToBeSent_CAN[DataToBeSent_CAN_StackPointer].TargetLocation = *targetAdress;
            if (true == lastByte)
            {
                //                DataToBeSent_CAN[DataToBeSent_CAN_StackPointer].ReadyForSending = true;
                //                DataToBeSent_CAN[DataToBeSent_CAN_StackPointer].DataWasSent = false;

                DataToBeSent_CAN[DataToBeSent_CAN_StackPointer].DataState = DATA_READY;
                DataToBeSent_CAN_StackPointer++;
                if (DataToBeSent_CAN_StackPointer >= DTBS_I2C_FIFO_SIZE)
                {
                    DataToBeSent_CAN_StackPointer = 0;
                }
            }
            else
            {
                DataToBeSent_CAN[DataToBeSent_CAN_StackPointer].NextIndex++;
                DataToBeSent_CAN[DataToBeSent_CAN_StackPointer].DataState = DATA_PENDING;
                //                DataToBeSent_CAN[DataToBeSent_CAN_StackPointer].ReadyForSending = false;
                //                DataToBeSent_CAN[DataToBeSent_CAN_StackPointer].DataWasSent = false;
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
            nextIndex = DataToBeSent_RS232[DataToBeSent_RS232_StackPointer].NextIndex;
            DataToBeSent_RS232[DataToBeSent_RS232_StackPointer].Data[nextIndex] = *dataToBeSend;
            DataToBeSent_RS232[DataToBeSent_RS232_StackPointer].TargetLocation = *targetAdress;
            if (true == lastByte)
            {
                DataToBeSent_RS232[DataToBeSent_RS232_StackPointer].DataState = DATA_READY;
                //                DataToBeSent_RS232[DataToBeSent_RS232_StackPointer].ReadyForSending = true;
                //                DataToBeSent_RS232[DataToBeSent_RS232_StackPointer].DataWasSent = false;

                DataToBeSent_RS232_StackPointer++;
                if (DataToBeSent_RS232_StackPointer >= DTBS_RS232_FIFO_SIZE)
                {
                    DataToBeSent_RS232_StackPointer = 0;
                }
            }
            else
            {
                DataToBeSent_RS232[DataToBeSent_RS232_StackPointer].NextIndex++;
                DataToBeSent_RS232[DataToBeSent_RS232_StackPointer].DataState = DATA_PENDING;
                //                DataToBeSent_RS232[DataToBeSent_RS232_StackPointer].ReadyForSending = false;
                //                DataToBeSent_RS232[DataToBeSent_RS232_StackPointer].DataWasSent = false;
            }
            break;
        case APP_PROTOCOL_UNKNOWN:
            break;
    }

}

void MainApplication_vAddDataToBuffers()
{
    uint8_t index;
    /* This is for sending via CAN */
    if (DataToBeSent_CAN[DataToBeSent_CAN_ReadPointer].DataState == DATA_READY)
    {
        MainApplication_vAddDataToBuffers_CAN();
        //        DataToBeSent_CAN[0].DataWasSent = true;
        DataToBeSent_CAN[DataToBeSent_CAN_ReadPointer].DataState = DATA_WAS_SENT;
        DataToBeSent_CAN_ReadPointer++;
        if (DataToBeSent_CAN_ReadPointer >= DTBS_CAN_FIFO_SIZE)
        {
            DataToBeSent_CAN_ReadPointer = 0;
        }
    }
    /* This is for sending via I2C */
    //    for (index = 0; index < DataToBeSent_I2C_StackPointer; index++)
    //    {
    if (DataToBeSent_I2C[DataToBeSent_I2C_ReadPointer].DataState == DATA_READY)
    {
        if (true == I2C_bIsMasterModeActive())
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
    }

    /* This is for sending data on RS232 */
    if (DataToBeSent_RS232[DataToBeSent_RS232_ReadPointer].DataState == DATA_READY)
    {

        for (index = 0; index <= DataToBeSent_RS232[DataToBeSent_RS232_ReadPointer].NextIndex; index++)
        {
            RS232_vSendData(DataToBeSent_RS232[DataToBeSent_RS232_ReadPointer].Data[index]);
        }
        DataToBeSent_RS232[DataToBeSent_RS232_ReadPointer].DataState = DATA_WAS_SENT;
        DataToBeSent_RS232[DataToBeSent_RS232_ReadPointer].NextIndex = 0;
        DataToBeSent_RS232_ReadPointer++;
        if (DataToBeSent_RS232_ReadPointer == DTBS_RS232_FIFO_SIZE)
        {
            DataToBeSent_RS232_ReadPointer = 0;
        }

    }
    //    }
}

void MainApplication_vAddDataToBuffers_CAN()
{

    CAN_Frame frame;
    CAN_vFrameSetData(&frame, DataToBeSent_CAN[DataToBeSent_CAN_ReadPointer].Data, (DataToBeSent_CAN[DataToBeSent_CAN_ReadPointer].NextIndex + 1));
    DataToBeSent_CAN[DataToBeSent_CAN_ReadPointer].NextIndex = 0;
    DataToBeSent_CAN[DataToBeSent_CAN_ReadPointer].DataState = DATA_WAS_SENT;
    frame.Frame_Identifier = DataToBeSent_CAN[DataToBeSent_CAN_ReadPointer].TargetLocation;
    frame.Frame_RTR = false;
    CAN_vTransmitFrame(frame);
}

uint8_t BasicOperations_BinarySearch(MainApplication_LookUpTable * table, uint8_t startIndex, uint8_t endIndex, uint8_t searchedValue, bool searchBy)
{
    switch (searchBy)
    {
            /* Searching by receiver if searchBy == 0 */
        case 0:

            while (startIndex <= endIndex)
            {
                /* Right shifting with one place means division by two*/
                uint8_t middle = startIndex + ((endIndex - startIndex) >> 1);
                if (table[middle].Receiver == searchedValue)
                    return middle;

                if (table[middle].Receiver < searchedValue)
                    startIndex = middle + 1;
                else
                    endIndex = middle - 1;
            }
            break;
            /* Searching by TargetLocation if searchBy == 1 */
        case 1:
            while (startIndex <= endIndex)
            {
                /* Right shifting with one place means division by two*/
                uint8_t middle = startIndex + ((endIndex - startIndex) >> 1);
                if (table[middle].TargetLocation == searchedValue)
                    return middle;

                if (table[middle].TargetLocation < searchedValue)
                    startIndex = middle + 1;
                else
                    endIndex = middle - 1;
            }
            break;
    }
    /* return -1 if the element is not found */
    return 0xFF;
}

/**
 End of File
 */