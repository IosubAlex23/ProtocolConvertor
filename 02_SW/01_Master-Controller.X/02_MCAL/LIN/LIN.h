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
/*----------------------------------------------------------------------------*/
/*                                 Data types                                 */
/*----------------------------------------------------------------------------*/
typedef enum
{
    LEGACY = 0u,
    ENHANCED,
}CheckSUM_Mode;

typedef enum
{
    LIN_RX_IDLE,
    LIN_RX_BREAK,
    LIN_RX_SYNC,
    LIN_RX_DATA,
    LIN_RX_PID,
    LIN_RX_CHECKSUM,
}LIN_RX_State;

typedef struct
{
    uint8_t pid;
    uint8_t data[8];
    uint8_t checksum;
}LIN_packet;

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
 * \param     None
 * \return    None 
 */
void LIN_vInit(uint8_t mode);

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

void LIN_stateCheck(void);

void LIN_vTransmit(uint8_t identifier, uint8_t NoOfBytes, uint8_t *data);


#endif /* HEAD_H_ */
