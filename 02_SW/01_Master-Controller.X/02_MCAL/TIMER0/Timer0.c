/**
 * \file       Time0.c
 * \author     Iosub Mihai Alexandru
 * \brief      This is the driver for the Timer 0 module for PIC18F26K83
 */

/*----------------------------------------------------------------------------*/
/*                                 Includes                                   */
/*----------------------------------------------------------------------------*/
#include "Timer0.h"
/*----------------------------------------------------------------------------*/
/*                               Local defines                                */
/*----------------------------------------------------------------------------*/
#define CLK_SOURCE_POS          (5u)
#define MD16_POSITION           (4u)
#define ENABLE_POSITION         (7u)
#define TIMER_SET_16BIT_MODE()  MASK_8BIT_SET_BIT(T0CON0, MD16_POSITION)
#define TIMER_SET_8BIT_MODE()   MASK_8BIT_CLEAR_BIT(T0CON0, MD16_POSITION)
#define TIMER_ENABLE()          MASK_8BIT_SET_BIT(T0CON0, ENABLE_POSITION)
#define TIMER_DISABLE()         MASK_8BIT_CLEAR_BIT(T0CON0, ENABLE_POSITION)
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
void Timer0_vInit(void)
{
    /*
     * bit 7 EN: TMR0 Enable bit; 0 = 0 = The module is disabled and in the lowest power mode;
     * bit 6-5 Unimplemented / RO
     * bit 4 MD16 = 1: TMR0 Operating as 16-Bit Timer
     * bit 3-0 OUTPS<3:0>: TMR0 Output Postscaler (Divider) Select bits - 0000 = 1:1 Postscaler*/
    T0CON0 = 0x10;
    /* bit 7-5 CS<2:0>:Timer0 Clock Source Select bits; 011 = HFINTOSC
     * bit 4 ASYNC: TMR0 Input Asynchronization Enable bit; 0 = The input to the TMR0 counter is synchronized to FOSC/4
     * bit 3-0 CKPS<3:0>: Prescaler Rate Select bit; 0110 = 1:64 => 1Mhz
     */
    T0CON1 = 0x66;
    TMR0L = 0x00;
    TMR0H = 0x00;
}

void Timer0_vStart(Timer_Configuration config)
{
    uint8_t a;
    if (TIMER_8BIT_MODE == config.opMode)
    {
        T0CON0 = (config.postscalerValue - 1u);
        TIMER_SET_8BIT_MODE();

        /* T0CON1 */
        T0CON1 = ((config.clockSourceValue << CLK_SOURCE_POS) | MASK_8BIT_GET_LSB_HALF(config.prescalerPower));
        TMR0L = config.startValue;
        TMR0H = config.endValue;
    }
    else if (TIMER_16BIT_MODE == config.opMode)
    {
        T0CON0 = (config.postscalerValue - 1u);
        TIMER_SET_16BIT_MODE();

        /* T0CON1 */
        T0CON1 = ((config.clockSourceValue << CLK_SOURCE_POS) | MASK_16BIT_GET_LSB_HALF(config.prescalerPower));
        /* Always write TMR0H first because of the buffer register - See FIGURE 20-1: BLOCK DIAGRAM OF TIMER0 */
        a = MASK_16BIT_GET_MSB_HALF(config.startValue);
        TMR0H = MASK_16BIT_GET_MSB_HALF(config.startValue);
        a = MASK_16BIT_GET_LSB_HALF(config.startValue);
        TMR0L = MASK_16BIT_GET_LSB_HALF(config.startValue);

    }
    TIMER_ENABLE();
}

void Timer0_vStop()
{
    TIMER_DISABLE();
}

bool Timer0_bWasOverflow()
{
    bool returnValue = false;
    if (PIR3bits.TMR0IF == 1)
    {
        returnValue = true;
        PIR3bits.TMR0IF = 0;
    }
    return returnValue;
}
/*----------------------------------------------------------------------------*/
/*                     Implementation of local functions                      */
/*----------------------------------------------------------------------------*/
