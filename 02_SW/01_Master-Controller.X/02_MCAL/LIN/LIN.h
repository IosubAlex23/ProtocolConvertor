#ifndef LIN_H_
#define LIN_H_

/**
 * \file       Head.h
 * \author     Ioan Nicoara
 * \brief      Short description for this header file
 */


/*----------------------------------------------------------------------------*/
/*                                  Includes                                  */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*                             Defines and macros                             */
/*----------------------------------------------------------------------------*/
#define CHECKSUM_BIT        (3u)    //represents bit no.3 from U2CON2 Register
#define BREAK_BIT           (0u)      //represents bit no.0 from U2CON1 Register
#define PID_MASK            (0x3F)  //used for extract identifier from PID

typedef enum {
    LEGACY = 0u,
    ENHANCED,
} CheckSUM_Mode;

typedef enum {
    LIN_RX_IDLE,
    LIN_RX_BREAK,
    LIN_RX_SYNC,
    LIN_RX_DATA,
    LIN_RX_PID,
    LIN_RX_CHECKSUM,
} LIN_RX_State;

typedef enum {
    Publisher,
    Subscriber,
} Mode;

typedef struct {
    uint8_t pid;
    uint8_t data[8];
    uint8_t checksum;
    uint8_t noOfBytes;

} LIN_packet;

typedef enum {
    LIN_RESPONSE_IDLE = 0u,
    LIN_RESPONSE_DATA_REQUESTED,
    LIN_RESPONSE_DATA_READY,
} LIN_SlaveResponseStatus;

typedef struct {
    uint8_t pid;
    uint8_t noOfBytes;
    uint8_t dataForResponse[8];
    uint8_t dataPendingValue[8];

    LIN_SlaveResponseStatus dataForResponseStatus;
    Mode type;

} PID_Description;

typedef struct {
    UartMode configUartMode;
    BaudGeneratorSpeed configBaudGeneratorSpeed;
    BaudValue configBaudValue;
    TransmitPolarity configTransmitPolarity;
    StopBitMode configStopBitMode;
    CheckSUM_Mode config_ChecksumMode;

} LIN_Configuration;
/*----------------------------------------------------------------------------*/
/*                                 Data types                                 */
/*----------------------------------------------------------------------------*/

LIN_Configuration LIN_actualConfig;


/*----------------------------------------------------------------------------*/
/*                 External declaration of global RAM-Variables               */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*                 External declaration of global ROM-Variables               */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*                  External declaration of global functions                  */
/*----------------------------------------------------------------------------*/

/**
 * \brief     This function initialize the LIN Module;
 * \param     * Config - represents the configuration wich to be applied on the module
 * \return    None 
 */
void LIN_vInit(LIN_Configuration * Config);

/**
 * \brief     This function is used for transmit data on LIN Protocol;
 * \param     identifier - represents message identifier from LIN network
 *            NoOfBytes - represents number of bytes to be transmit
 *            * data - is a pointer that indicates the source of the data
 * \return    None 
 */
void LIN_vTransmit(uint8_t identifier, uint8_t NoOfBytes, uint8_t *data);

/**
 * \brief     This function is used to get last packet of the data received on LIN bus;
 * \param     
 * \return    returnValue  - represents the last packet received 
 */
LIN_packet * LIN_GetPacket();

/**
 * \brief     This function is used to add new pid in lookup table;
 * \param     pid - represents message identifier from LIN network
 *            noOfDataBytes - represents number of the which are transferred on a specific PID
 * \return     - 
 */
void LIN_vAddNewPID(uint8_t pid, uint8_t noOfDataBytes);

/**
 * \brief     This function is used for Slave response at a master header
 * \param     pid - represents message identifier from LIN network
 *            * data - is a pointer that indicates the source of the data
 *            noOfDataBytes - represents number of the which are transferred on a specific PID
 * \return     - 
 */
void LIN_SetDataForResponse(uint8_t pid, uint8_t *data, uint8_t noOfDataBytes);

/**
 * \brief     This function is used to check if there  is a new packet available
 * \param     pid - represents message identifier from LIN network
 *            * data - is a pointer that indicates the source of the data
 *            noOfDataBytes - represents number of the which are transferred on a specific PID
 * \return     - 
 */
bool LIN_bNewPacketAvailable(void);

bool LIN_bDataWasRequested(uint8_t * matchedPID);

#endif /* HEAD_H_ */
