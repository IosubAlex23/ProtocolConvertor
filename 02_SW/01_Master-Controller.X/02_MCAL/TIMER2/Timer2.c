/**
 * \file       Timer2.c
 * \author     Iosub Mihai Alexandru
 * \brief      This is the driver for the Timer 0 module for PIC18F26K83
 */

/*----------------------------------------------------------------------------*/
/*                                 Includes                                   */
/*----------------------------------------------------------------------------*/
#include "Timer2.h"
/*----------------------------------------------------------------------------*/
/*                               Local defines                                */
/*----------------------------------------------------------------------------*/

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

/*----------------------------------------------------------------------------*/
/*                       Declaration of local functions                       */
/*----------------------------------------------------------------------------*/
/**
 * \brief     This function [...];
 * \param     None
 * \return    None 
 */
/*----------------------------------------------------------------------------*/
/*                     Implementation of global functions                     */

/*----------------------------------------------------------------------------*/
void Timer2_vInit(Timer2_OutputFrequency targetFrequency)
{
    // T2CS HFINTOSC; 
    T2CLKCON = 0x03;

    // T2PSYNC Not Synchronized; T2MODE Software control; T2CKPOL Rising Edge; T2CKSYNC Synchronized; 
    T2HLT = 0x20;

    // T2RSEL T2CKIPPS pin; 
    T2RST = 0x00;

    // TMR2 0; 
    T2TMR = 0x00;

    // Clearing IF flag.
    PIR4bits.TMR2IF = 0;

    T2CON = 0x00;
    switch (targetFrequency)
    {
        case TMR_100kHz:
            T2CON = 0x20;
            T2PR = 0xA0;
            break;
        case TMR_200kHz:
            T2CON = 0x20;
            T2PR = 0x50;
            break;
        case TMR_300kHz:
            T2PR = 0xD5; // 100kHz
            break;
        case TMR_400kHz:
            T2PR = 0xA0; // 250kHz out on I2C
            break;
        case TMR_500kHz:
            T2PR = 0x80; // 400khz out on I2C
            break;
        case TMR_600kHz:
            T2PR = 0x6A;
            break;
        case TMR_700kHz:
            T2PR = 0x5B;
            break;
        case TMR_800kHz:
            T2PR = 0x50;
            break;
        case TMR_900kHz:
            T2PR = 0x47;
            break;
        case TMR_1000kHz:
            T2PR = 0x40;
            break;
        case TMR_1100kHz:
            T2PR = 0x3A;
            break;
        case TMR_1200kHz:
            T2PR = 0x35;
            break;
        case TMR_1300kHz:
            T2PR = 0x31;
            break;
        case TMR_1400kHz:
            T2PR = 0x2D;
            break;
        case TMR_1500kHz:
            T2PR = 0x2A;
            break;
        case TMR_1600kHz:
            T2PR = 0x28;
            break;
    }
}

void Timer2_vStart(void)
{
    T2CON |= 0x80;
}

void Timer2_vStop(void)
{
    T2CON &= 0xEF;
}

bool Timer2_bWasOverflow(void)
{
    bool returnValue = false;
    if (PIR4bits.TMR2IF == 1)
    {
        returnValue = true;
        PIR4bits.TMR2IF = 0;
    }
    return returnValue;
}
/*----------------------------------------------------------------------------*/
/*                     Implementation of local functions                      */
/*----------------------------------------------------------------------------*/
