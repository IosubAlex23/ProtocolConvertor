/**
 * \file       GPIO.c
 * \author     Iosub Mihai Alexandru
 * \brief      Short description for this source file
 */

/*----------------------------------------------------------------------------*/
/*                                 Includes                                   */
/*----------------------------------------------------------------------------*/
#include <stdlib.h>

#include "GPIO.h"

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
void GPIO_vSetPinOpenDrain(GPIO_tPortPin targetPin, bool openDrainStatus)
{
    uint8_t port = MASK_8BIT_GET_MSB_HALF(targetPin);
    uint8_t pin = MASK_8BIT_GET_LSB_HALF(targetPin);
    switch (port)
    {
        case 0xA:
            if (true == openDrainStatus)
            {
                /* Setting a bit in PORTx will drive the pin high */
                MASK_8BIT_SET_BIT(ODCONA, pin);
            }
            else
            {
                /* Clearing a bit in PORTx will drive the pin low */
                MASK_8BIT_CLEAR_BIT(ODCONA, pin);
            }
            break;
        case 0xB:
            if (true == openDrainStatus)
            {
                /* Setting a bit in PORTx will drive the pin high */
                MASK_8BIT_SET_BIT(ODCONB, pin);
            }
            else
            {
                /* Clearing a bit in PORTx will drive the pin low */
                MASK_8BIT_CLEAR_BIT(ODCONB, pin);
            }
            break;
        case 0xC:
            if (true == openDrainStatus)
            {
                /* Setting a bit in PORTx will drive the pin high */
                MASK_8BIT_SET_BIT(ODCONC, pin);
            }
            else
            {
                /* Clearing a bit in PORTx will drive the pin low */
                MASK_8BIT_CLEAR_BIT(ODCONC, pin);
            }
            break;
    }
}
void GPIO_vSetPinDirection(GPIO_tPortPin targetPin, GPIO_tPinDirectionType pinDirection)
{
    uint8_t port = MASK_8BIT_GET_MSB_HALF(targetPin);
    uint8_t pin = MASK_8BIT_GET_LSB_HALF(targetPin);
    switch (port)
    {
        case 0xA:
            if (GPIO_OUTPUT_PIN == pinDirection)
            {
                /* Clearing a bit in TRISx will make the pin direction to be output */
                MASK_8BIT_CLEAR_BIT(TRISA, pin);
            }
            else
            {
                /* Setting a bit in TRISx will make the pin direction to be input */
                MASK_8BIT_SET_BIT(TRISA, pin);
                /* ST and TTL input devices are enabled */
                MASK_8BIT_CLEAR_BIT(ANSELA, pin);
            }
            break;
        case 0xB:
            if (GPIO_OUTPUT_PIN == pinDirection)
            {
                /* Clearing a bit in TRISx will make the pin direction to be output */
                MASK_8BIT_CLEAR_BIT(TRISB, pin);
            }
            else
            {
                /* Setting a bit in TRISx will make the pin direction to be input */
                MASK_8BIT_SET_BIT(TRISB, pin);
                /* ST and TTL input devices are enabled */
                MASK_8BIT_CLEAR_BIT(ANSELB, pin);
            }
            break;
        case 0xC:
            if (GPIO_OUTPUT_PIN == pinDirection)
            {
                /* Clearing a bit in TRISx will make the pin direction to be output */
                MASK_8BIT_CLEAR_BIT(TRISC, pin);
            }
            else
            {
                /* Setting a bit in TRISx will make the pin direction to be input */
                MASK_8BIT_SET_BIT(TRISC, pin);
                /* ST and TTL input devices are enabled */
                MASK_8BIT_CLEAR_BIT(ANSELC, pin);
            }
            break;
    }
}

void GPIO_vSetPinLevel(GPIO_tPortPin targetPin, STD_LogicLevel pinLevel)
{
    uint8_t port = MASK_8BIT_GET_MSB_HALF(targetPin);
    uint8_t pin = MASK_8BIT_GET_LSB_HALF(targetPin);
    switch (port)
    {
        case 0xA:
            if (STD_HIGH == pinLevel)
            {
                /* Setting a bit in PORTx will drive the pin high */
                MASK_8BIT_SET_BIT(LATA, pin);
            }
            else
            {
                /* Clearing a bit in PORTx will drive the pin low */
                MASK_8BIT_CLEAR_BIT(LATA, pin);
            }
            break;
        case 0xB:
            if (STD_HIGH == pinLevel)
            {
                /* Setting a bit in PORTx will drive the pin high */
                MASK_8BIT_SET_BIT(LATB, pin);
            }
            else
            {
                /* Clearing a bit in PORTx will drive the pin low */
                MASK_8BIT_CLEAR_BIT(LATB, pin);
            }
            break;
        case 0xC:
            if (STD_HIGH == pinLevel)
            {
                /* Setting a bit in PORTx will drive the pin high */
                MASK_8BIT_SET_BIT(LATC, pin);
            }
            else
            {
                /* Clearing a bit in PORTx will drive the pin low */
                MASK_8BIT_CLEAR_BIT(LATC, pin);
            }
            break;
    }
}

STD_LogicLevel GPIO_ui8GetPinLevel(GPIO_tPortPin targetPin)
{
    uint8_t port = MASK_8BIT_GET_MSB_HALF(targetPin);
    uint8_t pin = MASK_8BIT_GET_LSB_HALF(targetPin);
    STD_LogicLevel returnValue = STD_UNKNOWN;
    switch (port)
    {
        case 0xA:
            if (0u == MASK_8BIT_GET_BIT(PORTA, pin))
            {
                returnValue = STD_LOW;
            }
            else
            {
                returnValue = STD_HIGH;
            }
            break;
        case 0xB:
            if (0u == MASK_8BIT_GET_BIT(PORTB, pin))
            {
                returnValue = STD_LOW;
            }
            else
            {
                returnValue = STD_HIGH;
            }
            break;
        case 0xC:
            if (0u == MASK_8BIT_GET_BIT(PORTC, pin))
            {
                returnValue = STD_LOW;
            }
            else
            {
                returnValue = STD_HIGH;
            }
            break;
    }
    return returnValue;
}
/*----------------------------------------------------------------------------*/
/*                     Implementation of local functions                      */
/*----------------------------------------------------------------------------*/
