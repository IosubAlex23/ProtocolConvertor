/* 
 * File:   types.h
 * Author: Alex
 *
 * Created on February 15, 2019, 8:25 PM
 */

#ifndef TYPES_H
#define	TYPES_H

/*----------------------------------------------------------------------------*/
/*                                  Includes                                  */
/*----------------------------------------------------------------------------*/
#include "../mcc_generated_files/mcc.h"
/*----------------------------------------------------------------------------*/
/*                             Defines and macros                             */
/*----------------------------------------------------------------------------*/
/* This is a mask used to clear the MS 4 bites of a byte */
#define MASK_4MSB_CLEAR                                  (0x0F)
#define MASK_8BIT_HALF                                   (4u)
#define MASK_FOR_CLEARING_BIT(bitPos)                    (~(1u << bitPos))
#define MASK_FOR_SETTING_BIT(bitPos)                     (1u << bitPos)
/* This returns the MS 4 bites of a byte */
#define MASK_8BIT_GET_MSB_HALF(x)                       (x >> MASK_8BIT_HALF)
/* This returns the LS 4 bites of a byte */
#define MASK_8BIT_GET_LSB_HALF(x)                       (x & MASK_4MSB_CLEAR)
#define MASK_8BIT_CLEAR_BIT(reg, bitPosition)           (reg = reg & MASK_FOR_CLEARING_BIT(bitPosition))
#define MASK_8BIT_SET_BIT(reg, bitPosition)             (reg = reg | MASK_FOR_SETTING_BIT(bitPosition))
#define MASK_8BIT_GET_BIT(reg, bitPosition)             (reg & MASK_FOR_SETTING_BIT(bitPosition))
/*----------------------------------------------------------------------------*/
/*                                 Data types                                 */
/*----------------------------------------------------------------------------*/

typedef enum
{
    STD_LOW = 0u,
    STD_HIGH,
    STD_UNKNOWN,
}STD_LogicLevel;

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
 * \brief     This function [...];
 * \param     None
 * \return    None 
 */

#endif	/* TYPES_H */

