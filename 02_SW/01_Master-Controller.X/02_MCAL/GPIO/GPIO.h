/* 
 * File:   GPIO.h
 * Author: Alex
 *
 * Created on February 15, 2019, 8:08 PM
 */

#ifndef GPIO_H
#define	GPIO_H

/*----------------------------------------------------------------------------*/
/*                                  Includes                                  */
/*----------------------------------------------------------------------------*/
#include "../../03_Common/types.h"
/*----------------------------------------------------------------------------*/
/*                             Defines and macros                             */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*                                 Data types                                 */
/*----------------------------------------------------------------------------*/
typedef uint8_t GPIO_tPortPin;

typedef enum {
    GPIO_OUTPUT_PIN = 0u,
    GPIO_INPUT_PIN,
} GPIO_tPinDirectionType;
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
 * \brief     This function sets the direction of the pin;
 * \param     targetPin - 8bits value, ex: 0xC3 = PORT C, pin 3;
 * \return    None 
 */
void GPIO_vSetPinDirection(GPIO_tPortPin targetPin, GPIO_tPinDirectionType pinDirection);

/**
 * \brief     This function sets the logic level on a pin;
 * \param     targetPin - 8bits value, ex: 0xC3 = PORT C, pin 3;
 * \return    None 
 */
void GPIO_vSetPinLevel(GPIO_tPortPin targetPin, STD_LogicLevel pinLevel);

/**
 * \brief     This function returns the logic level on a pin;
 * \param     targetPin - 8bits value, ex: 0xC3 = PORT C, pin 3;
 * \return    the logic level on a pin (STD_LogicLevel)
 */
STD_LogicLevel GPIO_ui8GetPinLevel(GPIO_tPortPin targetPin);
#endif	/* GPIO_H */

