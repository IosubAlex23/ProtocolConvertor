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
#define DTBS_LIN_FIFO_SIZE                  (32u)
#define DTBS_RS232_FIFO_SIZE                (32u)
#define LKT_I2C_SIZE                        (16u)
#define LKT_CAN_SIZE                        (16u)
#define LKT_LIN_SIZE                        (16u)
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
#define GET_STOP_BIT_LIN(x)                 (x & 0x03)
#define DEBUG_ACTIVE                        (0)
#define UINT32_MAX_VALUE                    (0xFFFFFFFF)
#define VALUE_FOR_LKT_NOT_FOUND_BINARY_SRC  (0xFF)
#define LIN_NR_OF_BYTES_POS                 (0x03)
#define LIN_NR_OF_BYTES_MASK                (0x0F)
#define LIN_GET_NUMBER_OF_BYTES_FROM_LKT(x) ((x >> LIN_NR_OF_BYTES_POS) & LIN_NR_OF_BYTES_MASK)
#define CONVERSION_RUNNING_LED_PIN          (0xA5)
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
MainApplication_DataToBeSent DataToBeSent_LIN[DTBS_LIN_FIFO_SIZE];
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

MainApplication_LookUpTable LookUpTable_LIN[LKT_LIN_SIZE];

MainApplication_LookUpTable LookUpTable_RS232;

uint8_t DataToBeSent_CAN_StackPointer;
uint8_t DataToBeSent_CAN_ReadPointer;
uint32_t CAN_DataRequestedFromIdentifier;

uint8_t DataToBeSent_I2C_StackPointer;
uint8_t DataToBeSent_I2C_ReadPointer;

uint8_t DataToBeSent_LIN_StackPointer;
uint8_t DataToBeSent_LIN_ReadPointer;

uint8_t DataToBeSent_RS232_StackPointer;
uint8_t DataToBeSent_RS232_ReadPointer;

uint8_t LookUpTable_I2C_Elements;
uint8_t LookUpTable_CAN_Elements;
uint8_t LookUpTable_LIN_Elements;

uint8_t App_SPIReceivedData = 0;
Configurator_States ConfiguratorStates = CONFIGURATOR_IDLE;
bool App_ProtocolsState[NUMBER_OF_PROTOCOLS] = {false, false, false, false};

uint8_t App_NumberOfBytesToReceive;

LIN_Configuration LIN_actualConfig;

/*----------------------------------------------------------------------------*/
/*                             Local data at ROM                              */
/*----------------------------------------------------------------------------*/
static uint8_t App_Protocol_NumberOfBytesToReceive[NUMBER_OF_PROTOCOLS] = {2, 2, 3, 3};
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

BaudValue MainApplication_uiLINGetBaudValue(uint8_t indexOfBaud);
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
    Application_State = APP_CONFIGURATION_STATE;
    //    Application_State = APP_RUNNING_STATE;
    GPIO_vSetPinDirection(CONVERSION_RUNNING_LED_PIN, GPIO_OUTPUT_PIN);
    GPIO_vSetPinLevel(CONVERSION_RUNNING_LED_PIN, STD_LOW);
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

    /* LIN */

    for (index = 0; index < LKT_LIN_SIZE; index++)
    {
        LookUpTable_LIN[index].Receiver = 0xFF;
        LookUpTable_LIN[index].Request_DTBS_ptr = DataToBeSent_LIN;
        LookUpTable_LIN[index].StateOfTheRequest = REQUEST_IDLE;
        LookUpTable_LIN[index].TargetLocation = 0xFFFF;
        LookUpTable_LIN[index].TargetProtocol = APP_PROTOCOL_UNKNOWN;
    }
    LookUpTable_LIN_Elements = 0;
    DataToBeSent_LIN_StackPointer = 0;
    DataToBeSent_LIN_ReadPointer = 0;

    /* RS-232 */
    LookUpTable_RS232.Receiver = 0xFF;
    LookUpTable_RS232.Request_DTBS_ptr = DataToBeSent_RS232;
    LookUpTable_RS232.StateOfTheRequest = REQUEST_IDLE;
    LookUpTable_RS232.TargetLocation = 0xFFFF;
    LookUpTable_RS232.TargetProtocol = APP_PROTOCOL_UNKNOWN;
    DataToBeSent_RS232_StackPointer = 0;
    DataToBeSent_RS232_ReadPointer = 0;
}

