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
    // T2CS FOSC; 
    T2CLKCON = 0x02;

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
            T2CLKCON = 0x01;
            T2PR = 0x77;
            break;
        case TMR_200kHz:
            T2CLKCON = 0x01;
            T2PR = 0x3B;
            break;
        case TMR_300kHz:
            T2CLKCON = 0x01;
            T2PR = 0x27;
            break;
        case TMR_400kHz:
            T2CLKCON = 0x01;
            T2PR = 0x1D;
            break;
        case TMR_500kHz:
            T2CLKCON = 0x01;
            T2PR = 0x17;
            break;
        case TMR_600kHz:
            T2CLKCON = 0x01;
            T2PR = 0x13;
            break;
        case TMR_700kHz:
            T2PR = 0x44;
            break;
        case TMR_800kHz:
            T2PR = 0x3B;
            break;
        case TMR_900kHz:
            T2PR = 0x34;
            break;
        case TMR_1000kHz:
            T2PR = 0x2f;
            break;
        case TMR_1100kHz:
            T2PR = 0x2B;
            break;
        case TMR_1200kHz:
            T2PR = 0x27;
            break;
        case TMR_1300kHz:
            T2PR = 0x24;
            break;
        case TMR_1400kHz:
            T2PR = 0x21;
            break;
        case TMR_1500kHz:
            T2PR = 0x1F;
            break;
        case TMR_1600kHz:
            T2PR = 0x1D;
            break;
    }
    Timer2_vStart();
}

void Timer2_vStart(void)
{
    while (T2CON != 0x80)
    {
        T2CON |= 0x80;
    }
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
