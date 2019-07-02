/**
 * \file       Source.c
 * \author     Iosub Mihai Alexandru
 * \brief      Short description for this source file
 */

/*----------------------------------------------------------------------------*/
/*                                 Includes                                   */
/*----------------------------------------------------------------------------*/
#include "Interrupt.h"
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
void INTERRUPT_Initialize(void)
{
    INTCON0 = 0x00;
    INTCON0bits.GIEH = 1; // Enable high priority interrupts
    INTCON0bits.GIEL = 1; // Enable low priority interrupts
    INTCON0bits.IPEN = 1; // Enable interrupt priority
    /*PIE2bits.SPI1IE = 1; // Enable spi interrupt
    PIE2bits.SPI1RXIE = 1; // Enable SPI interrupt*/
}

void INTERRUPT_GlobalInterruptDisable(void)
{
    INTCON0bits.GIEH = 0; // Enable high priority interrupts
    INTCON0bits.GIEL = 0; // Enable low priority interrupts
    INTCON0bits.IPEN = 0; // Enable interrupt priority
}

void INTERRUPT_GlobalInterruptEnable(void)
{
    INTCON0bits.GIEH = 1; // Enable high priority interrupts
    INTCON0bits.GIEL = 1; // Enable low priority interrupts
    INTCON0bits.IPEN = 1; // Enable interrupt priority
}
/*----------------------------------------------------------------------------*/
/*                     Implementation of local functions                      */
/*----------------------------------------------------------------------------*/