void main(void)
{
    // Initialize the device
    SYSTEM_Initialize();
    Timer2_vInit(TMR_800kHz);

    TimeoutModule_vInit();
    I2C_vInit();
    //    I2C_vJoinAsSlave(0x08);
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

    LIN_actualConfig.configBaudGeneratorSpeed = HIGH_SPEED;
    LIN_actualConfig.configBaudValue = BAUD_19200;
    LIN_actualConfig.configUartMode = LIN_SLAVE;
    LIN_actualConfig.configTransmitPolarity = NON_INVERTED;
    LIN_actualConfig.configStopBitMode = ONE_STOP_BIT;
    LIN_actualConfig.config_ChecksumMode = ENHANCED;
    LIN_vInit(&LIN_actualConfig);

    MainApplication_vInit();
    INTERRUPT_Initialize();
    MainAplication_Protocol Configurator_targetProtocol;
    MainApplication_LookUpTable Configurator_tempTable;
    uint8_t App_NumberOfConfigBytesReceived;
    uint8_t App_NumberOfLKTSToReceive;
    uint8_t Configurator_LKT_element_index;
    uint8_t Configurator_CurrentLKT;
    uint8_t Configurator_Response_Value = 1;
    uint8_t a = 0, b = 0;
    BaudValue baudForConfig;

    //I2C_vMasterTransmit(0x08, 0x33, 0x33);
    while (DEBUG_ACTIVE)
    {
        b++;
        if (a != 0)
        {
            //            LIN_vTransmit(0x2, 0x2, TestArray);
            a--;
            __delay_ms(500);
        }
    }
    //    LookUpTable_I2C[0].Receiver = 0x8;
    //    LookUpTable_I2C[0].TargetProtocol = APP_PROTOCOL_RS232;
    //   LookUpTable_I2C[0].TargetLocation = 0x64;
    //    LookUpTable_I2C_Elements = 1;

    //    LookUpTable_LIN[0].Receiver = 0x1;
    //    LookUpTable_LIN[0].TargetProtocol = APP_PROTOCOL_I2C;
    //    LookUpTable_LIN[0].TargetLocation = 0x8;
    //    LookUpTable_LIN_Elements = 1;

    //    LookUpTable_CAN[0].Receiver = 0x65;
    //    LookUpTable_CAN[0].TargetProtocol = APP_PROTOCOL_RS232;
    //    LookUpTable_CAN_Elements = 1;

    //    LookUpTable_RS232.TargetLocation = 0x2;
    //    LookUpTable_RS232.TargetProtocol = APP_PROTOCOL_LIN;

    //        LookUpTable_LIN[0].Receiver = 0x01;
    //        LookUpTable_LIN[0].TargetProtocol = 0b00001011;
    //        LookUpTable_LIN_Elements = 1;


    while (!DEBUG_ACTIVE)
    {
        uint8_t test[10];
        uint8_t test_cnt = 0;
        switch (Application_State)
        {
                /* The code under this case will execute while connected to the device that is configuring the protocol convertor */
            case APP_CONFIGURATION_STATE:
                GPIO_vSetPinLevel(CONVERSION_RUNNING_LED_PIN, STD_LOW);
                switch (ConfiguratorStates)
                {
                    case CONFIGURATOR_IDLE:

                        App_SPIReceivedData = SPI_uiSlaveExchangeByte(App_SPIReceivedData);
                        if ((App_SPIReceivedData & APP_START_BYTE_MASK) == APP_START_BYTE)
                        {
                            Configurator_targetProtocol = CONFIGURATOR_GET_PROTOCOL_ID(App_SPIReceivedData);
                            App_ProtocolsState[Configurator_targetProtocol] = CONFIGURATOR_GET_PROTOCOL_STATE(App_SPIReceivedData);
                            App_NumberOfConfigBytesReceived = 0;
                            ConfiguratorStates = CONFIGURATOR_STARTED;
                            Configurator_Response_Value++;

                        }
                        break;
                    case CONFIGURATOR_STARTED:
                        switch (Configurator_targetProtocol)
                        {
                            case APP_PROTOCOL_CAN:
                                for (App_NumberOfConfigBytesReceived = 0; App_NumberOfConfigBytesReceived < App_Protocol_NumberOfBytesToReceive[Configurator_targetProtocol]; App_NumberOfConfigBytesReceived++)
                                {
                                    App_SPIReceivedData = SPI_uiSlaveExchangeByte(App_SPIReceivedData);
                                    if (App_NumberOfConfigBytesReceived == 0u)
                                    {
                                        App_NumberOfLKTSToReceive = App_SPIReceivedData;
                                        LookUpTable_CAN_Elements = App_NumberOfLKTSToReceive;

                                    }
                                    else if (App_NumberOfConfigBytesReceived == 1u)
                                    {
                                        CAN_config.Module_BaudRate = App_SPIReceivedData;

                                        if (App_ProtocolsState[APP_PROTOCOL_CAN] == true)
                                        {

                                        }
                                    }
                                    Configurator_Response_Value++;
                                    if (App_NumberOfConfigBytesReceived == (App_Protocol_NumberOfBytesToReceive[Configurator_targetProtocol]) - 1)
                                    {
                                        Configurator_CurrentLKT = 0;
                                        ConfiguratorStates = CONFIGURATOR_LOOKUPS;
                                        Configurator_Response_Value = 3;
                                    }

                                }
                                break;
                            case APP_PROTOCOL_I2C:
                                /* continue here*/
                                for (App_NumberOfConfigBytesReceived = 0; App_NumberOfConfigBytesReceived < App_Protocol_NumberOfBytesToReceive[Configurator_targetProtocol]; App_NumberOfConfigBytesReceived++)
                                {
                                    App_SPIReceivedData = SPI_uiSlaveExchangeByte(App_SPIReceivedData);
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

                                }
                                break;
                            case APP_PROTOCOL_LIN:
                                for (App_NumberOfConfigBytesReceived = 0; App_NumberOfConfigBytesReceived < App_Protocol_NumberOfBytesToReceive[Configurator_targetProtocol]; App_NumberOfConfigBytesReceived++)
                                {
                                    App_SPIReceivedData = SPI_uiSlaveExchangeByte(App_SPIReceivedData);

                                    test[App_NumberOfConfigBytesReceived] = App_SPIReceivedData;
                                    if (App_NumberOfConfigBytesReceived == 0u)
                                    {
                                        App_NumberOfLKTSToReceive = App_SPIReceivedData;
                                        LookUpTable_LIN_Elements = App_NumberOfLKTSToReceive;
                                    }
                                    else if (App_NumberOfConfigBytesReceived == 1u)
                                    {
                                        LIN_actualConfig.configUartMode = MASK_8BIT_GET_LSB_HALF(App_SPIReceivedData);
                                        //                                        App_SPIReceivedData = MASK_8BIT_GET_MSB_HALF(App_SPIReceivedData);
                                        baudForConfig = MainApplication_uiLINGetBaudValue(MASK_8BIT_GET_MSB_HALF(App_SPIReceivedData));
                                        //                                        LIN_v
                                        LIN_vBaudCalculator(1, baudForConfig);
                                        LIN_actualConfig.configBaudValue = baudForConfig;

                                    }
                                    else if (App_NumberOfConfigBytesReceived == 2u)
                                    {

                                        LIN_actualConfig.configStopBitMode = GET_STOP_BIT_LIN(App_SPIReceivedData & 0x03);
                                        LIN_actualConfig.config_ChecksumMode = MASK_8BIT_GET_BIT(App_SPIReceivedData, 2);
                                        LIN_actualConfig.configTransmitPolarity = MASK_8BIT_GET_BIT(App_SPIReceivedData, 3);
                                        LIN_actualConfig.configBaudGeneratorSpeed = MASK_8BIT_GET_BIT(App_SPIReceivedData, 4);

                                        if (true == App_ProtocolsState[APP_PROTOCOL_LIN])
                                        {
                                            LIN_vInit(&LIN_actualConfig);
                                        }
                                    }
                                    Configurator_Response_Value++;
                                    if (App_NumberOfConfigBytesReceived == (App_Protocol_NumberOfBytesToReceive[Configurator_targetProtocol]) - 1)
                                    {
                                        Configurator_CurrentLKT = 0;
                                        ConfiguratorStates = CONFIGURATOR_LOOKUPS;
                                        Configurator_Response_Value = 3;
                                    }

                                }
                                break;
                            case APP_PROTOCOL_RS232:
                                for (App_NumberOfConfigBytesReceived = 0; App_NumberOfConfigBytesReceived < App_Protocol_NumberOfBytesToReceive[Configurator_targetProtocol]; App_NumberOfConfigBytesReceived++)
                                {
                                    App_SPIReceivedData = SPI_uiSlaveExchangeByte(App_SPIReceivedData);
                                    if (App_NumberOfConfigBytesReceived == 0u)
                                    {
                                        App_NumberOfLKTSToReceive = App_SPIReceivedData;
                                        LookUpTable_I2C_Elements = App_NumberOfLKTSToReceive;
                                    }
                                    else if (App_NumberOfConfigBytesReceived == 1u)
                                    {
                                        /* get bits [6:3]*/
                                        RS232_actualConfig.communicationDesiredBaud = MainApplication_uiLINGetBaudValue(App_SPIReceivedData >> 3);
                                        /* get bits [2:0]*/
                                        RS232_actualConfig.communicationUartMode = MASK_8BIT_GET_LSB_HALF(App_SPIReceivedData & 0x70);

                                    }
                                    else if (App_NumberOfConfigBytesReceived == 2u)
                                    {

                                        RS232_actualConfig.communicationStopBitMode = GET_STOP_BIT_LIN(App_SPIReceivedData);
                                        RS232_actualConfig.communicationHandshakeFlowControl = (App_SPIReceivedData >> 2) & 0x03;
                                        RS232_actualConfig.communicationPolarity = MASK_8BIT_GET_BIT(App_SPIReceivedData, 4);
                                        RS232_actualConfig.communicationBaudGenSpeed = MASK_8BIT_GET_BIT(App_SPIReceivedData, 5);

                                        if (true == App_ProtocolsState[APP_PROTOCOL_RS232])
                                        {
                                            RS232_vInit(&RS232_actualConfig);
                                        }
                                    }
                                    Configurator_Response_Value++;
                                    if (App_NumberOfConfigBytesReceived == (App_Protocol_NumberOfBytesToReceive[Configurator_targetProtocol]) - 1)
                                    {
                                        Configurator_CurrentLKT = 0;
                                        ConfiguratorStates = CONFIGURATOR_LOOKUPS;
                                        Configurator_Response_Value = 3;
                                    }

                                }
                                break;
                        }
                        break;
                    case CONFIGURATOR_LOOKUPS:
                        if (App_NumberOfLKTSToReceive == 0x00)
                        {
                            App_SPIReceivedData = SPI_uiSlaveExchangeByte(App_SPIReceivedData);
                            ConfiguratorStates = CONFIGURATOR_IDLE;
                            if (1 == END_OF_CONFIGURATION_BYTE(App_SPIReceivedData))
                            {
                                ConfiguratorStates = CONFIGURATOR_IDLE;
                                Application_State = APP_RUNNING_STATE;
                                SPI_vInit(MASTER_MODE);
                                GPIO_vSetPinLevel(CONVERSION_RUNNING_LED_PIN, STD_HIGH);
                            }
                            break;
                        }
                        for (Configurator_LKT_element_index = 0; Configurator_LKT_element_index < BYTES_PER_LKT; Configurator_LKT_element_index++)
                        {
                            switch (Configurator_LKT_element_index)
                            {
                                case 0:
                                    App_SPIReceivedData = SPI_uiSlaveExchangeByte(App_SPIReceivedData);
                                    Configurator_tempTable.Receiver = App_SPIReceivedData;
                                    break;
                                case 1:
                                    App_SPIReceivedData = SPI_uiSlaveExchangeByte(App_SPIReceivedData);

                                    if (Configurator_targetProtocol == APP_PROTOCOL_LIN)
                                    {
                                        LIN_vAddNewPID(Configurator_tempTable.Receiver, App_SPIReceivedData);
                                    }
                                    else
                                    {
                                        MASK_16BIT_SET_MSB_HALF(Configurator_tempTable.Receiver, App_SPIReceivedData);
                                    }
                                    break;
                                case 2:
                                    App_SPIReceivedData = SPI_uiSlaveExchangeByte(App_SPIReceivedData);
                                    Configurator_tempTable.TargetLocation = App_SPIReceivedData;
                                    break;
                                case 3:
                                    App_SPIReceivedData = SPI_uiSlaveExchangeByte(App_SPIReceivedData);
                                    MASK_16BIT_SET_MSB_HALF(Configurator_tempTable.TargetLocation, App_SPIReceivedData);
                                    break;
                                case 4:
                                    App_SPIReceivedData = SPI_uiSlaveExchangeByte(App_SPIReceivedData);
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
                                                //                                                Application_State = APP_RUNNING_STATE;

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
                                                //                                                Application_State = APP_RUNNING_STATE;
                                            }
                                            break;
                                        case APP_PROTOCOL_LIN:
                                            LookUpTable_LIN[Configurator_CurrentLKT].Request_DTBS_ptr = DataToBeSent_LIN;
                                            LookUpTable_LIN[Configurator_CurrentLKT].StateOfTheRequest = REQUEST_IDLE;
                                            LookUpTable_LIN[Configurator_CurrentLKT].Receiver = Configurator_tempTable.Receiver;
                                            LookUpTable_LIN[Configurator_CurrentLKT].TargetLocation = Configurator_tempTable.TargetLocation;
                                            LookUpTable_LIN[Configurator_CurrentLKT].TargetProtocol = Configurator_tempTable.TargetProtocol;
                                            Configurator_CurrentLKT++;
                                            if (Configurator_CurrentLKT >= App_NumberOfLKTSToReceive)
                                            {
                                                ConfiguratorStates = CONFIGURATOR_IDLE;
                                                //                                                Application_State = APP_RUNNING_STATE;
                                            }
                                            break;
                                        case APP_PROTOCOL_RS232:
                                            LookUpTable_RS232.Request_DTBS_ptr = DataToBeSent_RS232;
                                            LookUpTable_RS232.StateOfTheRequest = REQUEST_IDLE;
                                            LookUpTable_RS232.Receiver = Configurator_tempTable.Receiver;
                                            LookUpTable_RS232.TargetLocation = Configurator_tempTable.TargetLocation;
                                            LookUpTable_RS232.TargetProtocol = Configurator_tempTable.TargetProtocol;
                                            break;
                                            /*default:
                                                Configurator_CurrentLKT++;
                                                if (Configurator_CurrentLKT >= App_NumberOfLKTSToReceive)
                                                {
                                                    ConfiguratorStates = CONFIGURATOR_IDLE;
                                                    Application_State = APP_RUNNING_STATE;
                                                }
                                                break;*/

                                    }

                                    break;
                            }
                            if (1 == END_OF_CONFIGURATION_BYTE(App_SPIReceivedData))
                            {
                                ConfiguratorStates = CONFIGURATOR_IDLE;
                                Application_State = APP_RUNNING_STATE;
                                SPI_vInit(MASTER_MODE);
                                GPIO_vSetPinLevel(CONVERSION_RUNNING_LED_PIN, STD_HIGH);
                                break;
                            }
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
                U2ERRIR &= 0x10;
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
    LIN_packet * linReceivedPacket;
    CAN_Buffer * target_CAN_buffer;
    CAN_Frame frameToBeSent;
    MainApplication_LookUpTable * targetTable;
    I2C_SlaveOperationType slaveResponseStatus = I2C_vSlaveMainFunction(&receivedData, (uint16_t*) & matchedAdrr, &I2C_IsAStop);
    /* This is used to convert data received from I2C_Slave Receiver to the corespondent protocol according to the LookUpTable*/
    if (App_ProtocolsState[APP_PROTOCOL_I2C] == true)
    {
        switch (slaveResponseStatus)
        {
            case I2C_NEW_DATA_RECEIVED:
                targetTable = MainApplication_uiCheckLookUpTable(APP_PROTOCOL_I2C, &matchedAdrr);
                if (targetTable != NULL)
                {
                    if ((true == I2C_IsAStop))
                    {
                        MainApplication_vAddDataToBeSent(targetTable->TargetProtocol, &receivedData, &targetTable->TargetLocation, true);
                    }
                    else
                    {
                        MainApplication_vAddDataToBeSent(targetTable->TargetProtocol, &receivedData, &targetTable->TargetLocation, false);
                    }
                    DataLogger_vSendDataReceived(matchedAdrr, APP_PROTOCOL_I2C, &receivedData, 1);
                }
                else
                {
                    /* 1 - one data byte */
                    DataLogger_vSendDestinationNotFound(matchedAdrr, APP_PROTOCOL_I2C, &receivedData, 1);
                }
                break;
            case I2C_DATA_REQUESTED:
                targetTable = MainApplication_uiCheckLookUpTable(APP_PROTOCOL_I2C, &matchedAdrr);
                if (targetTable != NULL)
                {
                    switch (targetTable->StateOfTheRequest)
                    {
                        case REQUEST_IDLE:
                            targetTable->Request_DTBS_ptr = MainApplication_CreateDataToBeSentSlot(targetTable->TargetProtocol, 0, &targetTable->TargetLocation);
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
                                case APP_PROTOCOL_LIN:
                                    //                                LIN_vtr
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
                        case REQUEST_DATA_READY:

                            break;
                    }
                }
                else
                {
                    /* 1 - one data byte */
                    DataLogger_vSendDestinationNotFound(matchedAdrr, APP_PROTOCOL_I2C, &receivedData, 0);
                }
                break;
            case I2C_NO_NEW_DATA:
                break;
        }
    }

    /* This is the CAN part */
    if (true == App_ProtocolsState[APP_PROTOCOL_CAN])
    {
        for (bufferIndex = 0; bufferIndex < numberOfReceiveBuffers_CAN; bufferIndex++)
        {
            target_CAN_buffer = CAN_uiGetBufferAdrress(&bufferIndex);
            /* Checking for new data from CAN bus*/
            if (CAN_bBufferHasNewData(target_CAN_buffer) == 1)
            {
                matchedAdrr = CAN_uiGetRxBufferIdentifier(&bufferIndex);
                targetTable = MainApplication_uiCheckLookUpTable(APP_PROTOCOL_CAN, &matchedAdrr);
                if (targetTable != NULL)
                {
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
                                        DataLogger_vSendDataRequested(matchedAdrr, APP_PROTOCOL_CAN, targetTable->TargetLocation, APP_PROTOCOL_I2C);

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

                                        DataLogger_vSendDataConverted(matchedAdrr, APP_PROTOCOL_CAN, frameToBeSent.Frame_DataBytes, frameToBeSent.Frame_DataLength);
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
                        DataLogger_vSendDataReceived(matchedAdrr, APP_PROTOCOL_CAN, target_CAN_buffer->BxData, target_CAN_buffer->BxDLC);
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
                else
                {
                    CAN_vSetBufferAsFree(target_CAN_buffer);
                    if (matchedAdrr != CAN_DataRequestedFromIdentifier)
                    {
                        if (target_CAN_buffer->BxDLC > CAN_MAX_DATA_BYTES)
                        {
                            DataLogger_vSendDestinationNotFound(matchedAdrr, APP_PROTOCOL_CAN, target_CAN_buffer->BxData, 0);
                        }
                        else
                        {
                            DataLogger_vSendDestinationNotFound(matchedAdrr, APP_PROTOCOL_CAN, target_CAN_buffer->BxData, target_CAN_buffer->BxDLC);
                        }
                    }
                }
                /* This checks if data was requested from CAN */
                /*else */
            }
        }
    }
    /* This is the LIN part */
    if (true == App_ProtocolsState[APP_PROTOCOL_LIN])
    {
        if (LIN_bNewPacketAvailable() == 1)
        {
            linReceivedPacket = LIN_GetPacket();
            targetTable = MainApplication_uiCheckLookUpTable(APP_PROTOCOL_LIN, (uint32_t*) & linReceivedPacket->pid);
            DataLogger_vSendDataReceived(linReceivedPacket->pid, APP_PROTOCOL_LIN, linReceivedPacket->data, linReceivedPacket->noOfBytes);
            for (index = 0; index < linReceivedPacket->noOfBytes; index++)
            {
                if (index == (linReceivedPacket->noOfBytes - 1))
                {
                    MainApplication_vAddDataToBeSent(targetTable->TargetProtocol, &linReceivedPacket->data[index], &targetTable->TargetLocation, true);
                }
                else
                {
                    MainApplication_vAddDataToBeSent(targetTable->TargetProtocol, &linReceivedPacket->data[index], &targetTable->TargetLocation, false);
                }
            }
        }
        else if (LIN_bDataWasRequested((uint32_t *) & matchedAdrr) == 1)
        {
            targetTable = MainApplication_uiCheckLookUpTable(APP_PROTOCOL_LIN, &matchedAdrr);
            if (targetTable != NULL)
            {
                switch (targetTable->StateOfTheRequest)
                {
                    case REQUEST_IDLE:
                        targetTable->Request_DTBS_ptr = MainApplication_CreateDataToBeSentSlot(targetTable->TargetProtocol, LIN_GET_NUMBER_OF_BYTES_FROM_LKT(MASK_16BIT_GET_MSB_HALF(targetTable->Receiver)), &targetTable->TargetLocation);
                        targetTable->StateOfTheRequest = REQUEST_DATA_PENDING;
                        switch (targetTable->TargetProtocol)
                        {
                            case APP_PROTOCOL_I2C:
                                I2C_vMasterRead(targetTable->TargetLocation, targetTable->Request_DTBS_ptr->NextIndex, targetTable->Request_DTBS_ptr->Data);
                                DataLogger_vSendDataRequested(matchedAdrr, APP_PROTOCOL_LIN, targetTable->TargetLocation, APP_PROTOCOL_I2C);

                                targetTable->StateOfTheRequest = REQUEST_DATA_READY;
                                /* This is set to pending so that the received data will not be sent back */
                                targetTable->Request_DTBS_ptr->DataState = DATA_PENDING;

                                LIN_SetDataForResponse(matchedAdrr, targetTable->Request_DTBS_ptr->Data, LIN_GET_NUMBER_OF_BYTES_FROM_LKT(MASK_16BIT_GET_MSB_HALF(targetTable->Receiver)));

                                targetTable->StateOfTheRequest = REQUEST_IDLE;
                                break;
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
                                    LIN_SetDataForResponse(matchedAdrr, targetTable->Request_DTBS_ptr->Data, LIN_GET_NUMBER_OF_BYTES_FROM_LKT(MASK_16BIT_GET_MSB_HALF(targetTable->Receiver)));

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
                    case REQUEST_DATA_READY:
                        /* Here can be implemented the response based on "cycles" so that the code executes the instructions
                         on a number o cycles
                         */

                        break;
                }
            }
        }
    }

    /* This is the RS-232 part */
    if (true == App_ProtocolsState[APP_PROTOCOL_RS232])
    {
        if (RS232_bReceiveDataReady() == 1)
        {
            receivedData = RS232_uiReceiveData();
            if (LookUpTable_RS232.TargetProtocol != APP_PROTOCOL_UNKNOWN)
            {
                targetTable = &LookUpTable_RS232;
                /* 1 data byte */
                DataLogger_vSendDataReceived(0x00, APP_PROTOCOL_RS232, &receivedData, 1);
                MainApplication_vAddDataToBeSent(targetTable->TargetProtocol, &receivedData, &targetTable->TargetLocation, true);
            }
            else
            {
                DataLogger_vSendDestinationNotFound(0x00, APP_PROTOCOL_RS232, &receivedData, 1);
            }
        }
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
            if (targetIndex != VALUE_FOR_LKT_NOT_FOUND_BINARY_SRC)
            {
                returnValue = &LookUpTable_CAN[targetIndex];
            }
            break;
        case APP_PROTOCOL_I2C:
            targetIndex = BasicOperations_BinarySearch(LookUpTable_I2C, 0, (LookUpTable_I2C_Elements - 1), *targetReceiver, 0);
            if (targetIndex != VALUE_FOR_LKT_NOT_FOUND_BINARY_SRC)
            {
                returnValue = &LookUpTable_I2C[targetIndex];
            }
            break;
        case APP_PROTOCOL_LIN:
            targetIndex = BasicOperations_BinarySearch(LookUpTable_LIN, 0, (LookUpTable_LIN_Elements - 1), *targetReceiver, 0);
            if (targetIndex != VALUE_FOR_LKT_NOT_FOUND_BINARY_SRC)
            {
                returnValue = &LookUpTable_LIN[targetIndex];
            }
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
                DataLogger_vSendDataConverted(*targetAdress, protocol, DataToBeSent_I2C[DataToBeSent_I2C_StackPointer].Data, (nextIndex + 1));
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
            nextIndex = DataToBeSent_LIN[DataToBeSent_LIN_StackPointer].NextIndex;
            DataToBeSent_LIN[DataToBeSent_LIN_StackPointer].Data[nextIndex] = *dataToBeSend;
            DataToBeSent_LIN[DataToBeSent_LIN_StackPointer].TargetLocation = *targetAdress;
            if (true == lastByte)
            {
                DataToBeSent_LIN[DataToBeSent_LIN_StackPointer].DataState = DATA_READY;
                //                DataToBeSent_RS232[DataToBeSent_RS232_StackPointer].ReadyForSending = true;
                //                DataToBeSent_RS232[DataToBeSent_RS232_StackPointer].DataWasSent = false;
                //                DataLogger_vSendDataConverted(*targetAdress, protocol, DataToBeSent_RS232[DataToBeSent_RS232_StackPointer].Data, (nextIndex + 1));
                DataToBeSent_LIN_StackPointer++;
                if (DataToBeSent_LIN_StackPointer >= DTBS_RS232_FIFO_SIZE)
                {
                    DataToBeSent_LIN_StackPointer = 0;
                }
            }
            else
            {
                DataToBeSent_LIN[DataToBeSent_LIN_StackPointer].NextIndex++;
                DataToBeSent_LIN[DataToBeSent_LIN_StackPointer].DataState = DATA_PENDING;
                //                DataToBeSent_RS232[DataToBeSent_RS232_StackPointer].ReadyForSending = false;
                //                DataToBeSent_RS232[DataToBeSent_RS232_StackPointer].DataWasSent = false;
            }
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
                //                DataLogger_vSendDataConverted(*targetAdress, protocol, DataToBeSent_RS232[DataToBeSent_RS232_StackPointer].Data, (nextIndex + 1));
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
    if (true == App_ProtocolsState[APP_PROTOCOL_CAN])
    {
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
    }
    /* This is for sending via I2C */
    //    for (index = 0; index < DataToBeSent_I2C_StackPointer; index++)
    //    {
    if (true == App_ProtocolsState[APP_PROTOCOL_I2C])
    {
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
    }

    if (true == App_ProtocolsState[APP_PROTOCOL_LIN])
    {
        /* This if for sending data on LIN */
        if (DataToBeSent_LIN[DataToBeSent_LIN_ReadPointer].DataState == DATA_READY)
        {
            if (LIN_actualConfig.configUartMode == LIN_MASTER)
            {
                DataLogger_vSendDataConverted(DataToBeSent_LIN[DataToBeSent_LIN_ReadPointer].TargetLocation, APP_PROTOCOL_LIN, DataToBeSent_LIN[DataToBeSent_LIN_ReadPointer].Data, (DataToBeSent_LIN[DataToBeSent_LIN_ReadPointer].NextIndex + 1));
                LIN_vTransmit(DataToBeSent_LIN[DataToBeSent_LIN_ReadPointer].TargetLocation, DataToBeSent_LIN[DataToBeSent_LIN_ReadPointer].NextIndex, DataToBeSent_LIN[DataToBeSent_LIN_ReadPointer].Data);
                //        LIN_vTransmit(DataToBeSent_LIN[DataToBeSent_LIN_ReadPointer].TargetLocation, DataToBeSent_LIN[DataToBeSent_LIN_ReadPointer].NextIndex, DataToBeSent_LIN[DataToBeSent_LIN_ReadPointer].Data);
                DataToBeSent_LIN[DataToBeSent_LIN_ReadPointer].DataState = DATA_WAS_SENT;
                DataToBeSent_LIN[DataToBeSent_LIN_ReadPointer].NextIndex = 0;
                DataToBeSent_LIN_ReadPointer++;
                if (DataToBeSent_LIN_ReadPointer == DTBS_LIN_FIFO_SIZE)
                {
                    DataToBeSent_LIN_ReadPointer = 0;
                }
            }
            else if (LIN_actualConfig.configUartMode == LIN_SLAVE)
            {
                DataLogger_vSendDataConverted(DataToBeSent_LIN[DataToBeSent_LIN_ReadPointer].TargetLocation, APP_PROTOCOL_LIN, DataToBeSent_LIN[DataToBeSent_LIN_ReadPointer].Data, (DataToBeSent_LIN[DataToBeSent_LIN_ReadPointer].NextIndex + 1));
                LIN_SetDataForResponse(DataToBeSent_LIN[DataToBeSent_LIN_ReadPointer].TargetLocation, DataToBeSent_LIN[DataToBeSent_LIN_ReadPointer].Data, DataToBeSent_LIN[DataToBeSent_LIN_ReadPointer].NextIndex + 1);
                //        LIN_vTransmit(DataToBeSent_LIN[DataToBeSent_LIN_ReadPointer].TargetLocation, DataToBeSent_LIN[DataToBeSent_LIN_ReadPointer].NextIndex, DataToBeSent_LIN[DataToBeSent_LIN_ReadPointer].Data);
                DataToBeSent_LIN[DataToBeSent_LIN_ReadPointer].DataState = DATA_WAS_SENT;
                DataToBeSent_LIN[DataToBeSent_LIN_ReadPointer].NextIndex = 0;
                DataToBeSent_LIN_ReadPointer++;
                if (DataToBeSent_LIN_ReadPointer == DTBS_LIN_FIFO_SIZE)
                {
                    DataToBeSent_LIN_ReadPointer = 0;
                }
            }
        }
    }

    if (true == App_ProtocolsState[APP_PROTOCOL_RS232])
    {
        /* This is for sending data on RS232 */
        if (DataToBeSent_RS232[DataToBeSent_RS232_ReadPointer].DataState == DATA_READY)
        {
            DataLogger_vSendDataConverted(0x00, APP_PROTOCOL_RS232, DataToBeSent_RS232[DataToBeSent_RS232_ReadPointer].Data, (DataToBeSent_RS232[DataToBeSent_RS232_ReadPointer].NextIndex + 1));
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
    DataLogger_vSendDataConverted(frame.Frame_Identifier, APP_PROTOCOL_CAN, frame.Frame_DataBytes, frame.Frame_DataLength);
}

BaudValue MainApplication_uiLINGetBaudValue(uint8_t indexOfBaud)
{
    BaudValue returnValue;
    switch (indexOfBaud)
    {
        case 0:
            returnValue = BAUD_110;
            break;
        case 1:
            returnValue = BAUD_150;
            break;
        case 2:
            returnValue = BAUD_300;
            break;
        case 3:
            returnValue = BAUD_1200;
            break;
        case 4:
            returnValue = BAUD_2400;
            break;
        case 5:
            returnValue = BAUD_4800;
            break;
        case 6:
            returnValue = BAUD_9600;
            break;
        case 7:
            returnValue = BAUD_19200;
            break;
        case 8:
            returnValue = BAUD_38400;
            break;
        case 9:
            returnValue = BAUD_57600;
            break;
        case 10:
            returnValue = BAUD_115200;
            break;
        case 11:
            returnValue = BAUD_230400;
            break;
        case 12:
            returnValue = BAUD_460800;
            break;
    }
    return returnValue;
}

uint8_t BasicOperations_BinarySearch(MainApplication_LookUpTable * table, uint8_t startIndex, uint8_t endIndex, uint8_t searchedValue, bool searchBy)
{
    uint8_t middle;
    if (endIndex != 255)
    {
        switch (searchBy)
        {
                /* Searching by receiver if searchBy == 0 */
            case 0:

                while (startIndex <= endIndex)
                {
                    /* Right shifting with one place means division by two*/
                    middle = startIndex + ((endIndex - startIndex) >> 1);
                    if (table[middle].Receiver == searchedValue)
                        return middle;

                    if (table[middle].Receiver < searchedValue)
                    {
                        startIndex = middle + 1;
                    }
                    else
                    {
                        if (middle != 0)
                        {
                            endIndex = middle - 1;
                        }
                        else
                        {
                            return VALUE_FOR_LKT_NOT_FOUND_BINARY_SRC;
                        }
                    }
                }
                break;
                /* Searching by TargetLocation if searchBy == 1 */
            case 1:
                while (startIndex <= endIndex)
                {
                    /* Right shifting with one place means division by two*/
                    middle = startIndex + ((endIndex - startIndex) >> 1);
                    if (table[middle].TargetLocation == searchedValue)
                        return middle;

                    if (table[middle].TargetLocation < searchedValue)
                        startIndex = middle + 1;
                    else
                    {
                        if (middle != 0)
                        {
                            endIndex = middle - 1;
                        }
                        else
                        {
                            return VALUE_FOR_LKT_NOT_FOUND_BINARY_SRC;
                        }
                    }
                }
                break;
        }
    }
    /* return 255 if the element is not found */
    return VALUE_FOR_LKT_NOT_FOUND_BINARY_SRC;
}

/**
 End of File
 */